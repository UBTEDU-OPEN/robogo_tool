#ifndef BASIC_SETTING_H
#define BASIC_SETTING_H

#include <QWidget>
#include <memory>

namespace Ui {
class BasicSettingUi;
}

class BasicSettingController;

class BasicSetting : public QWidget
{
    Q_OBJECT
public:
    explicit BasicSetting(QWidget *parent = nullptr);
    ~BasicSetting();

signals:
    void enterCalibration(QString toast_message, QString dialog_message);
    void exitCalibration();

public slots:
    /**
     * @brief pushWidgetToStack
     * 将子页面添加到stackedWidget中，并在最顶端显示
     * @param widget
     * 要添加的子页面
     */
    void pushWidgetToStack(QWidget *widget);

    /**
     * @brief popWidgetFromStack
     * 将子页面从栈中弹出
     * @param widget
     * 要弹出的子页面
     */
    void popWidgetFromStack(QWidget *widget);

private:
    std::unique_ptr<Ui::BasicSettingUi> m_ui;
    std::unique_ptr<BasicSettingController> m_controller;
};

#endif // BASIC_SETTING_H
