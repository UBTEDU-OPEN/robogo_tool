#ifndef ADVANCED_SETTING_H
#define ADVANCED_SETTING_H

#include <QWidget>
#include <memory>

namespace Ui {
class AdvancedSettingUi;
}

class AdvancedSettingController;

class AdvancedSetting : public QWidget
{
    Q_OBJECT
public:
    explicit AdvancedSetting(QWidget *parent = nullptr);
    ~AdvancedSetting();

signals:
    void updateModalState(bool is_modal);

    /************************************
    * 名称:sigChangeOtawidget
    * 功能:切换到Ota升级界面
    * 参数:bNeedCheck 切换时，是否需要查询
    * 返回:
    * 时间:2021/03/22
    * 作者:Joker
    ************************************/
    void sigChangeOtawidget(bool bNeedCheck);

public slots:
    void onPbCheckOtaClick();

private:
    std::unique_ptr<Ui::AdvancedSettingUi> m_ui;
    std::unique_ptr<AdvancedSettingController> m_controller;
};

#endif // ADVANCED_SETTING_H
