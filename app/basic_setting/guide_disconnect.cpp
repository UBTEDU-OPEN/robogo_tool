#include "guide_disconnect.h"
#include "ui_guide_disconnect.h"
#include <QStyleOption>
#include <QPainter>
#include <QBitmap>

GuideDisconnect::GuideDisconnect(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GuideDisconnect)
{
    ui->setupUi(this);
    Qt::WindowFlags flags = Qt::FramelessWindowHint | Qt::Dialog;
    this->setWindowFlags(flags);

    connect(ui->pb_close_2, &QPushButton::released,
            this, [this](){
        this->close();
     }
    );

    connect(ui->pb_ok, &QPushButton::released,
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

GuideDisconnect::~GuideDisconnect()
{
    delete ui;
}

void GuideDisconnect::paintEvent(QPaintEvent *)
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
