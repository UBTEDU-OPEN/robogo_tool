#include "advanced_setting_controller.h"
#include "advanced_setting.h"
#include "installation_guide.h"
#include "ucar_device.h"
#include "toast_manager.h"
#include <QLoggingCategory>
#include <assert.h>

Q_LOGGING_CATEGORY(adv_setting_ctrl, "advanced_setting_controller")

AdvancedSettingController::AdvancedSettingController(AdvancedSetting *view, QObject *parent)
    : QObject(parent),
      m_view(view)
{
    qCDebug(adv_setting_ctrl()) << __func__;
    assert(m_view);
}

AdvancedSettingController::~AdvancedSettingController()
{
    qCDebug(adv_setting_ctrl()) << __func__;
}

void AdvancedSettingController::onResetClicked()
{
    qCInfo(adv_setting_ctrl()) << __func__;
    bool ret = UCarDevice::instance().reset();
    if (ret)
    {
        qCInfo(adv_setting_ctrl()) << __func__ << "reset successfully";
        ToastManager::instance().show(tr("恢复设置成功"));
    }
    else
    {
        qCInfo(adv_setting_ctrl()) << __func__ << "reset error";
        ToastManager::instance().show(tr("恢复设置失败，请重置！"));
    }
}

void AdvancedSettingController::onInstallationGuide()
{
    qCInfo(adv_setting_ctrl()) << __func__;
    emit setWindowModal(true);
    InstallationGuide guide(m_view);
    guide.exec();
    emit setWindowModal(false);
}

void AdvancedSettingController::onCheckOta()
{
    qCInfo(adv_setting_ctrl()) << __func__;
    //处理OTA升级逻辑
//    emit setWindowModal(true);
//    InstallationGuide guide(m_view);
//    guide.exec();
//    emit setWindowModal(false);
}
