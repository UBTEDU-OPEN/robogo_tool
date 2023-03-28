#ifndef GUIDE_DISCONNECT_H
#define GUIDE_DISCONNECT_H

#include <QDialog>

namespace Ui {
class GuideDisconnect;
}

class GuideDisconnect : public QDialog
{
    Q_OBJECT

public:
    explicit GuideDisconnect(QWidget *parent = 0);
    ~GuideDisconnect();
private:
    void paintEvent(QPaintEvent *);
private:
    Ui::GuideDisconnect *ui;
};

#endif // GUIDE_DISCONNECT_H
