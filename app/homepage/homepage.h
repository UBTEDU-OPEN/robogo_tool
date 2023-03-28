#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include <QWidget>
#include <memory>
//#include "../ota/otawidget.h"

namespace Ui {
class HomepageUi;
}

class HomepageController;

class Homepage : public QWidget
{
    Q_OBJECT
public:
    explicit Homepage(QWidget *parent = nullptr);
    ~Homepage();

signals:
    void aboutToHide();
    void aboutToShow();
    void updateModalState(bool is_modal);
    /************************************
    * 名称:sigCheckOta
    * 功能:检查升级
    * 参数:有新版本是否弹提示
    * 返回:
    * 时间:2021/03/22
    * 作者:Joker
    ************************************/
    void sigCheckOta(bool bHomePageCheck);

public slots:
    /**
     * @brief onUpdateAngleValues
     *  设置x,y,x三个轴的角度值
     * @param x
     *  x轴角度值
     * @param y
     *  y轴角度值
     * @param z
     *  z轴角度值
     */
    void onUpdateAngleValues(float x, float y, float z);

    /**
     * @brief onConnectionStatusChanged
     *  处理设备连接状态变更事件
     * @param connected
     *  true: 设备处于连接状态
     *  false: 设备处于断开状态
     */
    void onConnectionStatusChanged(bool connected);

    // QWidget interface
protected:
    void paintEvent(QPaintEvent *event) override;
    void hideEvent(QHideEvent *event) override;
    void showEvent(QShowEvent *event) override;

private:
    std::unique_ptr<Ui::HomepageUi> m_ui;
    std::unique_ptr<HomepageController> m_controller;

};



#endif // HOMEPAGE_H
