#ifndef OTAWIDGET_H
#define OTAWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QFile>
#include <QPainter>
#include <QThread>
#include <QMetaType>

#include "httptransfer.h"
#include "httpdownload.h"
#include "ota_connect_guide.h"

Q_DECLARE_METATYPE(emConnectType)

class otaFileSending:public QObject
{
    Q_OBJECT


public:
    otaFileSending();
    ~otaFileSending();
public slots:
    void onTimeoutForSending(QFile* ota_file, int nAmount);

signals:
    void sigSetOtaProgessProcess(int nProcess);

    void sigSendingFinished();

    void sigUpgradeError(emConnectType nType);

private:
    QThread m_oThread;
};



namespace Ui {
class otawidget;
}


class otawidget : public QWidget
{
    Q_OBJECT

public:
    explicit otawidget(QWidget *parent = 0);
    ~otawidget();


    /**************************************************************************
    * 函数名: QueryOTAVersion
    * 功能:查询OTA版本
    * 参数: [@in] strProductName  产品名
    * 参数: [@in] strLanguage     语言名
    * 参数: [@in] strGroupVersion   版本名
    * 参数: [@in] bReply        是否需要回复web
    * 返回值:void
    * 时间:2021/03/24
    * 作者: Joker
    */
    virtual  int QueryOTAVersion(const QString& strProductName, const QString strLanguage, const QString & strGroupVersion, bool bHomePageCheck);

    bool checkIsOnline();
    bool isLinkLocalAddress(QHostAddress addr);

signals:
    void updateModalState(bool is_modal);
    void sigReturnSetting();
    /************************************
    * 名称:sigShowOtaWidget
    * 功能:切换到Ota升级界面
    * 参数:bNeedCheck 切换时，是否需要查询新版本
    * 返回:
    * 时间:2021/03/22
    * 作者:Joker
    ************************************/
    void sigShowOtaWidget(bool bNeedCheck);
    void setOtaWindowModal(bool on);
    void sigTimeoutForSending(QFile* ota_file, int nAmount);
public slots:
    void onDoCheckOta(bool bHomePageCheck);
    void onPBReturnClicked();

    void onOtaResult(bool bSuccess, const OTAVersionInfo &otaData, bool bHomePageCheck);


    void OnDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void OnFinishedDownload(bool bsuccess);
    void onOtaUpdateTimeout();

public:

    /**************************************************************************
    * 函数名: doBurning
    * 功能: 烧录版本
    * 参数: [@in] strFilePath 文件全路径
    * 参数: [@in] strMd5  md5
    * 时间:2021/03/24
    * 作者: Joker
    */
    bool doBurning();

    bool handleVerifyMd5(const QString &strFilePath, const QString &strMd5);

public slots:
    void UpdateFailed(emConnectType nType);

    void SetOtaProgessProcess(int nProcess);



protected:
    void paintEvent(QPaintEvent *event);
private slots:
    void on_pb_return_clicked();

    void on_pbDoUpdate_clicked();

//    void onTimeoutForSending();

    void onSendingFinished();

private:
    Ui::otawidget *ui;
    HttpTransfer*     m_pHttpTrans;       //http传输类,负责云端数据查询
    HttpDownload*     m_pHttpDownload;    //http下载类，负责下载操作

    QString m_strUrl;
    QString m_strLocalUcarVersion;
    QString m_strMd5;
    QString m_strNewAiboxVersion; //后台获取的最新Aibox版本
//    QString  m_strReleaseNote;
    QTimer m_timer;
//    QTimer m_sending_timer;
    int m_nTimerCalc; //升级时间计算
    int m_amount;
    int m_current_pkg_num;
    QFile m_ota_file;
    bool m_bFirstCheckOta;
    otaFileSending m_sender;
};


#endif // OTAWIDGET_H
