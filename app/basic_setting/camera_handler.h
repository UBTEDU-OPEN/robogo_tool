#ifndef CAMERA_HANDLER_H
#define CAMERA_HANDLER_H

#include <QThread>
#include <QTimer>
#include <QPixmap>
#include <memory>
#include <opencv2/opencv.hpp>
#include <mutex>

class Camera;

class CameraHandler : public QThread
{
    Q_OBJECT
public:
    CameraHandler();
    ~CameraHandler();

    bool start();
    void stop();
    void pause();
    void resume();

signals:
    void newFrame(cv::Mat frame);
    void calibrationDone();
    void cameraDisconnected();
    void cameraConnected();
    void heartBeat();
    void stopTimer();
    void startTimer();

    // QThread interface
protected:
    void run();

private slots:
    void frameHandler();

private:
    void stopTimerFromAnyThread();
    void startTimerFromAnyThread();

    QTimer m_timer;
    std::unique_ptr<Camera> m_camera;
    std::mutex m_mutex;
};


#endif // CAMERA_HANDLER_H
