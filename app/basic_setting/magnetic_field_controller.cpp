#include "magnetic_field_controller.h"
#include "magnetic_calibration.h"
#include "operation_guide.h"
#include "guide_step.h"
#include "ucar_device.h"
#include "toast_manager.h"
#include "close_dialog.h"
#include <QLoggingCategory>
#include <QTimer>
#include <QProcess>
#include <assert.h>

Q_LOGGING_CATEGORY(magnetic_controller, "magnetic_controller")


MagneticFieldController::MagneticFieldController(MagneticCalibration *view, QObject *parent)
    : QObject(parent),
      m_view(view),
      m_offset_x(0),
      m_offset_y(0),
      m_offset_z(0),
      m_opeartion_guide_process(nullptr)
{
    qCDebug(magnetic_controller()) << __func__;
    assert(m_view);

    QTimer::singleShot(0, this, &MagneticFieldController::onGetAndUpdateOffsets);
    connect(&m_offline_check_timer, &QTimer::timeout, this, &MagneticFieldController::onLostConnection);
    connect(&UCarDevice::instance(), &UCarDevice::magneticData, this, &MagneticFieldController::onReadMagneticData, Qt::QueuedConnection);
    UCarDevice::instance().enableDistributing(true);
}

MagneticFieldController::~MagneticFieldController()
{
    qCDebug(magnetic_controller()) << __func__;
    if(m_opeartion_guide_process)
    {
        disconnect(m_opeartion_guide_process, 0, this, 0);
        m_opeartion_guide_process->close();
        m_opeartion_guide_process = nullptr;
    }
}

void MagneticFieldController::onBackButtonClicked()
{
    qCInfo(magnetic_controller()) << __func__;

    if (m_view->isOnCalibration())
    {
        CloseDialog dialog(m_view);
        dialog.setMessage(BACK_DIALOG_MESSAGE);
        dialog.setOkButtonText(BTN_TEXT_QUIT);
        dialog.setCancelButtonText(BTN_TEXT_CONTINUE);
        dialog.setTitileText(TITLE_TEXT_EXIT_CAL);
        auto ret = dialog.exec();
        //TODO by Joker 交互要求点击关闭按钮是继续校准的操作
        if (ret == QDialog::Rejected)
        {
            qCInfo(magnetic_controller()) << __func__ << "Back cancelled";
            return ;
        }
        qCInfo(magnetic_controller()) << __func__ << "Back confirmed";
    }

    emit m_view->backToPreviousView();
    m_view->setButtonsStatus(MagneticCalibration::Status::Idle);
    UCarDevice::instance().enableDistributing(false);
    UCarDevice::instance().exitAdjusting();
    emit m_view->exitCalibration();
}

void MagneticFieldController::onGuideButtonClicked()
{
    qCInfo(magnetic_controller()) << __func__;
    QPoint global_pos = m_view->mapToGlobal(m_view->pos());
    QStringList args;
    args << QString::number(global_pos.x()) << QString::number(global_pos.y()) << QString::number(m_view->width()) << QString::number(m_view->height());
    if (!m_opeartion_guide_process)
    {
        m_opeartion_guide_process = new QProcess(this);
        connect(m_opeartion_guide_process, SIGNAL(finished(int)), this, SLOT(onGuideClosed(int)));
        m_opeartion_guide_process->start("/usr/bin/OperationGuide", args);
        m_opeartion_guide_process->waitForStarted();
    }
}

void MagneticFieldController::onStartButtonClicked()
{
    qCInfo(magnetic_controller()) << __func__;
    m_view->setButtonsStatus(MagneticCalibration::Status::Calibrating);
    m_offset_x = m_offset_y = m_offset_z = 0;

    bool ret =  UCarDevice::instance().startMagneticAdjusting();
    if(!ret)
    {
        ToastManager::instance().show(tr("校准出错"));
        m_view->setButtonsStatus(MagneticCalibration::Status::Idle);
    }
    else
    {
        emit m_view->enterCalibration(TAB_FORBIDDEN_MESSAGE, CLOSE_CONFIRM_MESSAGE);
        m_offline_check_timer.start(1000);
    }
}

void MagneticFieldController::onFinishButtonClicked()
{
    qCInfo(magnetic_controller()) << __func__;

    m_offline_check_timer.stop();

    bool ret =  UCarDevice::instance().exitAdjusting();
    QString text;
    if(!ret)
    {
        text = tr("保存失败，请重新操作!");
    }
    else
    {
        text = tr("保存成功!");
        m_view->setButtonsStatus(MagneticCalibration::Status::Idle);
        emit m_view->exitCalibration();
        // 读取新的补偿值，更新到界面
        QTimer::singleShot(100, this, &MagneticFieldController::onGetAndUpdateOffsets);
    }
    ToastManager::instance().show(text);
}

void MagneticFieldController::onReadMagneticData(int32_t x, int32_t y, int32_t z)
{
    qCDebug(magnetic_controller()) << __func__;
    if (m_offline_check_timer.isActive())
    {
        m_offline_check_timer.start(1000);
    }
    emit setMagneticFieldValues(x - m_offset_x, y - m_offset_y, z - m_offset_z);
}

void MagneticFieldController::onGetAndUpdateOffsets()
{
    int32_t x = 0, y = 0, z = 0;
    bool ret = UCarDevice::instance().getMagneticOffsets(x, y, z);
    if (ret)
    {
        emit this->setMagneticFieldOffsets(x, y, z);
        m_offset_x = x;
        m_offset_y = y;
        m_offset_z = z;
        qCInfo(magnetic_controller()) << "MagneticField offset: " << x << " " << y << " " << z;
    }
    else
    {
        qCInfo(magnetic_controller()) << "Failed to get MagneticField offset";
    }
}

void MagneticFieldController::onViewHideEvent(void)
{
    qCInfo(magnetic_controller()) << __func__;
    UCarDevice::instance().enableDistributing(false);
    disconnect(&UCarDevice::instance(), 0, this, 0);
    emit m_view->exitCalibration();
}

void MagneticFieldController::onViewShowEvent(void)
{
    qCInfo(magnetic_controller()) << __func__;
}

void MagneticFieldController::onLostConnection()
{
    qCInfo(magnetic_controller()) << __func__;

    m_offline_check_timer.stop();

    // 重置UI状态
    m_view->setButtonsStatus(MagneticCalibration::Status::Idle);
    emit m_view->exitCalibration();

    ToastManager::instance().show(tr("设备连接异常，请重新校准！"));
}

void MagneticFieldController::onGuideClosed(int code)
{
    qCInfo(magnetic_controller()) << __func__ << "code : " << code;

    m_opeartion_guide_process = nullptr;
}
