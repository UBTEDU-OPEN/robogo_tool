#include "perspective_controller.h"
#include "perspective.h"
#include "camera_handler.h"
#include "operation_guide.h"
#include "guide_step.h"
#include "toast_manager.h"
#include "ota/ota_handler.h"
#include "ucar_device.h"
#include "setting.h"
#include "PerspectiveCalibration.h"
#include <QTimer>
#include <QLoggingCategory>
#include <QPainter>
#include <QPoint>
#include <algorithm>

Q_LOGGING_CATEGORY(perspective_controller, "perspective_controller")

using namespace cv;

PerspectiveController::PerspectiveController(Perspective *view, QObject *parent)
    : QObject(parent),
      m_view(view),
      m_handler(new CameraHandler),
      m_temp_point(-1,-1),
      m_scale_x(1.0),
      m_scale_y(1.0),
      m_notify(view)
{
    QTimer::singleShot(100, this, &PerspectiveController::onStartHandler);
    connect (&m_notify, &Notify::backButtonClicked, this, &PerspectiveController::onBackButtonClicked);
    connect(&m_timer, &QTimer::timeout, this, &PerspectiveController::onTimeout);
    m_timer.setInterval(180*1000);
    m_timer.start();

    connect(&m_camera_dog, &QTimer::timeout, this, &PerspectiveController::onDogBite);
    m_camera_dog.setInterval(100);

    QSize live_size = m_view->getLiveWidgetSize();

    m_scale_x = 640.0 / live_size.width();
    m_scale_y = 480.0 / live_size.height();

    QList<double> matrix_list = Setting::instance().getMatrix();
    if (matrix_list.size() >= 9)
    {
        m_matrix = (Mat_<double>(3, 3) <<
                matrix_list[0] * 2, matrix_list[1], matrix_list[2] *2 ,
                matrix_list[3], matrix_list[4] * 2, matrix_list[5] * 2,
                matrix_list[6], matrix_list[7], matrix_list[8]);
    }

    QList<double> coef_list = Setting::instance().getCoef();
    if (coef_list.size() >= 5)
    {
        m_coef = (Mat_<double>(1, 5) <<
                  coef_list[0], coef_list[1], coef_list[2], coef_list[3], coef_list[4]);
    }

    setDistCoeff();
}

PerspectiveController::~PerspectiveController()
{

}

void PerspectiveController::onBackButtonClicked()
{
    qCInfo(perspective_controller()) << __func__;

    if (m_camera_dog.isActive())
        m_camera_dog.stop();

    if (m_handler)
    {
        disconnect(m_handler.get(), 0, this, 0);
        m_handler->stop();
        m_handler->wait();
    }
    emit m_view->exitCalibration();
    emit m_view->backToPreviousView();

}

void PerspectiveController::onGuideButtonClicked()
{
    qCInfo(perspective_controller()) << __func__;
    OperationGuide *guide = new OperationGuide(m_view);
    guide->setTitle(tr("摄像头标定操作说明"));

    std::vector<GuideStep* > steps;

    if (m_view->isManualMode())
    {
        GuideStep *step_1 = new GuideStep(1, MANUAL_GUIDE_TEXT, GuideStep::Image, ":/ui/images/img_perspective03.png", guide, GuideStep::Single);
        steps.push_back(step_1);
    }
    else
    {
        GuideStep *step_1 = new GuideStep(1, STEP1_TEXT, GuideStep::Image, ":/ui/images/img_perspective01.png", guide, GuideStep::First);
        steps.push_back(step_1);
        GuideStep *step_2 = new GuideStep(2, STEP2_TEXT, GuideStep::Image, ":/ui/images/img_perspective02.png", guide, GuideStep::Last);
        steps.push_back(step_2);
    }

    guide->initGuideSteps(steps);

//    QPoint global_pos = m_view->mapToGlobal(m_view->pos());
//    guide->move(global_pos.x() + m_view->width()/2 - guide->width()/2,
//                global_pos.y() + m_view->height()/2 - guide->height()/2);
    guide->exec();
}

void PerspectiveController::onCalibrationDone()
{
    qCInfo(perspective_controller()) << __func__;
    m_camera_dog.stop();
    m_notify.exec();
}

void PerspectiveController::onTimeout()
{
    qCInfo(perspective_controller()) << __func__;
//    ToastManager::instance().show("请按照操作说明调整小车位置！");
    m_camera_dog.stop();
    m_handler->pause();
    Notify notify(m_view);
    notify.setMessage(tr("自动标定时间过长？建议尝试手动标定！"));
    notify.setBtnText(tr("确定"));
    notify.exec();
    m_handler->resume();
    m_camera_dog.start();
}

void PerspectiveController::onCameraDisconnected()
{
    qCInfo(perspective_controller()) << __func__;
    emit updateText(tr("未检测到摄像头，请检查摄像头连接"));
}

void PerspectiveController::onStartHandler()
{
    qCDebug(perspective_controller())  << __func__ ;
    bool ret = this->m_handler->start();
    if(!ret)
    {
        qCWarning(perspective_controller())  << __func__ <<  " Failed to start image handler";
        // todo: notify
    }
    else{
        m_handler->moveToThread(m_handler.get());
        connect(m_handler.get(), &CameraHandler::newFrame,
                this, &PerspectiveController::onNewFrame);
        connect(m_handler.get(), &CameraHandler::calibrationDone,
                this, &PerspectiveController::onCalibrationDone);
        connect(m_handler.get(), &CameraHandler::cameraDisconnected,
                this, &PerspectiveController::onCameraDisconnected);
//        connect(m_handler.get(), &PerspectiveImageHandler::finished,
//                this, &PerspectiveController::onStartHandler);
        connect(m_handler.get(), &CameraHandler::heartBeat,
                this, &PerspectiveController::onHandlerHeartBeat, Qt::QueuedConnection);
    }
}

void PerspectiveController::onDogBite()
{
    qCWarning(perspective_controller())  << __func__ ;
    disconnect(m_handler.get(), 0, this, 0);
    m_camera_dog.stop();
    m_handler->stop();
    m_handler->wait();
    onCameraDisconnected();
    m_handler = nullptr;
    m_handler = std::make_shared<CameraHandler>();
    onStartHandler();
}

void PerspectiveController::onHandlerHeartBeat()
{
    m_camera_dog.start();
}

void PerspectiveController::onAutoClicked()
{
    qCInfo(perspective_controller()) << __func__;
    if (!m_view->isManualMode())
    {
        return ;
    }

    m_timer.start();
    m_view->setManualMode(false);
    m_temp_point = QPoint(-1, -1);
}

void PerspectiveController::onManualClicked()
{
    qCInfo(perspective_controller()) << __func__;

    if (m_view->isManualMode())
    {
        return ;
    }

    m_selected_points.clear();
    m_timer.stop();
    m_view->setManualMode(true);
}

void PerspectiveController::onPointSelected(QPoint point)
{
    qCInfo(perspective_controller()) << __func__ << "Point -> " << point;
    uint8_t point_count = m_selected_points.size();
    qCInfo(perspective_controller()) << __func__ << "Point count -> " << point_count;
    if (point_count >= POINTS_AMOUNT_MAX)
    {
        return ;
    }
    m_temp_point = point;
}

void PerspectiveController::setLiveGuideMessage(QPixmap &frame, const QString &message)
{
    QPainter painter(&frame);
    QRect rect = QRect(0, 0, frame.width(), 40);
    painter.setOpacity(0.6);
    painter.setBrush(Qt::black);
    painter.drawRect(rect);
    painter.setOpacity(1.0);
    painter.setPen(Qt::white);
    painter.setFont(QFont("SourceHanSansCN", 12, QFont::Medium));
    painter.drawText(rect, Qt::AlignCenter, message);
    painter.end();
}

void PerspectiveController::onNewFrame(Mat frame)
{
    if (m_view->isManualMode())
    {
        handleFrameForManualMode(frame);
    }
    else
    {
        handleFrameForAutoMode(frame);
    }
}

void PerspectiveController::onPreviousBtnClicked()
{
    qCInfo(perspective_controller()) << __func__;

    qCInfo(perspective_controller()) << "Points in the list : " << m_selected_points;
    m_temp_point = m_selected_points.takeLast();
    qCInfo(perspective_controller()) << "Points in the list after pop : " << m_selected_points;
    uint8_t point_count = m_selected_points.size();
    if (point_count <= POINTS_AMOUNT_MAX)
    {
        m_view->setManualProgress(Perspective::ManualSteps(point_count), true);
    }
}

void PerspectiveController::onNextBtnClicked()
{
    qCInfo(perspective_controller()) << __func__;
    m_selected_points.append(m_temp_point);
    auto point_count = m_selected_points.size();
    m_temp_point = QPoint(-1, -1);
    m_view->setManualProgress(Perspective::ManualSteps(point_count));
}

void PerspectiveController::onFinishBtnClicked()
{

    qCInfo(perspective_controller()) << __func__;


    if (m_selected_points.size() != POINTS_AMOUNT_MAX)
    {
        qCWarning(perspective_controller()) << "Point count error -> " << m_selected_points;
        return ;
    }
    emit setFinishBtnEnable(false);
    reorderPoints();

    qCInfo(perspective_controller()) << "Reordered points: " << m_selected_points;

    QList<uint16_t> result_list;
    for (auto point : m_selected_points)
    {
        result_list.append(point.x() * m_scale_x / 2);
        result_list.append(point.y() * m_scale_y / 2);
    }

    UCarDevice::instance().setPerspective(result_list);

    onCalibrationDone();
    emit setFinishBtnEnable(true);
}

void PerspectiveController::drawSelectedPoints(QPixmap &frame)
{
    QPainter painter(&frame);
    painter.setOpacity(0.5);
    painter.setBrush(QColor(0x14, 0xde, 0x32));
    painter.setPen(QColor(0x14, 0xde, 0x32));
    // 画已经确认的点
    for (auto point : m_selected_points)
    {
        auto mapped_point = QPoint(point.x() * m_scale_x, point.y() * m_scale_y);
        painter.drawEllipse(mapped_point, POINT_RADIUS, POINT_RADIUS);
    }

    // 画临时选中的未确认点
    if (m_temp_point != QPoint(-1, -1))
    {
        auto mapped_point = QPoint(m_temp_point.x() * m_scale_x, m_temp_point.y() * m_scale_y);
        painter.drawEllipse(mapped_point, POINT_RADIUS, POINT_RADIUS);
    }
    painter.end();
}

void PerspectiveController::reorderPoints()
{
    qCInfo(perspective_controller()) << __func__;
    auto point_count = m_selected_points.size();

    qCInfo(perspective_controller()) << "Point count : " << point_count;
    if (point_count != POINTS_AMOUNT_MAX)
    {
        qCInfo(perspective_controller()) << "Point count error";
        return ;
    }

    // 按y坐标升序排序
    for (auto i = 0; i < point_count; ++i)
    {
        for (auto j = point_count - 1; j > i; --j)
        {
            if (m_selected_points[j].y() < m_selected_points[j-1].y())
            {
                std::swap(m_selected_points[j], m_selected_points[j-1]);
            }
        }
    }

    // 前两个点和后两个点分别按x坐标排序
    if (m_selected_points[0].x() > m_selected_points[1].x())
    {
        std::swap(m_selected_points[0], m_selected_points[1]);
    }

    if (m_selected_points[2].x() > m_selected_points[3].x())
    {
        std::swap(m_selected_points[2], m_selected_points[3]);
    }
}

void PerspectiveController::setDistCoeff()
{
    cv::Mat R = cv::Mat::eye(3, 3, CV_32F);
    //初始化设置：相机畸变参数设置
    initUndistortRectifyMap(m_matrix, m_coef, R, m_matrix,
//                            cv::Size(320, 240), CV_32FC1, m_mapx, m_mapy);
                            cv::Size(640, 480), CV_32FC1, m_mapx, m_mapy);
}

void PerspectiveController::distortionCorr(const cv::Mat& srcImg, cv::Mat& distImg)
{
    //相机畸变矫正
    remap(srcImg, distImg, m_mapx, m_mapy, cv::INTER_LINEAR);
}

void PerspectiveController::handleFrameForAutoMode(Mat &frame)
{
    bool done = false;
    bool is_result_valid = true;

    // 执行畸变校正
    cv::Mat distImage;
    distortionCorr(frame, distImage);
    frame = distImage.clone();

    cv::cvtColor(distImage, distImage, cv::COLOR_BGR2GRAY);
    std::vector<std::vector<cv::Point>> result_contour;
    CalibrationProcess(distImage, result_contour);

    qCInfo(perspective_controller()) << "Result contour size: " << result_contour.size();

    for(auto i = 0; i < result_contour.size(); ++i)
    {
        auto v = result_contour[i];
        for (auto p : v)
        {
            qCDebug(perspective_controller()) << p.x << " " << p.y;
        }

        if((abs(result_contour[i][2].y - 430) > 4) ||
                (abs(result_contour[i][3].y - 430) > 4))
        {
            is_result_valid = false;
            break;
        }
        std::vector<cv::Point> points;
        points.push_back(result_contour[i][0]);
        points.push_back(result_contour[i][1]);
        points.push_back(result_contour[i][3]);
        points.push_back(result_contour[i][2]);
        polylines(frame, points, true, cv::Scalar(0, 255, 0), 2, cv::LINE_AA);
    }

    if (result_contour.size() > 1 && is_result_valid)
    {
        QList<uint16_t> result_list;
        if(result_contour[0][0].x < result_contour[1][0].x)
        {
            // result_contour[0]在左边
            result_list.append(result_contour[0][0].x / 2);
            result_list.append(result_contour[0][0].y / 2);
            result_list.append(result_contour[1][1].x / 2);
            result_list.append(result_contour[1][1].y / 2);
            result_list.append(result_contour[0][2].x / 2);
            result_list.append(result_contour[0][2].y / 2);
            result_list.append(result_contour[1][3].x / 2);
            result_list.append(result_contour[1][3].y / 2);

        }
        else
        {
            // result_contour[1]在左边
            result_list.append(result_contour[1][0].x / 2);
            result_list.append(result_contour[1][0].y / 2);
            result_list.append(result_contour[0][1].x / 2);
            result_list.append(result_contour[0][1].y / 2);
            result_list.append(result_contour[1][2].x / 2);
            result_list.append(result_contour[1][2].y / 2);
            result_list.append(result_contour[0][3].x / 2);
            result_list.append(result_contour[0][3].y / 2);
        }

        UCarDevice::instance().setPerspective(result_list);

        done = true;
    }

    line(frame, Point(320,0), Point(320, 480), Scalar(0, 0, 255), 1, LINE_AA);
    line(frame, Point(0, 430), Point(640, 430), Scalar(0, 0, 255), 1, LINE_AA);

    cvtColor(frame, frame, COLOR_BGR2RGB);
    QImage img = QImage((const unsigned char*)(frame.data), frame.cols, frame.rows, QImage::Format_RGB888);
    emit updateLiveFrame(QPixmap::fromImage(img));

    if(done)
    {
        onCalibrationDone();
    }
}

void PerspectiveController::handleFrameForManualMode(cv::Mat &frame)
{
    // 执行畸变校正
    cv::Mat distImage;
    distortionCorr(frame, distImage);
    frame = distImage;

    line(frame, Point(320,0), Point(320, 480), Scalar(0, 0, 255), 1, LINE_AA);
    line(frame, Point(0, 430), Point(640, 430), Scalar(0, 0, 255), 1, LINE_AA);
    QImage img = QImage((const unsigned char*)(frame.data), frame.cols, frame.rows, QImage::Format_RGB888);

    cvtColor(frame, frame, COLOR_BGR2RGB);
    QPixmap pix = QPixmap::fromImage(img);
    // 显示画面顶部提示信息
    uint8_t point_count = m_selected_points.size();
    if (point_count <= POINTS_AMOUNT_MAX)
    {
        setLiveGuideMessage(pix, gGuideMessageMap[point_count]);
    }

    // 显示已选的点
    if (point_count > 0 || m_temp_point != QPoint(-1, -1))
    {
        drawSelectedPoints(pix);
    }
    emit updateLiveFrame(pix);
}
