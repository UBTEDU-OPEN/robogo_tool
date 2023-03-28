#include "guide_step.h"
#include "ui_guide_step.h"
//#include "video_widget.h"

GuideStep::GuideStep(uint8_t index, QString text, GuideStepType guide_type, QString resource, QWidget *parent, GuidePosition pos)
    : QWidget(parent),
      m_ui(new Ui::GuideStepUi),
      m_text(text),
      m_type(guide_type),
      m_resource(resource)
//      m_video(nullptr)
{
    m_ui->setupUi(this);

    m_ui->lb_num->setText(QString::number(index));
    m_ui->lb_text->setText(m_text);
    if (pos == First)
    {
        m_ui->pb_back->hide();
        m_ui->lb_left_spacer->show();
        m_ui->lb_right_spacer->hide();
    }
    else if(pos == Last)
    {
        m_ui->pb_forward->hide();
        m_ui->lb_left_spacer->hide();
        m_ui->lb_right_spacer->show();
    }
    else if(pos == Single)
    {
        m_ui->pb_forward->hide();
        m_ui->pb_back->hide();
        m_ui->lb_num->hide();
        m_ui->lb_left_spacer->show();
        m_ui->lb_right_spacer->show();
    }
    else
    {
        m_ui->lb_left_spacer->hide();
        m_ui->lb_right_spacer->hide();
    }

    if (m_type == GuideStep::Image)
    {
        QLabel* lb_image = new QLabel(this);
        m_ui->placeholder->addWidget(lb_image);
        lb_image->setPixmap(QPixmap(resource));
    }
    else if (m_type == GuideStep::Video)
    {
//        m_video = new VideoWidget(this);
//        m_video->setVideoSource(resource);
//        m_ui->placeholder->addWidget(m_video);
    }

    connect(m_ui->pb_back, &QPushButton::released, this, &GuideStep::onChangingSteps);
    connect(m_ui->pb_forward, &QPushButton::released, this, &GuideStep::onChangingSteps);
}

GuideStep::~GuideStep()
{

}

void GuideStep::onParentClosed()
{
//    if (m_video)
//    {
//        m_video->stop();
//    }
}

void GuideStep::onChangingSteps()
{
//    if (m_video)
//    {
//        m_video->stop();
//    }
    QObject* obj = sender();

    QPushButton* button = qobject_cast<QPushButton*>(obj);
    if (button == m_ui->pb_back)
    {
        emit goToNextStep(-1);
    }
    else if (button == m_ui->pb_forward)
    {
        emit goToNextStep(1);
    }

}
