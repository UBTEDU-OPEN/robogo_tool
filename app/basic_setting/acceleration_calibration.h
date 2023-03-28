#ifndef ACCELERATION_CALIBRATION_H
#define ACCELERATION_CALIBRATION_H

#include <QWidget>
#include <memory>

namespace Ui {
class AccelUi;
}

class AccelerationCalibrationController;

class AccelerationCalibration : public QWidget
{
    Q_OBJECT
public:
    enum class Status {
        Idle,
        Calibrating,
        Finished
    };

    explicit AccelerationCalibration(QWidget *parent = nullptr);
    ~AccelerationCalibration();

    void setButtonsStatus(AccelerationCalibration::Status status);

signals:
    void backToPreviousView();
    void aboutToHide();
    void enterCalibration(QString toast_message, QString dialog_message);
    void exitCalibration();

public slots:
    void onUpdateValues(int32_t x, int32_t y, int32_t z);
    void onUpdateProgress(uint8_t value);

protected:
    void paintEvent(QPaintEvent *event) override;
    void hideEvent(QHideEvent *event) override;

private:
    std::unique_ptr<Ui::AccelUi> m_ui;
    std::unique_ptr<AccelerationCalibrationController> m_controller;
};

#endif // ACCELERATION_CALIBRATION_H
