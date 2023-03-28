#include "ota_close_guide.h"
#include "ui_ota_close_guide.h"
#include <QStyleOption>
#include <QPainter>
#include <QBitmap>

OtaCloseGuide::OtaCloseGuide(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OtaCloseGuide)
{
    ui->setupUi(this);
    Qt::WindowFlags flags = Qt::FramelessWindowHint | Qt::Dialog;
    this->setWindowFlags(flags);
    if(parent)
    {
        QPoint global_pos = parent->mapToGlobal(QPoint(0, 0));
        auto parent_size = parent->size();
        auto my_size = this->size();
        move(global_pos.x() + (parent->width()-my_size.width())/2, global_pos.y() + (parent_size.height()-my_size.height())/2);
    }
//    if(parent)
//    {
//        move(parent->width()/2 - this->width()/2, parent->height()/2 - this->height()/2);
//    }

}

OtaCloseGuide::~OtaCloseGuide()
{
    delete ui;
}



void OtaCloseGuide::on_pBOk_clicked()
{
    accept();
}

void OtaCloseGuide::on_pBCancel_clicked()
{
    reject();
}

void OtaCloseGuide::on_pb_close_clicked()
{
    reject();
}

void OtaCloseGuide::paintEvent(QPaintEvent *)
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
    painter.drawRoundedRect(bmp.rect(), 10, 10);
    setMask(bmp);
}
