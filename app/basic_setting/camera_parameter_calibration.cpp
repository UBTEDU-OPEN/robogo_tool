#include "camera_parameter_calibration.h"
#include "ui_camera_parameter_calibration.h"
#include "camera_param_cali_controller.h"

#include <QLoggingCategory>
#include <QMouseEvent>

Q_LOGGING_CATEGORY(camera_parameter_cali, "camera_parameter_cali")

using namespace std;

CameraParameterCalibration::CameraParameterCalibration(QWidget *parent)
    : QWidget(parent),
      m_ui(new Ui::CameraParameterCalibrationUi)
{
    qCDebug(camera_parameter_cali()) << __func__;
    m_ui->setupUi(this);
    installEventFilter(this);

    m_controller = make_unique<CameraParamCaliController>(this);

    connect(m_controller.get(), &CameraParamCaliController::updateLiveFrame,
            this, &CameraParameterCalibration::onUpdateLive);
    connect(m_controller.get(), &CameraParamCaliController::updateText,
            this, &CameraParameterCalibration::onUpdateText);
    connect(m_ui->pb_back, &QPushButton::released,
            m_controller.get(), &CameraParamCaliController::onBackButtonClicked);
    connect(m_ui->lb_guide, &QLabel::linkActivated,
            m_controller.get(), &CameraParamCaliController::onGuideButtonClicked);
    connect(m_ui->pb_capture, &QPushButton::released,
            m_controller.get(), &CameraParamCaliController::onCaptureBtnPressed);
    connect(m_ui->pb_save, &QPushButton::released,
            m_controller.get(), &CameraParamCaliController::onSaveButtonClicked);
    connect(m_ui->pb_cancel, &QPushButton::released,
            m_controller.get(), &CameraParamCaliController::onCancelButtonClicked);

    initMask();
    showCalibration();
    setCaptureBtnEnable(false);
}

CameraParameterCalibration::~CameraParameterCalibration()
{
    qCDebug(camera_parameter_cali()) << __func__;
}

void CameraParameterCalibration::onUpdateLive(QPixmap frame)
{
    m_ui->lb_live->setPixmap(frame);
    m_ui->pb_capture->setFocus();
}

void CameraParameterCalibration::setCaptureBtnEnable(bool bEnable)
{
    qInfo(camera_parameter_cali()) << __func__ << bEnable;
    m_ui->pb_capture->setEnabled(bEnable);
}

bool CameraParameterCalibration::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent* key_event = (QKeyEvent*)event;
        if (key_event->key() == Qt::Key_Space)
        {
            if (m_ui->pb_capture->isEnabled() && m_ui->pb_capture->isVisible())
            {
                m_ui->pb_capture->released();
            }
        }
    }

    return false;
}

QSize CameraParameterCalibration::getLiveWidgetSize()
{
    return m_ui->lb_live->size();
}

void CameraParameterCalibration::onUpdateText(QString text)
{
    m_ui->lb_live->setText(text);
}

void CameraParameterCalibration::showResult()
{
    m_ui->stackedWidget->setCurrentIndex(1);
}

void CameraParameterCalibration::showCalibration()
{
    m_ui->stackedWidget->setCurrentIndex(0);
}

void CameraParameterCalibration::setResultText(const QString &text)
{
    m_ui->textBrowser->setText(text);
}

void CameraParameterCalibration::setSaveBtnEnable(bool bEnable)
{
    qInfo(camera_parameter_cali()) << __func__ << bEnable;
    m_ui->pb_save->setEnabled(bEnable);
}

void CameraParameterCalibration::setMaskEnable(bool enabled)
{
    qInfo(camera_parameter_cali()) << __func__ << enabled;

    m_mask->setVisible(enabled);
}

void CameraParameterCalibration::setLiveMaskEnable(bool enabled)
{
    qInfo(camera_parameter_cali()) << __func__ << enabled;

    m_live_mask->setVisible(enabled);
}

void CameraParameterCalibration::initMask()
{
    m_mask = createMaskForWidget(this, true);
    m_live_mask = createMaskForWidget(m_ui->lb_live, false);
}

QWidget* CameraParameterCalibration::createMaskForWidget(QWidget *parent_widget, bool text_required)
{
    QWidget* mask = new QWidget(parent_widget);
    mask->setStyleSheet("background-color: rgba(0, 0, 0, 128);");
    mask->setFixedSize(parent_widget->size());
    QPixmap pixmap(":/ui/images/ic_loading.png");
    QLabel *lb_pic = new QLabel(mask);
    lb_pic->setPixmap(pixmap);
    lb_pic->adjustSize();
    lb_pic->setStyleSheet("background-color: transparent;");
    auto offset = 0;
    if (text_required)
    {
        offset = 14;
    }
    lb_pic->move(parent_widget->width()/2 - lb_pic->width()/2, parent_widget->height()/2 - lb_pic->width()/2 - offset);

    if (text_required)
    {
        QLabel *lb_text = new QLabel(mask);
        lb_text->setText(tr("标定运算中"));
        lb_text->setStyleSheet("background-color: transparent; font-family: SourceHanSansCN-Normal; font-size: 14px; color:#FFFFFF; text-align: center");
        lb_text->adjustSize();
        lb_text->move(parent_widget->width()/2 - lb_pic->width()/2 - 10, lb_pic->geometry().y() + lb_pic->height() + 14);
    }

    mask->setVisible(false);
    parent_widget->stackUnder(mask);

    return mask;
}
