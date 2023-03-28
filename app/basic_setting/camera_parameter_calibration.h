#ifndef CAMERA_PARAMETER_CALIBRATION_H
#define CAMERA_PARAMETER_CALIBRATION_H

#include <QWidget>
#include <QPixmap>
#include <memory>

namespace Ui {
class CameraParameterCalibrationUi;
}

class CameraParamCaliController;

class CameraParameterCalibration : public QWidget
{
    Q_OBJECT
public:
    explicit CameraParameterCalibration(QWidget *parent = nullptr);
    ~CameraParameterCalibration();

    QSize getLiveWidgetSize();
    void setCaptureBtnEnable(bool bEnable);
    void setSaveBtnEnable(bool bEnable);
    void showResult();
    void showCalibration();
    void setResultText(const QString &text);
    void setMaskEnable(bool enabled);
    void setLiveMaskEnable(bool enabled);

signals:
    void backToPreviousView();
    void enterCalibration();
    void exitCalibration();

private slots:
    void onUpdateLive(QPixmap frame);
    void onUpdateText(QString text);

private:
    bool eventFilter(QObject *watched, QEvent *event) override;
    void initMask();
    QWidget *createMaskForWidget(QWidget *parent_widget, bool text_required);

    std::unique_ptr<Ui::CameraParameterCalibrationUi> m_ui;
    std::unique_ptr<CameraParamCaliController> m_controller;
    QWidget* m_mask;
    QWidget* m_live_mask;
};

#endif // CAMERA_PARAMETER_CALIBRATION_H
