#include "camera_param_cali_controller.h"
#include "camera_parameter_calibration.h"
#include "camera_handler.h"
#include "notify.h"
#include "camera_parameter_guide.h"
#include "setting.h"
#include "toast_manager.h"
#include <opencv2/core.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/calib3d.hpp"
#include <opencv2/highgui.hpp>
#include <QLoggingCategory>
#include <QPixmap>
#include <QPainter>
#include <QDir>
#include <QBuffer>

Q_LOGGING_CATEGORY(cam_param_controller, "cam_param_controller")

using namespace cv;
using namespace std;


CameraParamCaliController::CameraParamCaliController(CameraParameterCalibration *view, QObject *parent)
    : QObject(parent),
      m_view(view),
      m_handler(new CameraHandler),
      m_step(0),
      m_is_calibrating(true),
      m_board_size(9, 6)
{
    QTimer::singleShot(100, this, &CameraParamCaliController::onStartHandler);
    connect(&m_camera_dog, &QTimer::timeout, this, &CameraParamCaliController::onDogBite);
    m_camera_dog.setInterval(100);

    for(int j = 0;j<m_board_size.height;j++){
         for(int k = 0; k<m_board_size.width;k++){
             m_world_points.push_back(Point3f(j*1.0 ,k*1.0 ,0.0f));
         }
     }
}

CameraParamCaliController::~CameraParamCaliController()
{
}

void CameraParamCaliController::onCaptureBtnPressed()
{
    qCInfo(cam_param_controller()) << __func__;
    m_view->setCaptureBtnEnable(false);
    m_view->setLiveMaskEnable(true);

    QTimer::singleShot(1, this, &CameraParamCaliController::handleImage);

    /*
    if (handleImage(frame))
    {
        m_step++;
    }
    else
    {
        Notify notify(m_view);
        QPixmap pic(":/ui/images/ic_error_sign.png");
        QByteArray ba;
        QBuffer buffer(&ba);
        pic.save(&buffer, "PNG");
        QString text = QString("<img src=\"data:image/png;base64,>") + ba.toBase64() + "\"/>";
        text += QString("<p>本次拍摄结果不可用，</p><p>请尝试按照说明和参考调整标定板倾斜角度后重新拍摄。</p>");
        notify.setMessage(text);
        notify.setBtnText("重新拍摄");
//        notify.setFixedSize(500, 300);
        notify.exec();
    }

    if (m_step >= 15)
    {
        // done
        m_view->setMaskEnable(true);
        QTimer::singleShot(1, this, &CameraParamCaliController::onCalibrating);
    }
    */
}

void CameraParamCaliController::onStartHandler()
{
    qCDebug(cam_param_controller())  << __func__ ;
    m_view->setCaptureBtnEnable(false);
    bool ret = this->m_handler->start();
    if(!ret)
    {
        qCWarning(cam_param_controller())  << __func__ <<  " Failed to start image handler";
        // todo: notify
    }
    else
    {
        m_handler->moveToThread(m_handler.get());
        connect(m_handler.get(), &CameraHandler::newFrame,
                this, &CameraParamCaliController::onNewFrame);
        connect(m_handler.get(), &CameraHandler::cameraDisconnected,
                this, &CameraParamCaliController::onCameraDisconnected);
        connect(m_handler.get(), &CameraHandler::heartBeat,
                this, &CameraParamCaliController::onHandlerHeartBeat, Qt::QueuedConnection);
    }
}

void CameraParamCaliController::onCameraDisconnected()
{
    qCInfo(cam_param_controller()) << __func__;
    emit updateText(tr("未检测到摄像头，请检查摄像头连接"));
    m_view->setCaptureBtnEnable(false);
}

void CameraParamCaliController::onDogBite()
{
    qCWarning(cam_param_controller())  << __func__ ;
    disconnect(m_handler.get(), 0, this, 0);
    m_camera_dog.stop();
    m_handler->stop();
    m_handler->wait();
    onCameraDisconnected();
    m_handler = nullptr;
    m_handler = std::make_shared<CameraHandler>();
    onStartHandler();
}

void CameraParamCaliController::onHandlerHeartBeat()
{
    m_camera_dog.start();
}

void CameraParamCaliController::onNewFrame(Mat frame)
{
    if (!m_is_calibrating)
    {
        return;
    }

    {
        lock_guard<mutex> guard(m_frame_mutex);
        m_current_frame = frame.clone();
    }
    // 镜像显示图片
    flip(frame, frame, 1);
    cvtColor(frame, frame, COLOR_BGR2RGB);
    QImage img = QImage((const unsigned char*)(frame.data), frame.cols, frame.rows, QImage::Format_RGB888);
    QPixmap pix = QPixmap::fromImage(img);
    drawPrompt(pix);
    emit updateLiveFrame(pix);
    m_view->setCaptureBtnEnable(true);
}

void CameraParamCaliController::onBackButtonClicked()
{
    qCInfo(cam_param_controller()) << __func__;
    disconnect(m_handler.get(), 0, this, 0);
    m_camera_dog.stop();
    m_handler->stop();
    m_handler->wait();
    emit m_view->backToPreviousView();
    emit m_view->exitCalibration();
}

void CameraParamCaliController::onGuideButtonClicked()
{
    qCInfo(cam_param_controller()) << __func__;

    CameraParameterGuide guide(m_view);
    guide.exec();
}

void CameraParamCaliController::drawPrompt(QPixmap &frame)
{
#if 1
    QPoint pos(0, 0);
    QPixmap prompt_pic;
    QString text = "";
    switch (m_step % 3)
    {
    case 0:
        prompt_pic.load(":/ui/images/ic_calibration_para.png");
        text = tr("    标定板平行");
        break;
    case 1:
        prompt_pic.load(":/ui/images/ic_calibration_left.png");
        text = tr("标定板向左倾斜");
        break;
    case 2:
        prompt_pic.load(":/ui/images/ic_calibration_right.png");
        text = tr("标定板向右倾斜");
        break;
    }
    switch (m_step / 3)
    {
    case 0:
        pos = QPoint(0, 0);
        break;
    case 1:
        pos = QPoint(frame.width() - prompt_pic.width(), 0);
        break;
    case 2:
        pos = QPoint(0, frame.height() - prompt_pic.height());
        break;
    case 3:
        pos = QPoint(frame.width() - prompt_pic.width(), frame.height() - prompt_pic.height());
        break;
    case 4:
        pos = QPoint((frame.width() - prompt_pic.width())/2, (frame.height() - prompt_pic.height())/2);
        break;
    default:
        break;
    }

    QPainter painter(&frame);
    painter.drawPixmap(pos, prompt_pic);
    painter.setPen(QPen("#FF625A"));//设置画笔形式
    painter.setFont(QFont("SourceHanSansCN", 14, QFont::Normal));
    painter.drawText(prompt_pic.width()/2 - 70 + pos.x(), prompt_pic.height()/2 + pos.y(), text);
    painter.end();
#else
    //以下是全部由QPainter绘制

    int nFrameWidth = 280;
    int nFrameHeight = 190;
    int nTilt = 15;

    switch (m_step / 3) //需要拍照5次
    {
    case 0://左上角
        pos = QPoint(0, 0);
        break;
    case 1://右上角
        pos = QPoint(frame.width() - nFrameWidth, 0);
        break;
    case 2://左下角
        pos = QPoint(0, frame.height() - nFrameHeight);
        break;
    case 3://右下角
        pos = QPoint(frame.width() - nFrameWidth, frame.height() - nFrameHeight);
        break;
    case 4://中间
        pos = QPoint((frame.width() - nFrameWidth)/2, (frame.height() - nFrameHeight)/2);
        break;
    default:
        break;
    }

    QPoint topLeft;
    QPoint topRight;
    QPoint BottonRight;
    QPoint BottonLeft;
    QPainter painter(&frame);
    QString text = "";
    switch (m_step % 3)//绘制标定板边框
    {
        case 0://中
            topLeft  = pos;
            topRight = pos + QPoint(nFrameWidth, 0);
            BottonRight = pos + QPoint(nFrameWidth, nFrameHeight);
            BottonLeft = pos + QPoint(0, nFrameHeight);
            text = tr("   标定板平行");
            break;
        case 1://左
            topLeft  = pos + QPoint(0, nTilt);
            topRight = pos + QPoint(nFrameWidth, 0);
            BottonRight = pos + QPoint(nFrameWidth, nFrameHeight);
            BottonLeft = pos + QPoint(0, nFrameHeight - nTilt);
            text = tr("标定板向左倾斜");
            break;
        case 2://右
            topLeft  = pos;
            topRight = pos + QPoint(nFrameWidth, nTilt);
            BottonRight = pos + QPoint(nFrameWidth, nFrameHeight - nTilt);
            BottonLeft = pos + QPoint(0, nFrameHeight);
            text = tr("标定板向右倾斜");
            break;
    }
    painter.setPen(QPen(Qt::red, 3));//设置画笔形式
    const QPoint points[4] = {topLeft, topRight, BottonRight, BottonLeft};
    painter.drawPolygon(points, 4);

    painter.setFont(QFont("SourceHanSansCN", 12, QFont::Medium));
    painter.drawText(nFrameWidth/2 - 45 + pos.x(), nFrameHeight/2 + pos.y(), text);

#endif
}

void CameraParamCaliController::handleImage()
{
    qCInfo(cam_param_controller()) << __func__;
    Mat frame;
    {
        lock_guard<mutex> guard(m_frame_mutex);
        frame = m_current_frame.clone();
    }

    vector<Point2f> corners;
    bool found = findChessboardCorners(frame, m_board_size, corners,
        CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_FAST_CHECK | CALIB_CB_NORMALIZE_IMAGE);
    m_view->setLiveMaskEnable(false);
    if (found)
    {
        m_corners_3d.push_back(corners);
        //画出角点
        drawChessboardCorners(frame, m_board_size,corners, found);

        m_images.push_back(frame);
        m_world_points_3d.push_back(m_world_points);
        m_step++;
    }
    else
    {
        Notify notify(m_view);
        QPixmap pic(":/ui/images/ic_error_sign.png");
        QByteArray ba;
        QBuffer buffer(&ba);
        pic.save(&buffer, "PNG");
        QString text = QString("<img src=\"data:image/png;base64,>") + ba.toBase64() + "\"/>";
        text += QString(tr("<p>本次拍摄结果不可用，</p><p>请尝试按照说明和参考调整标定板倾斜角度后重新拍摄。</p>"));
        notify.setMessage(text);
        notify.setBtnText(tr("重新拍摄"));
        notify.exec();
    }

    if (m_step >= 15)
    {
        // done
        m_view->setMaskEnable(true);
        QTimer::singleShot(0, this, &CameraParamCaliController::onCalibrating);
    }
}

void CameraParamCaliController::onSaveButtonClicked()
{
    qCInfo(cam_param_controller()) << __func__;
    m_view->setSaveBtnEnable(false);
    bool ret = Setting::instance().setMatrixAndCoef(m_matrix_list, m_coef_list);
    if (ret)
    {
        ToastManager::instance().show(tr("导入数据成功！"));
        QDir dir;
        QString pic_dir = Setting::instance().getCameraPicFolderPath();
        // 保存图片
        if (!dir.exists(pic_dir))
        {
            ret = dir.mkpath(pic_dir);
            if (!ret)
            {
                ToastManager::instance().show(tr("保存图片失败！"));
                return ;
            }
        }

        int i = 1;
        foreach (auto image, m_images)
        {
            imwrite(QString("%1/%2.png").arg(pic_dir, QString::number(i++)).toStdString().c_str(), image);
        }
    }
    else
    {
        ToastManager::instance().show(tr("导入数据失败！"));
    }
}

void CameraParamCaliController::onCancelButtonClicked()
{
    qCInfo(cam_param_controller()) << __func__;
    m_step = 0;
    m_matrix_list.clear();
    m_coef_list.clear();
    m_images.clear();
    m_view->showCalibration();
    m_is_calibrating = true;
}

void CameraParamCaliController::showResult()
{
    qCInfo(cam_param_controller()) << __func__;

    m_is_calibrating = false;
    QString text;
    text.append(QString("<p>camera matrix:</p>"));
    if (m_matrix_list.size() >= 9)
    {
        for (auto i = 0; i < 3; ++i)
        {
            text.append(QString("<p>[%1  %2  %3]</p>").arg(
                            QString::number(m_matrix_list[i*3]), QString::number(m_matrix_list[i*3 + 1]), QString::number(m_matrix_list[i*3 + 2])));
        }
    }

    if (m_coef_list.size() >= 5)
    {
        text.append(QString("<p>distortion coefficients: [%1  %2  %3  %4  %5]</p>").arg(
                        QString::number(m_coef_list[0]), QString::number(m_coef_list[1]), QString::number(m_coef_list[2]),
                QString::number(m_coef_list[3]), QString::number(m_coef_list[4])));
    }
    m_view->setResultText(text);

    m_view->setSaveBtnEnable(true);
    m_view->showResult();
}

void CameraParamCaliController::onCalibrating()
{
    Mat camera_matrix = Mat::eye(3, 3, CV_64F);
    Mat dist_coeffs = Mat::zeros(8, 1, CV_64F);;
    vector<Mat> rvecs, tvecs;
    int iFixedPoint = -1;
    vector<Point3f> newObjPoints;
    float squareSize = 0.025;
    float grid_width = squareSize * (m_board_size.width - 1);

    vector<vector<Point3f> > objectPoints(1);

    for( int i = 0; i < m_board_size.height; i++ )
        for( int j = 0; j < m_board_size.width; j++ )
            objectPoints[0].push_back(Point3f(float(j*squareSize),
                                      float(i*squareSize), 0));

    objectPoints[0][m_board_size.width - 1].x = objectPoints[0][0].x + grid_width;
    newObjPoints = objectPoints[0];

    double rms = calibrateCameraRO(m_world_points_3d, m_corners_3d, m_current_frame.size(), iFixedPoint,
                            camera_matrix, dist_coeffs, rvecs, tvecs, newObjPoints,
                            CALIB_FIX_K3 | CALIB_USE_LU);


    qCDebug(cam_param_controller())  << " Camera intrinsic: " << camera_matrix.rows << "x" << camera_matrix.cols;
    for (auto r = 0; r < camera_matrix.rows; ++r)
    {
        qCDebug(cam_param_controller())  << camera_matrix.at<double>(r,0)/2.0 << " " << camera_matrix.at<double>(r,1)/2.0 << " " << camera_matrix.at<double>(r,2)/2.0;
        for(auto c = 0; c < camera_matrix.cols; ++c)
        {
            if(r != 2)
            {
                m_matrix_list.append(camera_matrix.at<double>(r,c)/2.0);
            }
            else
            {
                m_matrix_list.append(camera_matrix.at<double>(r,c));
            }
        }
    }

    qCDebug(cam_param_controller())  << dist_coeffs.rows << "x" <<dist_coeffs.cols;
    for(int i = 0;i < dist_coeffs.rows;i++)
    {
        qCDebug(cam_param_controller())  << dist_coeffs.at<double>(0,i) << " " ;
        m_coef_list.append(dist_coeffs.at<double>(0, i));
    }

    m_view->setMaskEnable(false);
    showResult();
}
