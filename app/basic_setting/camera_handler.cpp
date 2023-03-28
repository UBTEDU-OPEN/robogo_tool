#include "camera_handler.h"
#include "camera.h"
#include "ucar_device.h"
#include <QImage>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(camera_handler, "camera_handler")

using namespace cv;
using namespace std;

namespace {
const uint16_t FRAME_WIDTH{640};
const uint16_t FRAME_HEIGHT{480};
}


CameraHandler::CameraHandler()
    : m_camera(new Camera)
{
    m_timer.setInterval(66);

    m_camera->moveToThread(this);
    m_timer.moveToThread(this);
}

CameraHandler::~CameraHandler()
{

}

void CameraHandler::frameHandler()
{
    qCDebug(camera_handler()) << __func__ << "Camera is opened -> " << m_camera->isOpened();
    lock_guard<mutex>guard(m_mutex);
    if(!m_camera->isOpened())
    {
        qCWarning(camera_handler())  << __func__ <<  " Camera not opened";

        bool ret = m_camera->open(FRAME_WIDTH, FRAME_HEIGHT);

        if(ret)
        {
            qCInfo(camera_handler())  << __func__ <<  " Camera opened";
//            emit cameraConnected();
        }
        else
        {
            emit cameraDisconnected();
            return ;
        }
    }

    emit heartBeat();

    Mat frame;
    bool ret = m_camera->read(frame);
    if(!ret)
    {
        qCWarning(camera_handler())  << __func__ <<  " Failed to read camera";
        if(m_camera->isOpened())
        {
            m_camera->close();
        }
        return ;
    }

    emit newFrame(frame);
    return;
}

bool CameraHandler::start()
{
    qCInfo(camera_handler())  << __func__;


    if(!QThread::isRunning())
    {
        qCInfo(camera_handler())  << "Thread not running , start thread";
        QThread::start();
    }
    else
    {
        qCInfo(camera_handler())  << "Thread is still running, won't start thread";
    }

    return true;
}

void CameraHandler::stop()
{
    qCInfo(camera_handler())  << __func__;
    stopTimerFromAnyThread();

    lock_guard<mutex>guard(m_mutex);
    m_camera->close();
    QThread::quit();
}

void CameraHandler::run()
{
    qCInfo(camera_handler())  << __func__;
    m_timer.dumpObjectInfo();
    disconnect(&m_timer, 0, this, 0);
    connect(this, &CameraHandler::stopTimer, this, [this]() {
       this->m_timer.stop();
    });
    connect(this, &CameraHandler::startTimer, this, [this]() {
       this->m_timer.start();
    });
    connect(&m_timer, &QTimer::timeout, this, &CameraHandler::frameHandler, Qt::QueuedConnection);
    m_timer.dumpObjectInfo();
    m_timer.start();

    exec();
}

void CameraHandler::stopTimerFromAnyThread()
{
    emit stopTimer();
}

void CameraHandler::startTimerFromAnyThread()
{
    emit startTimer();
}

void CameraHandler::pause()
{
    qCInfo(camera_handler())  << __func__;

    stopTimerFromAnyThread();
}

void CameraHandler::resume()
{
    qCInfo(camera_handler())  << __func__;

    startTimerFromAnyThread();
}
