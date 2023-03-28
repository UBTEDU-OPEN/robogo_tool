#include "close_dialog.h"
#include "ui_close_dialog.h"
#include <QStyleOption>
#include <QPainter>
#include <QBitmap>

CloseDialog::CloseDialog(QWidget *parent)
    : QDialog(parent),
      m_ui(new Ui::CloseDialogUi)
{
    m_ui->setupUi(this);
//    setWindowModality(Qt::ApplicationModal);
//    Qt::WindowFlags flags = Qt::FramelessWindowHint;
//    this->setWindowFlags(flags);
    Qt::WindowFlags flags = Qt::FramelessWindowHint | Qt::Dialog;
    this->setWindowFlags(flags);
    connect(m_ui->pb_close, &QPushButton::released,
            this, [this](){
        this->reject();
     }
    );

    connect(m_ui->pb_cancel, &QPushButton::released,
            this, [this](){
        this->reject();
     }
    );

    connect(m_ui->pb_ok, &QPushButton::released,
            this, [this](){
        this->accept();
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

CloseDialog::~CloseDialog()
{

}

void CloseDialog::setMessage(const QString &message)
{
    m_ui->label->setText(message);
}

void CloseDialog::setTitileText(const QString &text)
{
   m_ui->lb_title->setText(text);
}

void CloseDialog::setOkButtonText(const QString &text)
{
    m_ui->pb_ok->setText(text);
}

void CloseDialog::setCancelButtonText(const QString &text)
{
    m_ui->pb_cancel->setText(text);
}

void CloseDialog::paintEvent(QPaintEvent *)
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
