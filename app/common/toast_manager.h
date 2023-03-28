#ifndef TOAST_MANAGER_H
#define TOAST_MANAGER_H

#include "singleton.h"
#include <QObject>
#include <memory>

class Toast;

class ToastManager : public QObject, Singleton<ToastManager>
{
    Q_OBJECT
public:
    /**
     * @brief instance
     * 返回单例对象
     * @return
     */
    static ToastManager &instance();

    /**
     * @brief setRootWidget
     * 设置显示toast的widget对象, 即toast的父控件
     * @param view
     * toast的父控件
     */
    void setRootWidget(std::shared_ptr<QWidget> view);

    /**
     * @brief show
     * 显示toast
     * @param message
     * toast消息内容
     */
    void show(const QString &message);

    /**
     * @brief release
     * 释放未析构的toast
     */
    void release();

    ~ToastManager();

protected:
    explicit ToastManager(QObject *parent = nullptr);

private:
    static ToastManager* m_instance;
    std::weak_ptr<QWidget> m_view;
    std::shared_ptr<Toast> m_toast;
};

#endif // TOAST_MANAGER_H
