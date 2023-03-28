#include "connection_guidelines.h"
#include "ui_connection_guidelines.h"

ConnectionGuidelines::ConnectionGuidelines(QWidget *parent)
    : QDialog(parent),
      m_ui(new Ui::ConnectionGuidelinesDialog)
{
    m_ui->setupUi(this);
    setWindowModality(Qt::ApplicationModal);
    Qt::WindowFlags flags = Qt::FramelessWindowHint;
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
        move(parent->width()/2 - this->width()/2, parent->height()/2 - this->height()/2);
    }

//    if (parent)
//    {
//       QPoint global_pos = parent->mapToGlobal(parent->pos());
//    }
//
//    move(300, 180);//
}

ConnectionGuidelines::~ConnectionGuidelines()
{

}

