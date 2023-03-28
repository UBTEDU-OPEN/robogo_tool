#include "guide_rollback.h"
#include "ui_guide_rollback.h"
#include <QStyleOption>
#include <QPainter>
#include <QBitmap>

RollbackGuidelines::RollbackGuidelines(QWidget *parent)
    : QDialog(parent),
      m_ui(new Ui::RollbackGuideDialog)
{
    m_ui->setupUi(this);
    Qt::WindowFlags flags = Qt::FramelessWindowHint | Qt::Dialog;
    this->setWindowFlags(flags);

    connect(m_ui->pb_close, &QPushButton::released,
            this, [this](){
        this->close();
     }
    );

    connect(m_ui->pb_ok, &QPushButton::released,
            this, [this](){
        this->close();
     }
    );

    if(parent)
    {
        QPoint global_pos = parent->mapToGlobal(QPoint(0, 0));
        move(global_pos.x() + 300, global_pos.y() + 138);
    }
}

RollbackGuidelines::~RollbackGuidelines()
{
}

void RollbackGuidelines::paintEvent(QPaintEvent *)
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
}
