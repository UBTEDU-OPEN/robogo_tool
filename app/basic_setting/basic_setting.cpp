#include "basic_setting.h"
#include "ui_basic_setting.h"
#include "basic_setting_controller.h"
#include <QLoggingCategory>
#include <assert.h>

Q_LOGGING_CATEGORY(basic_setting, "basic_setting")

BasicSetting::BasicSetting(QWidget *parent)
    : QWidget(parent),
      m_ui(new Ui::BasicSettingUi)
{
    qCDebug(basic_setting()) << __func__;

    m_ui->setupUi(this);

    m_controller = std::make_unique<BasicSettingController>(this);

    connect(m_ui->home_page, &BasicSettingHome::accelButtonPressed,
            m_controller.get(), &BasicSettingController::onAccelerationPressed);
    connect(m_ui->home_page, &BasicSettingHome::magButtonPressed,
            m_controller.get(), &BasicSettingController::onMagneticFieldPressed);
    connect(m_ui->home_page, &BasicSettingHome::perspectiveButtonPressed,
            m_controller.get(), &BasicSettingController::onPerspectivePressed);
    connect(m_ui->home_page, &BasicSettingHome::cameraParamBtnPressed,
            m_controller.get(), &BasicSettingController::onCameraParameterCalibrationPressed);
    connect(m_ui->home_page, &BasicSettingHome::cameraParamInfoBtnPressed,
            m_controller.get(), &BasicSettingController::onCameraParamInfoPressed);
    connect(m_ui->home_page, &BasicSettingHome::cameraParamRestoreBtnPressed,
            m_controller.get(), &BasicSettingController::onCameraParamRestorePressed);
    connect(m_ui->home_page, &BasicSettingHome::cameraPicBrowserBtnPressed,
            m_controller.get(), &BasicSettingController::onCameraPicBrowserPressed);
    connect(m_ui->home_page, &BasicSettingHome::rollbackButtonPressed,
            m_controller.get(), &BasicSettingController::onRollbackButtonPressed);
    connect(m_ui->home_page, &BasicSettingHome::aboutToShow,
            m_controller.get(), &BasicSettingController::onHomeViewShowed);
    connect(m_controller.get(), &BasicSettingController::displaySubview,
            this, &BasicSetting::pushWidgetToStack);
    connect(m_controller.get(), &BasicSettingController::dismissSubview,
            this, &BasicSetting::popWidgetFromStack);
}

BasicSetting::~BasicSetting()
{
    qCDebug(basic_setting()) << __func__;
}

void BasicSetting::pushWidgetToStack(QWidget *widget)
{
    qCInfo(basic_setting()) << __func__;
    assert(widget);
    m_ui->stackedWidget->insertWidget(0, widget);
    m_ui->stackedWidget->setCurrentIndex(0);
}

void BasicSetting::popWidgetFromStack(QWidget *widget)
{
    qCInfo(basic_setting()) << __func__;
    assert(widget);

    m_ui->stackedWidget->removeWidget(widget);
    m_ui->stackedWidget->setCurrentIndex(0);
    widget->deleteLater();
}
