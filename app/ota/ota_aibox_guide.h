#ifndef OTA_AIBOX_GUIDE_H
#define OTA_AIBOX_GUIDE_H

#include <QDialog>

namespace Ui {
class ota_aibox_guide;
}

class OtaAiboxGuide : public QDialog
{
    Q_OBJECT

public:
    explicit OtaAiboxGuide(QString strSource, QString strDst, QWidget *parent = 0);
    ~OtaAiboxGuide();

void paintEvent(QPaintEvent *);

private slots:
    void on_pBOk_clicked();


    void on_pb_close_clicked();

private:
    Ui::ota_aibox_guide *ui;
};

#endif // OTA_AIBOX_GUIDE_H
