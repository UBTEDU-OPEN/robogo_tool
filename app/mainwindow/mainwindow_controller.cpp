#include "mainwindow_controller.h"
#include "mainwindow.h"
#include "ucar_device.h"
#include "close_dialog.h"
#include "ota_close_guide.h"
#include <QLoggingCategory>
#include <assert.h>

Q_LOGGING_CATEGORY(mainwindow_controller, "mainwindow_controller")

MainWindowController::MainWindowController(MainWindow *view, QObject *parent)
    : QObject(parent),
      m_view(view),
      m_on_calibration(false),
      m_close_dialog_msg("")
{
    assert(m_view);
}

void MainWindowController::onTabIndexChanged(uint8_t index)
{
    qCInfo(mainwindow_controller()) << "index: " << index;
    if (index == 2)
    {
        UCarDevice::instance().enableDistributing(false);
    }
}

void MainWindowController::onCloseClicked()
{
    qCInfo(mainwindow_controller()) << __func__ << "Calibration is on -> " << m_on_calibration;
    bool should_close = true;
    if (m_on_calibration)
    {
        CloseDialog dialog(m_view);
        if (m_close_dialog_msg.length() > 0)
        {
            dialog.setMessage(m_close_dialog_msg);
        }

        auto ret = dialog.exec();
        if (ret == QDialog::Accepted)
        {
            qCInfo(mainwindow_controller()) << __func__ << "Close confirmed";
        }
        else
        {
            should_close = false;
        }
    }

    if (m_view->isUpdating())
    {
        OtaCloseGuide otaGuide(m_view);
        if(otaGuide.exec() == QDialog::Rejected)
        {
            should_close = false;
        }
    }

    if (should_close)
    {
        UCarDevice::instance().enableDistributing(false);
        UCarDevice::instance().exitAdjusting();
        UCarDevice::instance().unload();
        m_view->confirmCloseEvent();
        m_view->close();
    }
}

void MainWindowController::onMinimizeClicked()
{
    qCInfo(mainwindow_controller()) << __func__;
    m_view->setWindowState(Qt::WindowMinimized);
}

void MainWindowController::onEnterCalibration(QString toast_message, QString dialog_message)
{
    qCInfo(mainwindow_controller()) << __func__;
    m_on_calibration = true;
    m_close_dialog_msg = dialog_message;
    m_view->setTabForbiddenMsg(toast_message);
    m_view->setCalibrationState(true);
}

void MainWindowController::onExitCalibration()
{
    qCInfo(mainwindow_controller()) << __func__;
    m_on_calibration = false;
    m_view->setCalibrationState(false);
}
