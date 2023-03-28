#include "video_widget.h"
#include "ui_video_widget.h"

#include <VLCQtCore/Common.h>
#include <VLCQtCore/Instance.h>
#include <VLCQtCore/Media.h>
#include <VLCQtCore/MediaPlayer.h>
#include <VLCQtWidgets/WidgetVolumeSlider.h>

#include <QDebug>

VideoWidget::VideoWidget(QWidget *parent)
    : QWidget(parent),
      m_ui(new Ui::VideoWidgetUi),
      m_media(0)
{
    m_ui->setupUi(this);

//    m_ui->widget->setAutoFillBackground(true);
}

VideoWidget::~VideoWidget()
{
    qDebug() << __func__;

    if (m_player->state() == Vlc::State::Playing)
    {
        m_player->stop();
    }
    delete m_player;
    delete m_media;
    delete m_instance;
}

void VideoWidget::setVideoSource(const QString &video_path)
{

    m_instance = new VlcInstance(VlcCommon::args(), this);
    m_player = new VlcMediaPlayer(m_instance);
    m_player->setVideoWidget(m_ui->widget);

    m_ui->widget->setMediaPlayer(m_player);
    m_ui->volume->setMediaPlayer(m_player);
    m_ui->volume->setVolume(50);
    m_ui->seek->setMediaPlayer(m_player);

    m_media = new VlcMedia(video_path, true,m_instance);
    m_player->open(m_media);
    m_player->pause();
    m_ui->pb_pause->hide();

    connect(m_ui->pb_play, &QPushButton::pressed, this, &VideoWidget::onPlayClicked);
    connect(m_ui->pb_pause, &QPushButton::pressed, this, &VideoWidget::onPlayClicked);
    connect(m_player, &VlcMediaPlayer::end, this, &VideoWidget::onPlayClicked);
}


void VideoWidget::onPlayClicked()
{
    qDebug() << m_player->state();
    if (m_player->state() == Vlc::State::Playing)
    {
        m_ui->pb_pause->hide();
        m_ui->pb_play->show();
        m_player->pause();
    }
    else if (m_player->state() == Vlc::State::Paused)
    {
        m_ui->pb_pause->show();
        m_ui->pb_play->hide();
        m_player->play();
    }
    else if (m_player->state() == Vlc::State::Ended)
    {
        m_ui->pb_pause->hide();
        m_ui->pb_play->show();
        m_player->open(m_media);
        m_player->pause();
    }
    else
    {
        m_ui->pb_pause->hide();
        m_ui->pb_play->show();
    }
}

void VideoWidget::stop()
{
    if (m_player->state() == Vlc::State::Playing)
    {
        m_player->pause();
        m_ui->pb_pause->hide();
        m_ui->pb_play->show();
    }
}
