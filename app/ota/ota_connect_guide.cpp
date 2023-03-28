#include "ota_connect_guide.h"
#include "ui_ota_connect_guide.h"
#include <QStyleOption>
#include <QPainter>
#include <QBitmap>

OtaConnectGuide::OtaConnectGuide(QWidget *parent, emConnectType nType) :
    QDialog(parent),
    ui(new Ui::OtaConnectGuide)
{
    ui->setupUi(this);
    Qt::WindowFlags flags = Qt::FramelessWindowHint | Qt::Dialog;
    this->setWindowFlags(flags);
    if (nType == eConnectTypeUcar)
    {
        ui->label->setText(tr("小车底盘没打开，请先给小车开机。"));
    }
    else if (nType == eConnectTypeNetwork)
    {
        ui->label->setText(tr("电脑网络连接失败，无法进行升级尝试，请先确认电脑网络连接正常"));
    }
    else if (nType == eOtaUpdateFailedTips)
    {
        ui->label->setText(tr("升级不成功，请查看小车是否处于开启状态，再重新升级，如多次升级不成功，请联系售后服务团队获取支持。"));
    }

    if(parent)
    {
        QPoint global_pos = parent->mapToGlobal(QPoint(0, 0));
        auto parent_size = parent->size();
        auto my_size = this->size();
        move(global_pos.x() + (parent->width()-my_size.width())/2, global_pos.y() + (parent_size.height()-my_size.height())/2);
    }
}

OtaConnectGuide::~OtaConnectGuide()
{
    delete ui;
}

void OtaConnectGuide::paintEvent(QPaintEvent *)
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

void OtaConnectGuide::on_pBOk_clicked()
{
    accept();
}

void OtaConnectGuide::on_pb_close_clicked()
{
     accept();
}
