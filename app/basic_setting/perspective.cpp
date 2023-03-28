#include "perspective.h"
#include "ui_perspective.h"
#include "perspective_controller.h"

#include <QLoggingCategory>
#include <QMouseEvent>

Q_LOGGING_CATEGORY(perspective, "perspective")

using namespace std;

Perspective::Perspective(QWidget *parent)
    : QWidget(parent),
      m_ui(new Ui::PerspectiveUi)
{
    qCDebug(perspective()) << __func__;
    m_ui->setupUi(this);
    m_ui->lb_live->installEventFilter(this);

    m_controller = make_unique<PerspectiveController>(this);

    connect(m_controller.get(), &PerspectiveController::updateLiveFrame,
            this, &Perspective::onUpdateLive);
    connect(m_controller.get(), &PerspectiveController::updateText,
            this, &Perspective::onUpdateText);
    connect(m_ui->pb_back, &QPushButton::released,
            m_controller.get(), &PerspectiveController::onBackButtonClicked);
    connect(m_ui->pb_guide, &QPushButton::released,
            m_controller.get(), &PerspectiveController::onGuideButtonClicked);
    connect(m_ui->pb_auto, &QPushButton::released,
            m_controller.get(), &PerspectiveController::onAutoClicked);
    connect(m_ui->pb_manual, &QPushButton::released,
            m_controller.get(), &PerspectiveController::onManualClicked);
    connect(this, &Perspective::selectPoint,
            m_controller.get(), &PerspectiveController::onPointSelected);
    connect(m_ui->pb_previous, &QPushButton::released,
            m_controller.get(), &PerspectiveController::onPreviousBtnClicked);
    connect(m_ui->pb_next, &QPushButton::released,
            m_controller.get(), &PerspectiveController::onNextBtnClicked);
    connect(m_ui->pb_done, &QPushButton::released,
            m_controller.get(), &PerspectiveController::onFinishBtnClicked);
    connect(m_controller.get(), &PerspectiveController::setFinishBtnEnable,
            this, &Perspective::onSetFinishBtnEnable);

    // 默认进入自动校准模式
    setManualMode(false);
}

Perspective::~Perspective()
{
    qCDebug(perspective()) << __func__;
}

void Perspective::onUpdateLive(QPixmap frame)
{
    m_ui->lb_live->setPixmap(frame);
}

void Perspective::onUpdateText(QString text)
{
    m_ui->lb_live->setText(text);
}

void Perspective::onSetFinishBtnEnable(bool bEnable)
{
    m_ui->pb_done->setEnabled(bEnable);
    m_ui->pb_previous->setEnabled(bEnable);
}

void Perspective::setManualMode(bool on)
{
    this->setProperty("onManualMode", on);
    m_ui->pb_auto->style()->polish(m_ui->pb_auto);
    m_ui->pb_manual->style()->polish(m_ui->pb_manual);

    if (on)
    {
        m_ui->label->setText(kTipsTextManual);
        m_ui->pb_previous->hide();
        m_ui->pb_next->show();
        m_ui->pb_next->setDisabled(true);
        m_ui->pb_done->hide();
    }
    else
    {
        m_ui->label->setText(kTipsTextAuto);
        m_ui->pb_previous->hide();
        m_ui->pb_next->hide();
        m_ui->pb_done->hide();
    }
}

bool Perspective::isManualMode()
{
    return this->property("onManualMode").toBool();
}

bool Perspective::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_ui->lb_live && event->type() == QEvent::MouseButtonPress)
    {
        if (isManualMode())
        {
            QMouseEvent* mouse_event = (QMouseEvent*)event;
            QPoint point = mouse_event->pos();
            emit selectPoint(point);
            m_ui->pb_next->setEnabled(true);
        }
    }

    return false;
}

void Perspective::setManualProgress(ManualSteps step, bool nextBtnEnabled)
{
    qCInfo(perspective()) << __func__ << "step -> " << static_cast<uint8_t>(step);
    switch (step) {
    case Perspective::ManualStep1:
        m_ui->pb_previous->hide();
        m_ui->pb_next->show();
        m_ui->pb_next->setEnabled(nextBtnEnabled);
        m_ui->pb_done->hide();
        break;
    case Perspective::ManualStep2:
    case Perspective::ManualStep3:
    case Perspective::ManualStep4:
        m_ui->pb_previous->show();
        m_ui->pb_next->show();
        m_ui->pb_next->setEnabled(nextBtnEnabled);
        m_ui->pb_done->hide();
        break;
    case Perspective::ManualStepDone:
        m_ui->pb_previous->show();
        m_ui->pb_next->hide();
        m_ui->pb_done->show();
        break;
    default:
        break;
    }
}

QSize Perspective::getLiveWidgetSize()
{
    return m_ui->lb_live->size();
}
