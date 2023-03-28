#include "advanced_setting.h"
#include "ui_advanced_setting.h"
#include "advanced_setting_controller.h"


AdvancedSetting::AdvancedSetting(QWidget *parent)
    : QWidget(parent),
      m_ui(new Ui::AdvancedSettingUi)
{
    m_ui->setupUi(this);
    m_controller = std::make_unique<AdvancedSettingController>(this);

    connect(m_ui->pb_reset, &QPushButton::released, m_controller.get(), &AdvancedSettingController::onResetClicked);
    connect(m_ui->pb_guide, &QPushButton::released, m_controller.get(), &AdvancedSettingController::onInstallationGuide);
    connect(m_ui->pb_checkOta, &QPushButton::released, this, &AdvancedSetting::onPbCheckOtaClick);
    connect(m_controller.get(), &AdvancedSettingController::setWindowModal, this, &AdvancedSetting::updateModalState);
}

AdvancedSetting::~AdvancedSetting()
{

}

void AdvancedSetting::onPbCheckOtaClick()
{
    emit sigChangeOtawidget(false);
}
