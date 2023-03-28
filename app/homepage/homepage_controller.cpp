#include "homepage_controller.h"
#include "homepage/connection_guidelines.h"
#include "toast_manager.h"
#include "ucar_device.h"
#include "basic_setting/camera_parameter_guide.h"
#include <QTimer>
#include <cstdlib>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(homepage_controller, "homepage_controller")

HomepageController::HomepageController(QWidget *view)
    : m_view(view),
      m_online(false)
{
    connect(&m_timer, &QTimer::timeout, this, &HomepageController::onLostConnection);
    connect(&m_offline_repeat_timer, &QTimer::timeout, this, &HomepageController::onLostConnection);
    m_auto_guide_timer.setInterval(3000);
    m_auto_guide_timer.setSingleShot(true);
    connect(&m_auto_guide_timer, &QTimer::timeout, this, &HomepageController::onOfflineTimerTriggered);

}

void HomepageController::onGuidelinesClicked(void)
{
    qCInfo(homepage_controller()) << __func__;

    /* modify by Joker 此处貌似可以不用设置
    UCarDevice::instance().enableDistributing(false);
    if (m_timer.isActive())
    {
        m_timer.stop();
    }
    */

    emit setWindowModal(true);

    ConnectionGuidelines guide(m_view);
    guide.exec();

    emit setWindowModal(false);

    /*modify by Joker 此处貌似可以不用设置
    m_timer.start(1000);

    UCarDevice::instance().enableDistributing(true);
    */
}

void HomepageController::onReadAngleData(float x, float y, float z)
{
    qCDebug(homepage_controller()) << __func__
                                   << " x: " << x
                                   << " y: " << y
                                   << " z: " << z;
    if(!m_online)
    {
        m_online = true;
        emit updateConnectionStatus(true);
    }

    if(m_offline_repeat_timer.isActive())
    {
        m_offline_repeat_timer.stop();
    }

    // 如果界面已经跳转，不再重置timer
    if (m_view->isVisible())
    {
        m_timer.start(1000);
    }

    emit updateAngleValues(x, y, z);
}

void HomepageController::onLostConnection()
{
    qCDebug(homepage_controller()) << __func__;
    m_online = false;
    emit updateConnectionStatus(false);
    emit updateAngleValues(0, 0, 0);
    m_timer.stop();
    ToastManager::instance().show(tr("传感器未连接成功，请重新连接。"));

    m_auto_guide_timer.start();
}

void HomepageController::onViewHideEvent(void)
{
    qCInfo(homepage_controller()) << __func__;

    // 取消数据订阅
    disconnect(&UCarDevice::instance(), 0, this, 0);

    if (m_timer.isActive())
    {
        m_timer.stop();
    }

    if (m_offline_repeat_timer.isActive())
    {
        m_offline_repeat_timer.stop();
    }

    if (m_auto_guide_timer.isActive())
    {
        m_auto_guide_timer.stop();
    }
}

void HomepageController::onViewShowEvent(void)
{
    qCInfo(homepage_controller()) << __func__;

    connect(&UCarDevice::instance(), &UCarDevice::angleData, this, &HomepageController::onReadAngleData, Qt::QueuedConnection);
    UCarDevice::instance().enableDistributing(true);
    m_timer.start(1000);
}

void HomepageController::onOfflineTimerTriggered()
{
    qCInfo(homepage_controller()) << __func__;
    if (m_offline_repeat_timer.isActive())
    {
        m_offline_repeat_timer.stop();
    }

    emit setWindowModal(true);

    ConnectionGuidelines guide(m_view);
    guide.exec();

    emit setWindowModal(false);

    m_offline_repeat_timer.start(180*1000);
}
