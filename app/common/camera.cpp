#include "camera.h"
#include <QLoggingCategory>
#include <opencv2/opencv.hpp>

Q_LOGGING_CATEGORY(camera, "camera")

using namespace cv;

const int g_fraction = 30;

Camera::Camera()
{
    qCDebug(camera()) << __func__;
}

Camera::~Camera()
{
    close();
}

bool Camera::open(const uint16_t width, const uint16_t height)
{
    qCDebug(camera()) << __func__ << " width: " << width << " height: " << height;
    if(m_cap.isOpened())
    {
        qCDebug(camera()) << "Camera has been opened";
        return true;
    }

    for(int i = 0; i < 4; ++i)
    {
        QString strCapFile =  QString("v4l2src device=/dev/video%1 ! video/x-raw, width=%2, height=%3, framerate=%4/1 ! videoconvert !  video/x-raw, format=BGR ! appsink max_buffers=2 drop=true")
                .arg(i).arg(width).arg(height).arg(g_fraction);
        printf("%s\n", strCapFile.toStdString().data());
        m_cap.open(strCapFile.toStdString(), CAP_GSTREAMER);
        if(m_cap.isOpened())
        {
            m_cap.set(CAP_PROP_FRAME_WIDTH, width);
            m_cap.set(CAP_PROP_FRAME_HEIGHT, height);
            qCDebug(camera()) << "Camera opened";
            return true;
        }
    }

    qCritical(camera()) << "Failed to open camera";
    return false;
}

void Camera::close()
{
    qCDebug(camera()) << __func__;
    if(m_cap.isOpened())
    {
        m_cap.release();
    }
}

bool Camera::read(Mat &frame)
{
    if(!m_cap.isOpened())
    {
        qCWarning(camera()) << __func__ << " Camera not opened";
        return false;
    }

    bool ret = m_cap.read(frame);
    if(!ret)
    {
        qCWarning(camera()) << __func__ << " Failed to read frame";
    }

    return ret;
}

bool Camera::isOpened()
{
    return m_cap.isOpened();
}
