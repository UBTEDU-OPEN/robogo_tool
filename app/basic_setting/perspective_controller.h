#ifndef PERSPECTIVE_CONTROLLER_H
#define PERSPECTIVE_CONTROLLER_H

#include <QObject>
#include <QPixmap>
#include <memory>
#include <QTimer>
#include <opencv2/opencv.hpp>
#include "notify.h"
#include <QMap>

class Perspective;
class CameraHandler;

class PerspectiveController : public QObject
{
    Q_OBJECT

    QString STEP1_TEXT{QObject::tr("将小车放置在虚线位置内，微调摄像头角度，使摄像头实时画面中的两条红线与地图矩形框内的水平和垂直的两条白线重合。")};
    QString STEP2_TEXT{QObject::tr("当实时画面中的两条红线与地图矩形框内的两条白线重合时，摄像头进行自动标定，等待地图矩形框全部变绿，即标定成功。")};
    QString MANUAL_GUIDE_TEXT{QObject::tr("    请点击选出画面内地图白色矩形框的四个角，确认后点击\“完成\”，完成手动标定，（如图）")};
    const uint8_t POINTS_AMOUNT_MAX{4};
    const uint8_t POINT_RADIUS{25};
    const QMap<uint8_t, QString> gGuideMessageMap{
        {0, QObject::tr("请选择第一个矩形角")},
        {1, QObject::tr("请选择第二个矩形角")},
        {2, QObject::tr("请选择第三个矩形角")},
        {3, QObject::tr("请选择第四个矩形角")},
        {4, QObject::tr("点击“完成”，完成手动标定")}
    };

public:
    explicit PerspectiveController(Perspective* view, QObject *parent = nullptr);
    ~PerspectiveController();

signals:
    void updateLiveFrame(QPixmap frame);
    void updateText(QString text);
    void setFinishBtnEnable(bool bEnable);

public slots:
    void onBackButtonClicked();
    void onGuideButtonClicked();
    void onAutoClicked();
    void onManualClicked();
    void onPointSelected(QPoint point);
    void onPreviousBtnClicked();
    void onNextBtnClicked();
    void onFinishBtnClicked();

private slots:
    void onCalibrationDone();
    void onTimeout();
    void onCameraDisconnected();
    void onStartHandler();
    void onDogBite();
    void onHandlerHeartBeat();
    void onNewFrame(cv::Mat frame);

private:
    void setLiveGuideMessage(QPixmap &frame, const QString &message);
    void drawSelectedPoints(QPixmap &frame);
    void reorderPoints();
    void setDistCoeff();
    void distortionCorr(const cv::Mat& srcImg, cv::Mat& distImg);
    void handleFrameForAutoMode(cv::Mat &frame);
    void handleFrameForManualMode(cv::Mat &frame);

    Perspective* m_view;
    std::shared_ptr<CameraHandler> m_handler;
    QTimer m_timer;
    QTimer m_camera_dog;
    QVector<QPoint> m_selected_points;
    QPoint m_temp_point;
    float m_scale_x;
    float m_scale_y;
    cv::Mat m_matrix;
    cv::Mat m_coef;
    cv::Mat m_mapx;
    cv::Mat m_mapy;
    Notify m_notify;
};

#endif // PERSPECTIVE_CONTROLLER_H
