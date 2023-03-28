#include "homepage.h"
#include "ui_homepage.h"
#include "homepage_controller.h"
#include <QStyleOption>
#include <QPainter>
#include <QLoggingCategory>
#include <cmath>

Q_LOGGING_CATEGORY(homepage, "homepage")

Homepage::Homepage(QWidget *parent)
    : QWidget(parent),
      m_ui(new Ui::HomepageUi)
{
    m_ui->setupUi(this);

    m_controller = std::make_unique<HomepageController>(this);
//    m_otawidget = std::make_unique<otawidget>(this);;

    connect(m_controller.get(), &HomepageController::updateAngleValues,
            this, &Homepage::onUpdateAngleValues);
    connect(m_controller.get(), &HomepageController::updateConnectionStatus,
            this, &Homepage::onConnectionStatusChanged);
    connect(m_ui->pb_guide, &QPushButton::released,
            m_controller.get(), &HomepageController::onGuidelinesClicked);
    connect(this, &Homepage::aboutToHide,
            m_controller.get(), &HomepageController::onViewHideEvent);
    connect(this, &Homepage::aboutToShow,
            m_controller.get(), &HomepageController::onViewShowEvent);
    connect(m_controller.get(), &HomepageController::setWindowModal,
            this, &Homepage::updateModalState);
}

Homepage::~Homepage()
{
}

void Homepage::onUpdateAngleValues(float x, float y, float z)
{
    QString text;
    text.sprintf("%.02f°", x);
    m_ui->lb_angle_x_value->setText(text);
    text.sprintf("%.02f°", y);
    m_ui->lb_angle_y_value->setText(text);
    text.sprintf("%.02f°", z);
    m_ui->lb_angle_z_value->setText(text);

    m_ui->gradienter->setDegValue(x);
    m_ui->gradienter->setRollValue(y);

    static QPixmap needle_raw(":/ui/images/compass_needle.png");
    static QPixmap needle = needle_raw.scaled(220, 220, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QTransform transform;
    QTransform trans_rotate = transform.rotate(floor(-z));
    QPixmap rotated = needle.transformed(trans_rotate);
    m_ui->lb_compass_needle->setPixmap(rotated);
}

void Homepage::onConnectionStatusChanged(bool connected)
{
    m_ui->lb_status->setText(connected ? tr("在线") : tr("离线"));
    m_ui->lb_status_icon->setProperty("on_line", connected);
    m_ui->lb_status_icon->style()->unpolish(m_ui->lb_status_icon);
    m_ui->lb_status_icon->style()->polish(m_ui->lb_status_icon);

    if (connected) //只有在线的时候才会提示
    {
        emit sigCheckOta(true);
    }
}

void Homepage::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    QWidget::paintEvent(event);
}

void Homepage::hideEvent(QHideEvent *event)
{
    qCInfo(homepage()) << __func__;
    emit aboutToHide();
    QWidget::hideEvent(event);
}

void Homepage::showEvent(QShowEvent *event)
{
    qCInfo(homepage()) << __func__;
    emit aboutToShow();
    QWidget::showEvent(event);
}
