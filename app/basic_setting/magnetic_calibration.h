#ifndef MAGNETIC_CALIBRATION_H
#define MAGNETIC_CALIBRATION_H

#include <QWidget>
#include <memory>

namespace Ui {
class MagneticUi;
}

class MagneticFieldController;

class MagneticCalibration : public QWidget
{
    Q_OBJECT
public:
    enum class Status {
        Idle,
        Calibrating,
    };


    explicit MagneticCalibration(QWidget *parent = nullptr);
    ~MagneticCalibration();

    void setButtonsStatus(MagneticCalibration::Status status);
    bool isOnCalibration() const;

signals:
    void backToPreviousView();
    void enterCalibration(QString toast_message, QString dialog_message);
    void exitCalibration();
    void aboutToHide();
    void aboutToShow();

    // QWidget interface
protected:
    void hideEvent(QHideEvent *event) override;
    void showEvent(QShowEvent *event) override;

private slots:
    void onUpdatingMagneticFieldValues(int32_t x, int32_t y, int32_t z);
    void onUpdatingMagneticFieldOffsets(int32_t x, int32_t y, int32_t z);

private:
    std::unique_ptr<Ui::MagneticUi> m_ui;
    std::unique_ptr<MagneticFieldController> m_controller;
    volatile bool m_on_cali;

};

#endif // MAGNETIC_CALIBRATION_H
