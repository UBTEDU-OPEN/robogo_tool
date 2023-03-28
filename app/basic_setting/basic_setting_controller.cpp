#include "basic_setting_controller.h"
#include "basic_setting.h"
#include "acceleration_calibration.h"
#include "magnetic_calibration.h"
#include "perspective.h"
#include "rollback.h"
#include "ucar_device.h"
#include "camera_parameter_calibration.h"
#include "camera_parameter_info.h"
#include "setting.h"
#include "toast_manager.h"
#include "guide_disconnect.h"
#include <QLoggingCategory>
#include <QDesktopServices>
#include <QUrl>
#include <QDir>
#include <assert.h>

Q_LOGGING_CATEGORY(basic_setting_controller, "basic_setting_controller")

BasicSettingController::BasicSettingController(BasicSetting *view, QObject *parent)
    : QObject(parent),
      m_view(view)


{
    qCDebug(basic_setting_controller()) << __func__;
    assert(m_view);
}

void BasicSettingController::onAccelerationPressed()
{
    qCInfo(basic_setting_controller()) << __func__;
    AccelerationCalibration* accel = new AccelerationCalibration(m_view);

    connect(accel, &AccelerationCalibration::enterCalibration, m_view, &BasicSetting::enterCalibration);
    connect(accel, &AccelerationCalibration::exitCalibration, m_view, &BasicSetting::exitCalibration);

    connect(accel, &AccelerationCalibration::backToPreviousView,
            this, [=](){
        emit this->dismissSubview(accel);
    });

    emit displaySubview(accel);
}

void BasicSettingController::onMagneticFieldPressed()
{
    qCInfo(basic_setting_controller()) << __func__;
    MagneticCalibration* magnetic = new MagneticCalibration(m_view);

    connect(magnetic, &MagneticCalibration::enterCalibration, m_view, &BasicSetting::enterCalibration);
    connect(magnetic, &MagneticCalibration::exitCalibration, m_view, &BasicSetting::exitCalibration);

    connect(magnetic, &MagneticCalibration::backToPreviousView,
            this, [=](){
        emit this->dismissSubview(magnetic);
    });

    emit displaySubview(magnetic);
}

void BasicSettingController::onPerspectivePressed()
{
    qCInfo(basic_setting_controller()) << __func__;
    Perspective* perspective= new Perspective(m_view);

//    connect(perspective, &Perspective::enterCalibration, m_view, &BasicSetting::enterCalibration);
    connect(perspective, &Perspective::exitCalibration, m_view, &BasicSetting::exitCalibration);
    connect(perspective, &Perspective::backToPreviousView,
            this, [=](){
        emit this->dismissSubview(perspective);
    });

    m_view->enterCalibration(PERSPECTIVE_TAB_FROBIDDEN_MESSAGE, PERSPECTIVE_CLOSE_CONFIRM_MESSAGE);

    emit displaySubview(perspective);
}

void BasicSettingController::onRollbackButtonPressed()
{
    qCInfo(basic_setting_controller()) << __func__;
    rollback* rollback_view= new rollback(m_view);

//    connect(perspective, &Perspective::enterCalibration, m_view, &BasicSetting::enterCalibration);
//    connect(perspective, &rollback::exitCalibration, m_view, &BasicSetting::exitCalibration);
    connect(rollback_view, &rollback::backToPreviousView,
            this, [=](){
        emit this->dismissSubview(rollback_view);
    });

//    m_view->enterCalibration(PERSPECTIVE_TAB_FROBIDDEN_MESSAGE, PERSPECTIVE_CLOSE_CONFIRM_MESSAGE);

    emit displaySubview(rollback_view);
}

void BasicSettingController::onHomeViewShowed()
{
    qCInfo(basic_setting_controller()) << __func__;
    UCarDevice::instance().enableDistributing(false);
}

void BasicSettingController::onCameraParameterCalibrationPressed()
{
    qCInfo(basic_setting_controller()) << __func__;
    CameraParameterCalibration* perspective= new CameraParameterCalibration(m_view);

    connect(perspective, &CameraParameterCalibration::exitCalibration, m_view, &BasicSetting::exitCalibration);
    connect(perspective, &CameraParameterCalibration::backToPreviousView,
            this, [=](){
        emit this->dismissSubview(perspective);
    });

    m_view->enterCalibration(PERSPECTIVE_TAB_FROBIDDEN_MESSAGE, PERSPECTIVE_CLOSE_CONFIRM_MESSAGE);

    emit displaySubview(perspective);
}

void BasicSettingController::onCameraParamInfoPressed()
{
    qCInfo(basic_setting_controller()) << __func__;
    CameraParameterInfo info_dialog(m_view);
    info_dialog.exec();
}

void BasicSettingController::onCameraParamRestorePressed()
{
    qCInfo(basic_setting_controller()) << __func__;

    bool ret = Setting::instance().restorMatrixAndCoef();
    if (ret)
    {
        ToastManager::instance().show(TOAST_RESTORE_CAM_PARAM_SUCCESS);
    }
    else
    {
        ToastManager::instance().show(TOAST_RESTORE_CAM_PARAM_ERROR);
    }
}

void BasicSettingController::onCameraPicBrowserPressed()
{
    qCInfo(basic_setting_controller()) << __func__;
    QString pic_dir = Setting::instance().getCameraPicFolderPath();
    QDir dir;
    if (!dir.exists(pic_dir))
    {
        dir.mkpath(pic_dir);
    }
    QDesktopServices::openUrl(QUrl(pic_dir));
}
