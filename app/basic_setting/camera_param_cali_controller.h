#ifndef CAMERA_PARAM_CALI_CONTROLLER_H
#define CAMERA_PARAM_CALI_CONTROLLER_H

#include <opencv2/opencv.hpp>
#include <QObject>
#include <QTimer>
#include <QPixmap>
#include <memory>
#include <mutex>
#include <atomic>

class CameraParameterCalibration;
class CameraHandler;

class CameraParamCaliController : public QObject
{
    Q_OBJECT
public:
    explicit CameraParamCaliController(CameraParameterCalibration *view, QObject *parent=nullptr);
    ~CameraParamCaliController();

    void onCaptureBtnPressed();

public slots:
    void onBackButtonClicked();
    void onGuideButtonClicked();
    void onSaveButtonClicked();
    void onCancelButtonClicked();

signals:
    void updateLiveFrame(QPixmap frame);
    void updateText(QString text);

private slots:
    void onCameraDisconnected();
    void onStartHandler();
    void onHandlerHeartBeat();
    void onNewFrame(cv::Mat frame);
    void onDogBite();
    void onCalibrating();
    void handleImage();

private:
    void drawPrompt(QPixmap &frame);
    void showResult();

    CameraParameterCalibration *m_view;
    std::shared_ptr<CameraHandler> m_handler;
    QTimer m_camera_dog;
    uint8_t m_step;
    std::vector<cv::Mat> m_images;
    std::vector<std::vector<cv::Point2f> > m_corners_3d;
    std::vector<cv::Point3f> m_world_points;
    std::vector<std::vector<cv::Point3f> > m_world_points_3d;
    cv::Mat m_current_frame;
    QList<double> m_matrix_list;
    QList<double> m_coef_list;
    std::mutex m_frame_mutex;
    std::atomic<bool> m_is_calibrating;
    cv::Size m_board_size;
};

#endif // CAMERA_PARAM_CALI_CONTROLLER_H
