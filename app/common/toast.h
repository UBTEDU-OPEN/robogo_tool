#ifndef TOAST_H
#define TOAST_H

#include <QWidget>
#include <QTimer>
#include <memory>

class MessageInfo;
class PresenterAction;
class ToastPresenter;

namespace Ui {
class Toast;
}

class Toast : public QWidget
{
    Q_OBJECT
public:
    explicit Toast(QString message, QWidget* parent = nullptr);
    ~Toast() override;

protected:
    void paintEvent(QPaintEvent *event) override;
    void showEvent(QShowEvent *event) override;

private:
    Ui::Toast *m_ui;
    QTimer m_timer;
};

#endif // TOAST_H
