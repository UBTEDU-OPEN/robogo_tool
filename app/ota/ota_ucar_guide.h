#ifndef OTA_UCAR_GUIDE_H
#define OTA_UCAR_GUIDE_H

#include <QDialog>

namespace Ui {
class OtaUcarGuide;
}

class OtaUcarGuide : public QDialog
{
    Q_OBJECT

public:
    explicit OtaUcarGuide(bool bIsImporttant, QWidget *parent = 0);
    ~OtaUcarGuide();

void paintEvent(QPaintEvent *);

private slots:
    void on_pBDoUpdate_clicked();

    void on_pb_close_clicked();

private:
    Ui::OtaUcarGuide *ui;
};

#endif // OTA_UCAR_GUIDE_H
