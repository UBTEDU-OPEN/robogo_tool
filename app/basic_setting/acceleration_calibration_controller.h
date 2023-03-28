#ifndef ACCELERATION_CALIBRATION_CONTROLLER_H
#define ACCELERATION_CALIBRATION_CONTROLLER_H

#include <QObject>
#include <QTimer>

class AccelerationCalibration;

class AccelerationCalibrationController : public QObject
{

    Q_OBJECT

    const QString CLOSE_CONFIRM_MESSAGE{QObject::tr("正在校准中，是否关闭RoboGo软件工具？如果关闭工具，校准数据将丢失。")};
    const QString TAB_FORBIDDEN_MESSAGE{QObject::tr("正在校准中，不可切换界面")};

public:
    explicit AccelerationCalibrationController(AccelerationCalibration* view, QObject *parent = nullptr);
    ~AccelerationCalibrationController();

signals:
    void setAccelValues(int32_t x, int32_t y, int32_t z);
    void setProgress(uint8_t value);

public slots:
    void onBackButtonPressed();
    void onStartButtonPressed();
    void onSaveButtonPressed();
    void onViewHiding(void);

private slots:
    void onReadAccelData(float x, float y, float z);
    void onUpdateProgress();
    void onGetAndUpdateOffsets();
    void onLostConnection();

private:
    AccelerationCalibration* m_view;
    QTimer m_progress_timer;
    uint8_t m_progress;
    QTimer m_offline_check_timer;
};



#endif // ACCELERATION_CALIBRATION_CONTROLLER_H
