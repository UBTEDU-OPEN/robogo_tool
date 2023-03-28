#include "operation_guide.h"
#include "ui_operation_guide.h"
#include "guide_step.h"
#include <QStyleOption>
#include <QPainter>
#include <QBitmap>


OperationGuide::OperationGuide(QWidget *parent)
    : QDialog(parent),
      m_ui(new Ui::OperationGuideUi)
{
    m_ui->setupUi(this);
    m_ui->indicator->installEventFilter(this);
    setModal(true);
    Qt::WindowFlags flags = Qt::FramelessWindowHint | Qt::Dialog;
    this->setWindowFlags(flags);
    this->setAttribute(Qt::WA_TranslucentBackground, false);
    this->setAttribute(Qt::WA_NoSystemBackground);
    this->setWindowIcon(QIcon(":/ui/images/robogo_icon.png"));

    connect(m_ui->pb_close, &QPushButton::released,
            this, [this](){
        this->close();
        emit this->dialogClosed();
     }
    );

    connect(m_ui->pb_ok, &QPushButton::released,
            this, [this](){
        this->close();
        emit this->dialogClosed();
     }
    );
    if(parent)
    {
        QPoint global_pos = parent->mapToGlobal(QPoint(0, 0));
        auto parent_size = parent->size();
        auto my_size = this->size();
        move(global_pos.x() + (parent->width()-my_size.width())/2, global_pos.y() + (parent_size.height()-my_size.height())/2);
    }
}

OperationGuide::~OperationGuide()
{

}

void OperationGuide::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    QBitmap bmp(this->size());
    bmp.fill();
    QPainter painter(&bmp);
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::black);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawRoundedRect(bmp.rect(), 13, 13);
    setMask(bmp);

    QWidget::paintEvent(event);
}

void OperationGuide::initGuideSteps(std::vector<GuideStep *> steps)
{
    auto steps_count = steps.size();
    for(decltype(steps_count) i = 0; i < steps_count; ++i)
    {
        connect(steps[i], &GuideStep::goToNextStep, this, &OperationGuide::onStepChanged);
        if(steps[i]->guideType() == GuideStep::Video)
        {
            connect(this, &OperationGuide::dialogClosed, steps[i], &GuideStep::onParentClosed);
        }
        m_ui->stackedWidget->addWidget(steps[i]);
    }
}

void OperationGuide::onStepChanged(int8_t direction)
{
    auto current_index = m_ui->stackedWidget->currentIndex();
    if (direction < 0 && current_index >= 1)
    {
        m_ui->stackedWidget->setCurrentIndex(current_index - 1);
    }
    else if (direction > 0 && current_index <= 3)
    {
        m_ui->stackedWidget->setCurrentIndex(current_index + 1);
    }

    m_ui->indicator->update();
}

bool OperationGuide::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_ui->indicator && event->type() == QEvent::Paint)
    {
        if (m_ui->stackedWidget->count() <= 1)
        {
            // 不需要画页面指示器
            return false;
        }

        QPainter painter;
        painter.begin(m_ui->indicator);
        painter.setRenderHint(QPainter::Antialiasing, true);

        auto w = m_ui->indicator->size().width();
        auto start = w / 2 - 45* m_ui->stackedWidget->count()/2.0;
        for (auto i = 0; i < m_ui->stackedWidget->count(); ++i)
        {
            if (i == m_ui->stackedWidget->currentIndex())
            {
                painter.setBrush(QBrush(QColor(0x0, 0x7A, 0xFF)));
                painter.setPen(QColor(0x0, 0x7A, 0xFF));
            }
            else
            {
                painter.setBrush(QBrush(QColor(0xF0, 0xF0, 0xF0)));
                painter.setPen(QColor(0xF0, 0xF0, 0xF0));
            }
            painter.drawRect(start + 50*i, 0, 40, 4);
        }
        painter.end();
        return true;
    }

    return false;

}

void OperationGuide::setTitle(const QString &title)
{
    m_ui->lb_title->setText(title);
}
