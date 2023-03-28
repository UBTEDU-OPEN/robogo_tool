#ifndef CAMERA_H
#define CAMERA_H

#include <opencv2/videoio.hpp>
#include <QObject>

class Camera : public QObject
{
public:
    Camera();
    ~Camera();

    bool open(const uint16_t width, const uint16_t height);
    void close();
    bool read(cv::Mat &frame);
    bool isOpened();

private:
    cv::VideoCapture m_cap;
};


#endif // CAMERA_H
