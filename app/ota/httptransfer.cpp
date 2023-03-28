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

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QDir>
#include <QUuid>
#include "ubtserverconfig.h"
#include "httptransfer.h"

HttpTransfer::HttpTransfer()
{
    m_pNetManager = NULL;
//    moveToThread(&m_oThread);
    connect(this, &HttpTransfer::SigGetQueryOTAVersion, this, &HttpTransfer::OnGetQueryOTAVersion);
    connect(this, &HttpTransfer::SigInit, this, &HttpTransfer::OnInit);

    m_nRequestID = -1;
    m_strFileData = "";
//    m_oThread.start();
    emit SigInit();
}

HttpTransfer::~HttpTransfer()
{
//    m_oThread.quit();
    if(m_pNetManager)
    {
        delete  m_pNetManager;
        m_pNetManager = NULL;
        //        SAFE_DELETE(m_pNetManager); // 出现释放打印异常
    }        m_pNetManager->disconnect();

}

/************************************
* 名称: stopThread
* 功能: 退出线程
* 参数:
* 返回:
* 时间:   2021/03/22
* 作者:   Joker
************************************/
void HttpTransfer::stopThread()
{
    m_oThread.quit();
    m_oThread.wait();
}

/************************************
* 名称: getQueryOTAVersion
* 功能: 查询OTA版本
* 参数: [in]serverURL 请求资源地，
* 参数: [in]nOldRequestID 上次请求的ID，若还没完成，可以结束上次请求
* 返回:   int 返回请求ID,外部监听信号时用于判断是否是自己需要的消息
* 时间:   2020/03/24
* 作者:   Joker
************************************/
int HttpTransfer::getQueryOTAVersion(const QString &serverURL, int nOldRequestID, bool bHomePageCheck)
{
//    if(!m_oThread.isRunning())
//        m_oThread.start();

    //先停止旧的请求，如果存在
    if(m_mapReplyFileLst.contains(nOldRequestID))
    {
        m_mapReplyFileLst[nOldRequestID]->abort();
    }

    int nRequestID = createUnusedRequestID();
    emit SigGetQueryOTAVersion(nRequestID, serverURL, bHomePageCheck);
    return nRequestID;
}

void HttpTransfer::resetResp()
{
    m_bytesResponse.clear();
}

int HttpTransfer::createUnusedRequestID()
{
    QMutexLocker lock(&m_mutex);
    ++m_nRequestID;
    return m_nRequestID;
}
/************************************
* 名称: OnGetQueryOTAVersion
* 功能: 查询OTA版本
* 参数: [in]nNewRequestID 上次请求的ID，若还没完成，可以结束上次请求
* 参数: [in]serverURL 请求资源地，
* 返回:   int 返回请求ID,外部监听信号时用于判断是否是自己需要的消息
* 时间:   2020/03/24
* 作者:   Joker
************************************/
void HttpTransfer::OnGetQueryOTAVersion(int nNewRequestID, const QString &serverURL, bool bHomePageCheck)
{
    QNetworkRequest request;
    QUrl url(serverURL);
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");


    UBTServerConfig config;
    QString strUBTSign = config.getHeaderUBTSign().c_str();

    qDebug()<< "HttpTransfer::OnPostPushFile strUBTSign =  "<< strUBTSign;
    //    request.setHeader(QNetworkRequest::ContentLengthHeader, m_mapParamData[nNewRequestID].length());
    QByteArray arUBTSign = strUBTSign.toUtf8();
    request.setRawHeader("X-UBT-Sign", arUBTSign);
    //通用认证部分
    QString strAppId =  config.getAppId().c_str();
    QString strDeviceId =  config.getDeviceID().c_str();
    //    QString strLanguage =  config.getLearnLessonLanguage();
    //    QString Token = CIniConfig::getInstance().getValueToken();
    request.setRawHeader("X-UBT-AppId", strAppId.toUtf8());
    request.setRawHeader("X-UBT-DeviceId", strDeviceId.toUtf8());
    //    request.setRawHeader("X-UBT-language", strLanguage.toUtf8());
    //    request.setRawHeader("authorization", Token.toUtf8());


    request.setAttribute(QNetworkRequest::Attribute(HTTPRequestID),qVariantFromValue((int)nNewRequestID));//设置ID

    request.setAttribute(QNetworkRequest::Attribute(HTTPOtaShowTips), bHomePageCheck);//设置类型
    request.setAttribute(QNetworkRequest::Attribute(HTTPRequestType), HTTP_REQUEST_GET_OTA_VERSION);//设置类型
    QNetworkReply* reply = m_pNetManager->get(request);
    connect(reply, &QNetworkReply::finished, this, &HttpTransfer::OnFinished);
    connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
            this, &HttpTransfer::OnError);
    connect(reply, &QNetworkReply::downloadProgress,
            this, &HttpTransfer::OnDownloadProgress);
    connect(reply, &QNetworkReply::readyRead,
            this, &HttpTransfer::readyRead);
    m_mapReplyFileLst.insert(nNewRequestID, reply);
}

/************************************
* 名称: OnFinished
* 功能: 指令请求完成槽函数
* 参数: [in]QNetworkReply* reply请求对象指针
* 返回:   void
* 时间:   2020/03/24
* 作者:   Joker
************************************/
void HttpTransfer::OnFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if(reply)
    {
        const QString sRequestType = reply->request().attribute(QNetworkRequest::Attribute(HTTPRequestType)).toByteArray();
        bool bHomePageCheck = reply->request().attribute(QNetworkRequest::Attribute(HTTPOtaShowTips)).toBool();
        reply->deleteLater();
        if (!m_strFileData.isEmpty())
        {
            if(sRequestType == HTTP_REQUEST_GET_OTA_VERSION)
            {
                QJsonParseError jsonError;
                QJsonDocument parseDocument = QJsonDocument::fromJson(m_strFileData.toUtf8(), & jsonError);
                m_strFileData = "";
                if(QJsonParseError::NoError == jsonError.error)
                {
                    if(parseDocument.isObject())
                    {
                        QJsonObject objectData = parseDocument.object();
                        QString strVersion = objectData.take(OTA_KEY_OTA_VERSION).toString();
                        if (strVersion.isEmpty())
                        {
                            emit sigOtaResult(true, OTAVersionInfo(), false);
                            return;
                        }
                        QJsonArray arrayData =  objectData.take(OTA_KEY_OTA_UMODULES).toArray();
                        bool bIsForced = objectData.take(OTA_KEY_ISFORCED).toBool(false);
                        OTAVersionInfo versionInfo;
                        versionInfo.isForced = bIsForced;
                        bool bHasNewVersion = true;
                        if (arrayData.size() <= 0)
                        {
                            bHasNewVersion = false;
                        }
                        for (int i = 0; i < arrayData.size(); i++)
                        {
                            QJsonObject objData = arrayData.at(i).toObject();
                            QString strModuleName = objData.take(OTA_KEY_MODULE_NAME).toString();
                            if (strModuleName.compare(OTA_MODULE_NAME_UCAR) == 0)
                            {
                                QString strUrl = objData.take(OTA_KEY_PACKAGE_URL).toString();
                                QString strPackegeMd5 = objData.take(OTA_KEY_PACKAGE_MD5).toString();
                                long long nFileSize = (long long)objData.take(OTA_KEY_PACKAGE_SIZE).toDouble();
                                QString strVersion = objData.take(OTA_KEY_VERSION_NAME).toString();
                                QString strReleaseNote= objData.take(OTA_KEY_RELEASE_NOTE).toString();
                                strcpy(versionInfo.ucarVersion, strVersion.toUtf8().data());
                                strcpy(versionInfo.releaseNote, strReleaseNote.toUtf8().data());
                                versionInfo.packageSize = nFileSize;
                                strcpy(versionInfo.packageUrl, strUrl.toUtf8().data());
                                strcpy(versionInfo.packageMd5, strPackegeMd5.toUtf8().data());
                                qDebug("QTA get info packageUrl = %s, packageMd5 = %s\n", strUrl.toUtf8().data(), strPackegeMd5.toUtf8().data());
                                qDebug("QTA get info packageSize = %lld, isForced = %d\n", nFileSize, bIsForced);
                            }
                            else if (strModuleName.compare(OTA_MODULE_NAME_AIBOX) == 0)
                            {
                                QString strVersion = objData.take(OTA_KEY_VERSION_NAME).toString();
                                strcpy(versionInfo.AiboxVersion, strVersion.toUtf8().data()); //aibox version
                            }
                        }
                        emit sigOtaResult(true, versionInfo, bHomePageCheck);
                    }
    //                [{
    //                    "moduleName": "web",
    //                    "versionName": "v1.0.0.2",
    //                    "isIncremental": false,
    //                    "packageUrl": "https://testqiniu.ubtrobot.com/upgrade/2020/06/1591356139051/ubt-teacher.rar",
    //                    "packageMd5": "99336947860306fdbf96f0eca3aabb60",
    //                    "packageSize": 26248209,
    //                    "isForced": false,
    //                    "releaseNote": "d",
    //                    "releaseTime": 1591358979
    //                }]

                }
            }
        }
    }
}

/************************************
* 名称: OnError
* 功能: 错误处理槽函数
* 参数: [in]QNetworkReply::NetworkError code请求错误代码
* 返回:   void
* 时间:   2020/03/24
* 作者:   Joker
************************************/
void HttpTransfer::OnError(QNetworkReply::NetworkError code)
{

    if(code == QNetworkReply::ConnectionRefusedError ||
            code == QNetworkReply::HostNotFoundError ||
            code == QNetworkReply::TimeoutError ||
            code == QNetworkReply::NetworkSessionFailedError)
    {
        //emit serverError();错误处理
    }
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if(reply)
    {

        qDebug()<<"HttpTransfer::OnError readall" <<reply->readAll();
        int nCode = reply->request().attribute( QNetworkRequest::HttpStatusCodeAttribute).toInt();
        qDebug()<<"HttpTransfer::OnError"<<nCode;
        QString requestType = reply->request().attribute(QNetworkRequest::Attribute(HTTPRequestType)).toString();
        int nRequestID = reply->request().attribute(QNetworkRequest::Attribute(HTTPRequestID)).toInt();
        bool bHomePageCheck = reply->request().attribute(QNetworkRequest::Attribute(HTTPOtaShowTips)).toBool();
        reply->deleteLater();
        if(requestType == HTTP_REQUEST_GET_OTA_VERSION)
        {
           emit sigOtaResult(false, OTAVersionInfo(), bHomePageCheck);
        }
    }
}

/************************************
* 名称: OnDownloadProgress
* 功能: 下载进度处理槽函数
* 参数: [in]bytesReceived 已经接收到的字节数
* 参数: [in]bytesTotal 总共字节数
* 返回:   void
* 时间:   2020/03/24
* 作者:   Joker
************************************/
void HttpTransfer::OnDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if(reply && bytesReceived != 0)
    {
        // 注意：这里会触发两次信号，
        // 第一次：bytesReceived和bytesTotal不相等
        // 第二次：bytesReceived和bytesTotal相等
        // 必须要等待接收数据和总数据相等的时候再读取数据
        // 否则第二次读取到的数据是0
        if (bytesReceived != bytesTotal)
        {
            return;
        }

        QByteArray data = reply->readAll();
        if (data.size() <= 0)
        {
            return;
        }

        QString requestType = reply->request().attribute(QNetworkRequest::Attribute(HTTPRequestType)).toString();
        int nHTTPRequestID = reply->request().attribute(QNetworkRequest::Attribute(HTTPRequestID)).toInt();
        qDebug("HttpTransfer::OnDownloadProgress requestType = %s\n", requestType.toUtf8().data());
        //ActionList 请求
//        if(requestType == HTTP_REQUEST_GET_FILE_LIST)
//        {
//            m_strFileData
//        }

    }
}


/************************************
* 名称: OnInit
* 功能: 初始化槽函数
* 返回:   void
* 时间:   2020/04/07
* 作者:   Joker
************************************/
void HttpTransfer::OnInit()
{
    m_pNetManager = new QNetworkAccessManager();
}

/**************************************************************************
* 函数名: readyRead
* 功能: 读取后台返回结果，因获取文件列表数据很大，会分批返回，故获取文件列表以OnFinished为准
* 参数: [@in]
* 参数: [@in]
* 返回值:void
* 时间:2021/03/24
* 作者: Joker
*/
void HttpTransfer::readyRead()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if(reply)
    {
        const QString sRequestType = reply->request().attribute(QNetworkRequest::Attribute(HTTPRequestType)).toByteArray();
        QByteArray arrData = reply->readAll();
        qDebug("HttpTransfer::readyRead sRequestType  =  %s\n", sRequestType.toUtf8().data());
        qDebug("HttpTransfer::readyRead arrData  =  %s\n", arrData.data());
        //        //上传动作的应答
        //        emit sigUploadActionResponse(sRequestType, nRequestID, nActionID, reply->readAll());=
        if (sRequestType == HTTP_REQUEST_GET_OTA_VERSION)
        {
            m_strFileData += arrData;
        }
    }
}
