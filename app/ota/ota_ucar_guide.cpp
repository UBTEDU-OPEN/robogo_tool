#include "ota_ucar_guide.h"
#include "ui_otaucarguide.h"
#include <QStyleOption>
#include <QPainter>
#include <QBitmap>

OtaUcarGuide::OtaUcarGuide(bool bIsImporttant, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OtaUcarGuide)
{
    ui->setupUi(this);
    Qt::WindowFlags flags = Qt::FramelessWindowHint | Qt::Dialog;
    this->setWindowFlags(flags);
    if (bIsImporttant)
    {
        ui->label_1->setText(tr("检测到RoboGo小车软件有重要版本更新,优化了车子的重点功能与体验，请更新！"));
        ui->lb_level_normal->hide();
    }
    else
    {
        ui->label_1->setText(tr("检查到RoboGo小车软件有最新版本"));
        ui->lb_level_important->hide();
    }
    if(parent)
    {
        QPoint global_pos = parent->mapToGlobal(QPoint(0, 0));
        auto parent_size = parent->size();
        auto my_size = this->size();
        move(global_pos.x() + (parent->width()-my_size.width())/2, global_pos.y() + (parent_size.height()-my_size.height())/2);
    }

}

OtaUcarGuide::~OtaUcarGuide()
{
    delete ui;
}

void OtaUcarGuide::paintEvent(QPaintEvent *)
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

void OtaUcarGuide::on_pBDoUpdate_clicked()
{
    accept();
}

void OtaUcarGuide::on_pb_close_clicked()
{
      reject();
}
