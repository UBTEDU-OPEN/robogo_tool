#include "acceleration_calibration_controller.h"
#include "acceleration_calibration.h"
#include "ucar_device.h"
#include "toast_manager.h"
#include <QLoggingCategory>
#include <assert.h>

Q_LOGGING_CATEGORY(accel_controller, "accel_controller")


AccelerationCalibrationController::AccelerationCalibrationController(AccelerationCalibration* view, QObject *parent)
    : QObject(parent),
      m_view(view),
      m_progress(0)
{
    qCInfo(accel_controller()) << __func__;
    assert(m_view);

    UCarDevice::instance().enableDistributing(false);

    connect(&m_progress_timer, &QTimer::timeout, this, &AccelerationCalibrationController::onUpdateProgress);
    m_progress_timer.setInterval(1500);
    connect(&m_offline_check_timer, &QTimer::timeout, this, &AccelerationCalibrationController::onLostConnection);

    QTimer::singleShot(0, this, &AccelerationCalibrationController::onGetAndUpdateOffsets);
}

AccelerationCalibrationController::~AccelerationCalibrationController()
{
    qCInfo(accel_controller()) << __func__;
}

void AccelerationCalibrationController::onBackButtonPressed()
{
    qCInfo(accel_controller()) << __func__;
    emit m_view->backToPreviousView();
    m_view->setButtonsStatus(AccelerationCalibration::Status::Idle);
    onViewHiding();
}

void AccelerationCalibrationController::onStartButtonPressed()
{
    qCInfo(accel_controller()) << __func__;
    m_view->setButtonsStatus(AccelerationCalibration::Status::Calibrating);
    bool ret =  UCarDevice::instance().startAccelAdjusting();
    if(!ret)
    {
        ToastManager::instance().show(tr("校准出错"));
        m_view->setButtonsStatus(AccelerationCalibration::Status::Idle);
    }
    else
    {
        // 目前无法在上位机计算补偿值，不需要获取上报数据
//        ImuDevice::instance().enableDistributing(true);
        emit m_view->enterCalibration(TAB_FORBIDDEN_MESSAGE, CLOSE_CONFIRM_MESSAGE);
        m_progress = 0;
        emit setProgress(m_progress);
        m_progress_timer.start();

        // 校准过程中检查设备连接
        connect(&UCarDevice::instance(), &UCarDevice::accelData, this, &AccelerationCalibrationController::onReadAccelData);
        UCarDevice::instance().enableDistributing(true);
    }
}

void AccelerationCalibrationController::onSaveButtonPressed()
{
    qCInfo(accel_controller()) << __func__;

    bool ret =  UCarDevice::instance().exitAdjusting();
    QString text;
    if(!ret)
    {
        text = tr("保存失败，请重新操作!");
    }
    else
    {
        text = tr("保存成功!");
        // 读取新的补偿值，更新到界面
        QTimer::singleShot(100, this, &AccelerationCalibrationController::onGetAndUpdateOffsets);
    }
    ToastManager::instance().show(text);

   m_view->setButtonsStatus(AccelerationCalibration::Status::Idle);
   UCarDevice::instance().enableDistributing(false);
   emit m_view->exitCalibration();
}

void AccelerationCalibrationController::onReadAccelData(float x, float y, float z)
{
    qCDebug(accel_controller()) << __func__;
    int32_t ix = x * 16*9.8;
    int32_t iy = y * 16*9.8;
    int32_t iz = z * 16*9.8;

    m_offline_check_timer.start(1000);

//    emit setAccelValues(ix, iy, iz);
}

void AccelerationCalibrationController::onViewHiding(void)
{
    qCInfo(accel_controller()) << __func__;
    UCarDevice::instance().enableDistributing(false);
    emit m_view->exitCalibration();

    if (m_progress_timer.isActive())
    {
        m_progress_timer.stop();
    }
    m_progress = 0;
    emit setProgress(m_progress);
}

void AccelerationCalibrationController::onUpdateProgress()
{
    qCInfo(accel_controller()) << __func__;
    m_progress += 50;
    emit setProgress(m_progress);

    if (m_progress >= 100)
    {
        disconnect(&UCarDevice::instance(), &UCarDevice::accelData, this, 0);
        UCarDevice::instance().enableDistributing(false);
        m_progress_timer.stop();
        m_offline_check_timer.stop();
        m_view->setButtonsStatus(AccelerationCalibration::Status::Finished);
        ToastManager::instance().show(tr("校准完成，请及时保存！"));

    }
}

void AccelerationCalibrationController::onGetAndUpdateOffsets()
{
    int32_t x = 0, y = 0, z = 0;
    bool ret = UCarDevice::instance().getAccelOffsets(x, y, z);
    if (ret)
    {
        emit this->setAccelValues(x, y, z);
        qCInfo(accel_controller()) << "Accel offset: " << x << " " << y << " " << z;
    }
    else
    {
        qCInfo(accel_controller()) << "Failed to get Accel offset";
    }
}

void AccelerationCalibrationController::onLostConnection()
{
    qCDebug(accel_controller()) << __func__;
    m_offline_check_timer.stop();
    // 停掉校准进度timer
    if (m_progress_timer.isActive())
    {
        m_progress_timer.stop();
    }

    // 重置UI状态
    m_view->setButtonsStatus(AccelerationCalibration::Status::Idle);
    emit setProgress(0);
    emit m_view->exitCalibration();

    ToastManager::instance().show(tr("设备连接异常，请重新校准！"));
}
