#include "acceleration_calibration.h"
#include "ui_accel.h"
#include "acceleration_calibration_controller.h"
#include <QLoggingCategory>
#include <QStyleOption>
#include <QPainter>
#include <assert.h>
#include <iostream>

Q_LOGGING_CATEGORY(accel_cali, "accel_cali")


AccelerationCalibration::AccelerationCalibration(QWidget *parent)
    : QWidget(parent),
      m_ui(new Ui::AccelUi)
{
    qCDebug(accel_cali()) << __func__;
    m_ui->setupUi(this);
    onUpdateProgress(0);
    setButtonsStatus(AccelerationCalibration::Status::Idle);

    m_controller = std::make_unique<AccelerationCalibrationController>(this);

    connect(m_ui->pb_back, &QPushButton::released,
            m_controller.get(), &AccelerationCalibrationController::onBackButtonPressed);
    connect(m_ui->pb_start, &QPushButton::released,
            m_controller.get(), &AccelerationCalibrationController::onStartButtonPressed);
    connect(m_ui->pb_save, &QPushButton::released,
            m_controller.get(), &AccelerationCalibrationController::onSaveButtonPressed);
    connect(m_controller.get(), &AccelerationCalibrationController::setAccelValues,
            this, &AccelerationCalibration::onUpdateValues);
    connect(m_controller.get(), &AccelerationCalibrationController::setProgress,
            this, &AccelerationCalibration::onUpdateProgress);
    // 最小化时，会触发hidingEvent, 会影响校准过程
//    connect(this, &AccelerationCalibration::aboutToHide,
//            m_controller.get(), &AccelerationCalibrationController::onViewHiding);
}

AccelerationCalibration::~AccelerationCalibration()
{
    qCDebug(accel_cali()) << __func__;
}

void AccelerationCalibration::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    QWidget::paintEvent(event);
}

void AccelerationCalibration::onUpdateValues(int32_t x, int32_t y, int32_t z)
{
    qCInfo(accel_cali()) << __func__
                         << "x: " << x
                         << "y: " << y
                         << "z: " << z;
    m_ui->lb_x->setText(QString::number(x));
    m_ui->lb_y->setText(QString::number(y));
    m_ui->lb_z->setText(QString::number(z));
}

void AccelerationCalibration::onUpdateProgress(uint8_t value)
{
    qCDebug(accel_cali()) << __func__ <<  " progress : " << value;
    m_ui->progressBar->setValue(value);

    auto x = m_ui->progressBar->pos().x() + m_ui->progressBar->width() * value / 100 - 15;
    auto y = m_ui->progressBar->pos().y() - 27;
    m_ui->frame_handler->move(x, y);
    m_ui->lb_progress->setText(QString::number(value));
    m_ui->frame_handler->update();
}

void AccelerationCalibration::setButtonsStatus(AccelerationCalibration::Status status)
{
    qCDebug(accel_cali()) << __func__ << "status -> " << static_cast<int>(status);
    switch (status)
    {
    case Status::Idle:
        m_ui->pb_start->setEnabled(true);
        m_ui->pb_save->setDisabled(true);
        onUpdateProgress(0);
        m_ui->warning_info_frame->show();
        m_ui->finish_info_frame->hide();
        break;
    case Status::Calibrating:
        m_ui->pb_start->setDisabled(true);
        m_ui->pb_save->setDisabled(true);
        m_ui->warning_info_frame->show();
        m_ui->finish_info_frame->hide();
        break;
    case Status::Finished:
        m_ui->pb_start->setEnabled(false);
        m_ui->pb_save->setEnabled(true);
        m_ui->warning_info_frame->hide();
        m_ui->finish_info_frame->show();
        break;
    default:
        break;
    }
}

void AccelerationCalibration::hideEvent(QHideEvent *event)
{
    qCInfo(accel_cali()) << __func__;
    emit aboutToHide();
    QWidget::hideEvent(event);
}
