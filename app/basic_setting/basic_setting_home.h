#ifndef BASIC_SETTING_HOME_H
#define BASIC_SETTING_HOME_H

#include <QWidget>
#include <memory>

namespace Ui {
class BasicSettingHomeUi;
}

class BasicSettingHome : public QWidget
{
    Q_OBJECT
public:
    explicit BasicSettingHome(QWidget *parent = nullptr);
    ~BasicSettingHome();

signals:
    void accelButtonPressed();
    void magButtonPressed();
    void perspectiveButtonPressed();
    void cameraParamBtnPressed();
    void cameraParamInfoBtnPressed();
    void cameraParamRestoreBtnPressed();
    void cameraPicBrowserBtnPressed();
    void rollbackButtonPressed();
    void aboutToShow();

    // QWidget interface
protected:
    void showEvent(QShowEvent *event) override;

private:
    std::unique_ptr<Ui::BasicSettingHomeUi> m_ui;

};

#endif // BASIC_SETTING_HOME_H
