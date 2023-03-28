#include "magnetic_calibration.h"
#include "ui_magnetic.h"
#include "magnetic_field_controller.h"
#include <QLoggingCategory>
#include <assert.h>

Q_LOGGING_CATEGORY(magnetic_view, "magnetic_view")

MagneticCalibration::MagneticCalibration(QWidget *parent)
    : QWidget(parent),
      m_ui(new Ui::MagneticUi),
      m_on_cali(false)
{
    qCDebug(magnetic_view()) << __func__;
    m_ui->setupUi(this);
    m_ui->frame_xy->setTitle("ChartXY", Ellipse::ZAxis);
    m_ui->frame_yz->setTitle("ChartYZ", Ellipse::XAxis);
    m_ui->frame_xz->setTitle("ChartXZ", Ellipse::YAxis);

    m_controller = std::make_unique<MagneticFieldController>(this);

    // button actions
    connect(m_ui->pb_back, &QPushButton::released,
            m_controller.get(), &MagneticFieldController::onBackButtonClicked);
    connect(m_ui->pb_guide, &QPushButton::released,
            m_controller.get(), &MagneticFieldController::onGuideButtonClicked);
    connect(m_ui->pb_start, &QPushButton::released,
            m_controller.get(), &MagneticFieldController::onStartButtonClicked);
    connect(m_ui->pb_finish, &QPushButton::released,
            m_controller.get(), &MagneticFieldController::onFinishButtonClicked);

//    connect(this, &MagneticCalibration::aboutToHide,
//            m_controller.get(), &MagneticFieldController::onViewHideEvent);
//    connect(this, &MagneticCalibration::aboutToShow,
//            m_controller.get(), &MagneticFieldController::onViewShowEvent);
    // updating values from controller
    connect(m_controller.get(), &MagneticFieldController::setMagneticFieldValues,
            this, &MagneticCalibration::onUpdatingMagneticFieldValues);
    connect(m_controller.get(), &MagneticFieldController::setMagneticFieldOffsets,
            this, &MagneticCalibration::onUpdatingMagneticFieldOffsets);

}

MagneticCalibration::~MagneticCalibration()
{
    qCDebug(magnetic_view()) << __func__;
}

void MagneticCalibration::onUpdatingMagneticFieldValues(int32_t x, int32_t y, int32_t z)
{
    qCDebug(magnetic_view()) << __func__;
    m_ui->current_x->setText(QString::number(x));
    m_ui->current_y->setText(QString::number(y));
    m_ui->current_z->setText(QString::number(z));

    if(m_on_cali)
    {
        m_ui->frame_xy->updatePoints(x, y, z, Ellipse::ZAxis);
        m_ui->frame_yz->updatePoints(x, y, z, Ellipse::XAxis);
        m_ui->frame_xz->updatePoints(x, y, z, Ellipse::YAxis);
    }
}

void MagneticCalibration::onUpdatingMagneticFieldOffsets(int32_t x, int32_t y, int32_t z)
{
    qCDebug(magnetic_view()) << __func__;
    m_ui->offset_x->setText(QString::number(x));
    m_ui->offset_y->setText(QString::number(y));
    m_ui->offset_z->setText(QString::number(z));
}

void MagneticCalibration::setButtonsStatus(MagneticCalibration::Status status)
{
    qCDebug(magnetic_view()) << __func__ << "status -> " << static_cast<int>(status);
    switch (status)
    {
    case Status::Idle:
        m_ui->pb_start->setEnabled(true);
        m_ui->pb_finish->setDisabled(true);
        m_ui->frame_xy->clear();
        m_ui->frame_yz->clear();
        m_ui->frame_xz->clear();
        m_on_cali = false;
        break;
    case Status::Calibrating:
        m_ui->pb_start->setDisabled(true);
        m_ui->pb_finish->setEnabled(true);
        m_on_cali = true;
        break;
    default:
        break;
    }
}

void MagneticCalibration::hideEvent(QHideEvent *event)
{
    qCInfo(magnetic_view()) << __func__;
    emit aboutToHide();
    QWidget::hideEvent(event);
}

void MagneticCalibration::showEvent(QShowEvent *event)
{
    qCInfo(magnetic_view()) << __func__;
    emit aboutToShow();
    QWidget::showEvent(event);
}

bool MagneticCalibration::isOnCalibration() const
{
    return m_on_cali;
}
