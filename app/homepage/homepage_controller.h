#ifndef HOMEPAGE_CONTROLLER_H
#define HOMEPAGE_CONTROLLER_H

#include <QObject>
#include <QTimer>
#include <memory>

class HomepageController : public QObject
{
    Q_OBJECT
public:
    explicit HomepageController(QWidget *view);
    ~HomepageController() = default;

signals:
    void updateAngleValues(float x, float y, float z);
    void updateConnectionStatus(bool connected);
    void setWindowModal(bool on);

public slots:
    /**
     * @brief onGuidelinesClicked
     * 连接指引按钮处理函数
     */
    void onGuidelinesClicked(void);

    void onViewHideEvent(void);
    void onViewShowEvent(void);

private slots:
    void onReadAngleData(float x, float y, float z);
    void onLostConnection();
    void onOfflineTimerTriggered();

private:
    QWidget* m_view;
    QTimer m_timer;
    QTimer m_offline_repeat_timer;
    QTimer m_auto_guide_timer;
    bool m_online;
};

#endif // HOMEPAGE_CONTROLLER_H
