#ifndef VIDEO_WIDGET_H
#define VIDEO_WIDGET_H

#include <QWidget>
#include <memory>

namespace Ui {
class VideoWidgetUi;
}

class VlcInstance;
class VlcMedia;
class VlcMediaPlayer;

class VideoWidget : public QWidget
{
    Q_OBJECT
public:
    explicit VideoWidget(QWidget *parent = nullptr);
    ~VideoWidget();

    void setVideoSource(const QString &video_path);
    void stop();
signals:

public slots:

private slots:
    void onPlayClicked();

private:
    bool loadVlc(const QString &filepath);

    std::unique_ptr<Ui::VideoWidgetUi> m_ui;
    VlcInstance* m_instance;
    VlcMedia* m_media;
    VlcMediaPlayer* m_player;

};

#endif // VIDEO_WIDGET_H
