#include "toast.h"
#include "ui_toast.h"
#include <QStyleOption>
#include <QPainter>
#include <QPropertyAnimation>
#include <QTimer>
#include <QMouseEvent>
#include <QLoggingCategory>
#include <QGraphicsOpacityEffect>

Q_LOGGING_CATEGORY(toast, "Toast")

using namespace std;

Toast::Toast(QString message, QWidget* parent)
    : QWidget(parent),
    m_ui(new Ui::Toast)
{
    qCDebug(toast()) << __func__;
    m_ui->setupUi(this);

    this->setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
//    this->setAttribute(Qt::WA_DeleteOnClose, true);

    m_ui->text_label->setText(message);

    auto op = new QGraphicsOpacityEffect(this);
    op->setOpacity(0.8);
    setGraphicsEffect(op);
    setAutoFillBackground(true);

    if(parent)
    {
        move(parent->width()/2 - this->width()/2, parent->height()/2 - this->height()/2);
    }
}

Toast::~Toast()
{
    qCDebug(toast()) << __func__;
}


void Toast::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void Toast::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)

    connect(&m_timer, &QTimer::timeout, this, &Toast::close);
    m_timer.start(3000);
}
