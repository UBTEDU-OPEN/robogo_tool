#include "ota_aibox_guide.h"
#include "ui_ota_aibox_guide.h"
#include <QStyleOption>
#include <QPainter>
#include <QBitmap>

OtaAiboxGuide::OtaAiboxGuide(QString strSource, QString strDst, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ota_aibox_guide)
{
    ui->setupUi(this);
    Qt::WindowFlags flags = Qt::FramelessWindowHint| Qt::Dialog;
    this->setWindowFlags(flags);
    QString strText1 = tr("检查到你的AIBOX当前版本为") + strSource + tr("，与RoboGo的软件不匹配") + tr("请使用其他工具把AIBOX升级到") + strDst + tr("，否则会影响RoboGo的功能");;
//    QString strText2 =
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

OtaAiboxGuide::~OtaAiboxGuide()
{
    delete ui;
}

void OtaAiboxGuide::paintEvent(QPaintEvent *)
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

void OtaAiboxGuide::on_pBOk_clicked()
{
    accept();
}

void OtaAiboxGuide::on_pb_close_clicked()
{
    accept();
}
