#ifndef OTA_CONNECT_GUIDE_H
#define OTA_CONNECT_GUIDE_H

#include <QDialog>

enum emConnectType
{
    eConnectTypeUcar = 0, //ucar未连接提示
    eConnectTypeNetwork=1,  //网络未连接提示
    eOtaUpdateFailedTips  //升级失败提示
};

namespace Ui {
class OtaConnectGuide;
}

class OtaConnectGuide : public QDialog
{
    Q_OBJECT

public:
    explicit OtaConnectGuide(QWidget *parent = 0, emConnectType nType = eConnectTypeUcar);
    ~OtaConnectGuide();

void paintEvent(QPaintEvent *);

private slots:
    void on_pBOk_clicked();

    void on_pb_close_clicked();

private:
    Ui::OtaConnectGuide *ui;
};

#endif // OTA_CONNECT_GUIDE_H
