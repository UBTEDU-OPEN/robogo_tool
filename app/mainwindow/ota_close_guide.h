#ifndef OTA_CLOSE_GUIDE_H
#define OTA_CLOSE_GUIDE_H

#include <QDialog>

namespace Ui {
class OtaCloseGuide;
}

class OtaCloseGuide : public QDialog
{
    Q_OBJECT

public:
    explicit OtaCloseGuide(QWidget *parent = 0);
    ~OtaCloseGuide();

protected:
    void paintEvent(QPaintEvent *);
private slots:

    void on_pBOk_clicked();

    void on_pBCancel_clicked();

    void on_pb_close_clicked();

private:
    Ui::OtaCloseGuide *ui;
};

#endif // OTA_CLOSE_GUIDE_H
