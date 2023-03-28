#include "camera_parameter_guide.h"
#include "ui_camera_parameter_guide.h"
#include <QStyleOption>
#include <QPainter>
#include <QBitmap>

CameraParameterGuide::CameraParameterGuide(QWidget *parent)
    : QDialog(parent),
      m_ui(new Ui::CameraParameterGuideUi)
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
        auto parent_size = parent->size();
        auto my_size = this->size();
        move(global_pos.x() + (parent->width()-my_size.width())/2, global_pos.y() + (parent_size.height()-my_size.height())/2);
    }
}

CameraParameterGuide::~CameraParameterGuide()
{
}

void CameraParameterGuide::paintEvent(QPaintEvent *)
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
