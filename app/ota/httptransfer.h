/*
* Copyright (c) 2020, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：httptransfer.h
* 创建时间：2021/03/24
* 文件标识：
* 文件摘要：http传输类，完成http指令请求和数据的获取
*
* 当前版本：1.0.0.0
* 作    者：Joker
* 完成时间：2021/03/24
* 版本摘要：
*/
#ifndef HTTPTRANSFER_H
#define HTTPTRANSFER_H
//#include "UBXPublic.h"
//#include "ActUpdateMangr.h"
#include <QObject>
#include <QMutexLocker>
#include <QMutex>
#include <QThread>
#include <QMap>
#include <QVector>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include "otaconst.h"


typedef struct _OTAVersionInfo
{
  char ucarVersion[32];
  char AiboxVersion[32];
  char packageUrl[256];
  char packageMd5[64];
  long long  packageSize;
  bool isForced;
  char releaseNote[1024];
  STRUCT_INIT(_OTAVersionInfo)
}OTAVersionInfo;

class HttpTransfer : public QObject
{
    Q_OBJECT
public:
    HttpTransfer();
    ~HttpTransfer();

public:
    /************************************
    * 名称: stopThread
    * 功能: 退出线程
    * 参数:
    * 返回:
    * 时间:   2021/03/22
    * 作者:   Joker
    ************************************/
    void stopThread();

public:
    /************************************
    * 名称: getQueryOTAVersion
    * 功能: 查询OTA版本
    * 参数: [in]serverURL 请求资源地，
    * 参数: [in]nOldRequestID 上次请求的ID，若还没完成，可以结束上次请求
    * 返回:   int 返回请求ID,外部监听信号时用于判断是否是自己需要的消息
    * 时间:   2020/03/24
    * 作者:   Joker
    ************************************/
    int getQueryOTAVersion(const QString& serverURL, int nOldRequestID, bool bHomePageCheck);

signals:
    /************************************
    * 名称: SigGetQueryOTAVersion
    * 功能: 查询OTA版本
    * 参数: [in]nNewRequestID 上次请求的ID，若还没完成，可以结束上次请求
    * 参数: [in]serverURL 请求资源地，
    * 返回:   int 返回请求ID,外部监听信号时用于判断是否是自己需要的消息
    * 时间:   2020/03/24
    * 作者:   Joker
    ************************************/
    void SigGetQueryOTAVersion(int nNewRequestID, const QString& serverURL, bool bHomePageCheck);

    /************************************
    * 名称: SigInit
    * 功能: 初始化,先运行线程，然后通过信号的方式通知线程初始化,确保网络对象属于线程
    * 返回:   void
    * 时间:   2020/04/07
    * 作者:   Joker
    ************************************/
    void SigInit();

    void sigOtaResult(bool bSuccess, const OTAVersionInfo &otaData, bool bHomePageCheck);

protected:
    // 清除应答数据
    void resetResp();

    // 更新、获取请求ID
    int createUnusedRequestID();

protected slots:

    /************************************
    * 名称: OnGetQueryOTAVersion
    * 功能: 查询OTA版本
    * 参数: [in]nNewRequestID 上次请求的ID，若还没完成，可以结束上次请求
    * 参数: [in]serverURL 请求资源地，
    * 返回:   int 返回请求ID,外部监听信号时用于判断是否是自己需要的消息
    * 时间:   2020/03/24
    * 作者:   Joker
    ************************************/
    void OnGetQueryOTAVersion(int nNewRequestID, const QString& serverURL, bool bHomePageCheck);

    /************************************
    * 名称: OnFinished
    * 功能: 指令请求完成槽函数
    * 返回:   void
    * 时间:   2020/03/24
    * 作者:   Joker
    ************************************/
    void OnFinished();

    /************************************
    * 名称: OnError
    * 功能: 错误处理槽函数
    * 参数: [in]QNetworkReply::NetworkError code请求错误代码
    * 返回:   void
    * 时间:   2020/03/24
    * 作者:   Joker
    ************************************/
    void OnError(QNetworkReply::NetworkError code);

    /************************************
    * 名称: OnDownloadProgress
    * 功能: 下载进度处理槽函数
    * 参数: [in]bytesReceived 已经接收到的字节数
    * 参数: [in]bytesTotal 总共字节数
    * 返回:   void
    * 时间:   2020/03/24
    * 作者:   Joker
    ************************************/
    void OnDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);

    void OnInit();

    /**************************************************************************
    * 函数名: readyRead
    * 功能: 读取后台返回结果，因获取文件列表数据很大，会分批返回，故获取文件列表以OnFinished为准
    * 参数: [@in]
    * 参数: [@in]
    * 返回值:void
    * 时间:2021/03/24
    * 作者: Joker
    */
    void readyRead();

private:
    QNetworkAccessManager *m_pNetManager;//必须在线程中new，否则get回来的reply对象会变成在主线程中，跨线程调用会崩溃
    //方式是，先运行线程，然后通过信号的方式通知线程初始化
    QThread m_oThread;//线程对象

    QMutex m_mutex; // 互斥锁
    QAtomicInt  m_nRequestID;//生成请求ID

    //Qt帮助文档对post请求的说明，因此需要保存data数据，以防发生意外，在完成后清空保存的数据
    //data must be open for reading and must remain valid until the finished() signal is emitted for this reply.
    QMap<int, QByteArray> m_mapParamData;//保存请求数据
    QMap<int, QByteArray> m_mapRecivedData;//根据请求ID保存读取到的数据
    QMap<int, QNetworkReply*> m_mapReplyFileLst;//File List请求
    QByteArray   m_bytesResponse;  //应答数据
    std::vector<std::string> getHttpHeader(const std::string &strDeviceId);
//    IOtaInterface* m_pOta;
    QString m_strFileData;  //文件列表数据，收到数据进行添加，最后再onFinish中使用并清空
};
#endif
