#ifndef NOTIFY_H
#define NOTIFY_H

#include <QDialog>
#include <memory>

namespace Ui {
    class NotifyUi;
}

class Notify: public QDialog
{
    Q_OBJECT
public:
    explicit Notify(QWidget *parent = nullptr);
    ~Notify();

    void setMessage(const QString &message);
    void setBtnText(const QString &text);

signals:
    void backButtonClicked();

public slots:

protected:
    void paintEvent(QPaintEvent *);
private:
    std::unique_ptr<Ui::NotifyUi> m_ui;
};

#endif // NOTIFY_H
