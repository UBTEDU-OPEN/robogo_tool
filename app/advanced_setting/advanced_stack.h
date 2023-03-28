#ifndef ADVANCED_STACK_H
#define ADVANCED_STACK_H

#include <QWidget>

namespace Ui {
class Advanced_Stack;
}

class Advanced_Stack : public QWidget
{
    Q_OBJECT

public:
    explicit Advanced_Stack(QWidget *parent = 0);
    ~Advanced_Stack();

signals:
    void sigCheckOta(bool bHomePageCheck);
    void sigShowOtaWidget();
    void sigAdvancedStackTab();
    void setWindowModal(bool on);
    void setOtaWindowModal(bool on);
public slots:

    /************************************
    * 名称:onChangeOtawidget
    * 功能:切换到Ota升级界面
    * 参数:bNeedCheck 切换时，是否需要查询新版本
    * 返回:
    * 时间:2021/03/22
    * 作者:Joker
    ************************************/
    void onChangeOtawidget(bool bNeedCheck);
    void onChangeAdvancedSet();

private:
    Ui::Advanced_Stack *ui;
};

#endif // ADVANCED_STACK_H
