#include "guide_version.h"
#include "ui_guide_version.h"
#include <QStyleOption>
#include <QPainter>
#include <QBitmap>

GuideUcarVersion::GuideUcarVersion(QString strVersion, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::guide_ucar_version)
{
    ui->setupUi(this);
    Qt::WindowFlags flags = Qt::FramelessWindowHint| Qt::Dialog;
    this->setWindowFlags(flags);
    QString strText1 = tr("当前底盘固件版本为") + strVersion + tr("，版本号过低，不支持此功能,") + tr("请先在高级配置->小车升级检查中对底盘进行ota升级");
//    QString strText2 = tr("请先在高级配置->小车升级检查中对底盘进行ota升级");
    ui->label->setText(strText1);
//    ui->label_2->setText(strText2);
    if(parent)
    {
        QPoint global_pos = parent->mapToGlobal(QPoint(0, 0));
        auto parent_size = parent->size();
        auto my_size = this->size();
        move(global_pos.x() + (parent->width()-my_size.width())/2, global_pos.y() + (parent_size.height()-my_size.height())/2);
    }
}

GuideUcarVersion::~GuideUcarVersion()
{
    delete ui;
}

void GuideUcarVersion::paintEvent(QPaintEvent *)
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

void GuideUcarVersion::on_pBOk_clicked()
{
    accept();
}

void GuideUcarVersion::on_pb_close_clicked()
{
    accept();
}
