#ifndef MAGNETIC_FIELD_CONTROLLER_H
#define MAGNETIC_FIELD_CONTROLLER_H

#include <QObject>
#include <QTimer>

class MagneticCalibration;
class QProcess;

class MagneticFieldController : public QObject
{
    Q_OBJECT

    const QString CLOSE_CONFIRM_MESSAGE{QObject::tr("正在校准中，是否关闭RoboGo软件工具？如果关闭工具，将保存实时校准数据。")};
    const QString TAB_FORBIDDEN_MESSAGE{QObject::tr("正在校准中，不可切换界面")};
    const QString BACK_DIALOG_MESSAGE{QObject::tr("正在校准中，是否退出磁场校准流程？若退出流程，将保存实时校准数据。")};
    const QString BTN_TEXT_CONTINUE{QObject::tr("继续校准")};
    const QString BTN_TEXT_QUIT{QObject::tr("退出")};
    const QString TITLE_TEXT_EXIT_CAL{QObject::tr("退出校准")};

public:
    explicit MagneticFieldController(MagneticCalibration* view, QObject *parent = nullptr);
    ~MagneticFieldController();

signals:
    void setMagneticFieldValues(int32_t x, int32_t y, int32_t z);
    void setMagneticFieldOffsets(int32_t x, int32_t y, int32_t z);

public slots:
    void onBackButtonClicked();
    void onGuideButtonClicked();
    void onStartButtonClicked();
    void onFinishButtonClicked();
    void onViewHideEvent();
    void onViewShowEvent();

private slots:
    void onReadMagneticData(int32_t x, int32_t y, int32_t z);
    void onGetAndUpdateOffsets();
    void onLostConnection();
    void onGuideClosed(int code);

private:
    MagneticCalibration* m_view;
    int32_t m_offset_x, m_offset_y, m_offset_z;
    QTimer m_offline_check_timer;
    QProcess* m_opeartion_guide_process;
};

#endif // MAGNETIC_FIELD_CONTROLLER_H
