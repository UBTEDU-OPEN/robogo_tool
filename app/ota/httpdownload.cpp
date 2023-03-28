/*
* Copyright (c) 2020, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：httpdownload.h
* 创建时间：2020/03/31
* 文件标识：
* 文件摘要：下载类
*
* 当前版本：1.0.0.0
* 作    者：Joker
* 完成时间：202021/03/24
* 版本摘要：
*/

#include "httpdownload.h"
//#include "ubxactionlibconstants.h"
//#include "UBXPublic.h"
#include <QFile>

HttpDownload::HttpDownload()
//    m_pOta(pFileSyn)
{
    m_pNetManager = NULL;
    moveToThread(&m_oThread);
    m_nRequestID = -1;
    connect(this, &HttpDownload::SigToPauseDownload, this, &HttpDownload::OnPauseDownload);
    connect(this, &HttpDownload::SigToResumeDownload, this, &HttpDownload::OnResumeDownload);
    connect(this, &HttpDownload::SigInit, this, &HttpDownload::OnInit);
    connect(this, &HttpDownload::SigDownloadOTAVersion, this, &HttpDownload::OnDownloadOTAVersion);
    m_oThread.start();
    emit SigInit();
}

HttpDownload::~HttpDownload()
{
    stopThread();
    if(m_pNetManager
            
            )
    {
        m_pNetManager->disconnect();
        delete m_pNetManager;  // 出现释放打印异常
    }
}

/************************************
* 名称: stopThread
* 功能: 停止线程
* 参数:
* 返回:
* 时间:   2021/03/22
* 作者:   Joker
************************************/
void HttpDownload::stopThread()
{
    m_oThread.quit();
    m_oThread.wait();
}


/************************************
* 名称: DownloadOTAVersion
* 功能: 发送下载动作请求
* 参数: [in]strUrl 下载地址
* 参数: [in]downLoadType 下载类型
* 返回:   int 返回请求ID，外部监听者可以据此判断是否是自己的请求
* 时间:   2020/03/31
* 作者:   Joker
************************************/
int HttpDownload::DownloadOTAVersion(const QString &strUrl, int downLoadType)
{
    ++m_nRequestID;

    emit SigDownloadOTAVersion(m_nRequestID, strUrl, downLoadType);
    return m_nRequestID;
}

/************************************
* 名称: OnError
* 功能: 网络错误槽函数
* 参数: [in]code 错误码
* 返回:   void
* 时间:   2020/03/31
* 作者:   Joker
************************************/
void HttpDownload::OnError(QNetworkReply::NetworkError code)
{
    if(code == QNetworkReply::ConnectionRefusedError ||
       code == QNetworkReply::HostNotFoundError ||
       code == QNetworkReply::TimeoutError ||
       code == QNetworkReply::NetworkSessionFailedError)
    {
        //特殊错误处理，暂时没有需求
    }

    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if(reply)
    {

//        int nDowmLoadType = reply->request().attribute(QNetworkRequest::Attribute(HTTPDownloadType)).toInt();

//        if (nFileID != 0)
//        {
//            if (m_pOta)
//            {
//                m_pOta->httpDownLoadResult(false, nChapterID, nFileID, nDowmLoadType);
//            }
//        }

//        if (nDowmLoadType == eHTTPDownloadWebOtaVersion || nDowmLoadType == eHTTPDownloadMainOtaVersion)
//        {
//            bool bReply = reply->request().attribute(QNetworkRequest::Attribute(HTTPOtaNeedReply)).toBool();
//            if (m_pOta)
//            {
//                m_pOta->httpOtaDownLoadResult(false, nDowmLoadType);
//            }
//        }
        //下载失败
//        //移除存储的本地路径
//        m_mapIteDada.remove(nActionID);
//        m_mapReply.remove(nActionID);
        emit SigFinishedDownload(false);
        reply->deleteLater();
    }
}

/************************************
* 名称: OnDownloadProgress
* 功能: 下载进度处理槽函数
* 参数: [in]bytesReceived 已经接收到的字节数
* 参数: [in]bytesTotal 总共的字节数
* 返回:   void
* 时间:   2020/03/31
* 作者:   Joker
************************************/
void HttpDownload::OnDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if(reply && bytesReceived != 0)
    {
//        int nRequestID = reply->request().attribute(QNetworkRequest::Attribute(HTTPRequestID)).toInt();
//        QString strUrl = reply->request().attribute(QNetworkRequest::Attribute(HTTPDownloadFilePath)).toString();

//        int nDowmLoadType = reply->request().attribute(QNetworkRequest::Attribute(HTTPDownloadType)).toInt();
//        int nChapterID = reply->request().attribute(QNetworkRequest::Attribute(HTTPChapterID)).toInt();
//        int nFileID = reply->request().attribute(QNetworkRequest::Attribute(HTTPFileID)).toInt();
//        if (nFileID != 0 || nChapterID != 0)
//        {
//            qDebug("HttpDownload::OnDownloadProgress nFileID =%d , nChapterID = %d", nFileID, nChapterID);
//        }
//        //通知完成下载
//        emit SigFinishedDownload(nRequestID, nActionID);

        quint64 nbytesAvailable = reply->bytesAvailable();
        if(nbytesAvailable > 0)
        {
            QString strUserFilePathName = OTA_DOWNLOAD_FILE_PATH;
            qDebug()<< "HttpDownload::OnDownloadProgress strUserFilePathName = "<<strUserFilePathName;
            QFile localFile(strUserFilePathName);

            //不存在则为刚开始下载
            if(!localFile.exists())
            {
//                emit SigStartDownload(nRequestID, m_mapIteDada[nActionID], bytesReceived, bytesTotal);
//                        if (m_pOta)
//                        {
//                            m_pOta->Show_Download_Process(bytesReceived, bytesTotal);
//                        }

            }
                qDebug("HttpDownload::OnDownloadProgress localFile is not exists strUserFilePathName = %s", strUserFilePathName.toUtf8().data());

            localFile.open(QIODevice::WriteOnly | QIODevice::Unbuffered | QIODevice::Append);
            if (!localFile.isOpen())
            {
                //如果文件无法写入，直接结束本次下载
                qDebug("HttpDownload::OnDownloadProgress localFile isOpen  abort strUserFilePathName = %s", strUserFilePathName.toUtf8().data());
                reply->abort();
//                    if (m_pOta)
//                    {
//                        m_pOta->httpDownLoadResult(false, nChapterID, nFileID, nDowmLoadType);
//                    }

//                    if (m_pOta)
//                    {
//                        m_pOta->httpOtaDownLoadResult(false, nDowmLoadType, bReply);
//                    }
                return;
            }

            quint64 dataSize = nbytesAvailable;
            quint64 nReadedBytes = 0;
            do
            {
                QByteArray data = reply->read(nbytesAvailable);

                nReadedBytes = dataSize = data.size();
                char *writeBuff = data.data();
                while (dataSize > 0)
                {
                    quint64 writedSize = localFile.write(writeBuff, dataSize);
                    dataSize -= writedSize;
                    writeBuff += writedSize;
                }
                localFile.flush();
                localFile.close();
            }while((nbytesAvailable -= nReadedBytes) > 0);

            emit SigDownloadProgress(bytesReceived, bytesTotal);
//            if (m_pOta)
//            {
//                m_pOta->Show_Download_Process(bytesReceived, bytesTotal);
//            }


            //已经存在则是断点下载，此时的received和total需要自己计算,因此最好不使用槽的参数，而使用自己记录过的值
//            emit SigDownloadProgress(nRequestID, nActionID, m_mapIteDada[nActionID].m_nHasDownloadedBytes, m_mapIteDada[nActionID].m_nActSize);

            //写入本地文件
//            if(bytesReceived == bytesTotal)
//            {
//                //如果是zip文件，则先解压
//                QString strFileSuffix = strFileName.mid(strFileName.lastIndexOf("."));
//                if(strFileSuffix.compare(".zip", Qt::CaseInsensitive) == 0)
//                {
//                    UnZipFile(strFileName);
//                }
//                g_actupdatemgr->AddDownLoadCount(nActionID);
//            }
        }
    }
}

/************************************
* 名称: OnPauseDownload
* 功能: 响应暂停下载槽函数
* 参数: [in]nActionID 指定要暂停的action ID
* 返回:   void
* 时间:   2021/03/22
* 作者:   Joker
************************************/
void HttpDownload::OnPauseDownload(int nActionID)
{
//    if(m_mapReply.contains(nActionID))
//    {
//        m_mapReply[nActionID]->abort();
//        m_mapReply.remove(nActionID);
//        emit SigHasPausedDownload(nActionID);
//    }
}

/************************************
* 名称: OnFinished
* 功能: 指令请求完成槽函数
* 返回:   void
* 时间:   2021/03/22
* 作者:   Joker
************************************/
void HttpDownload::OnFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if(reply)
    {
        int nRequestID = reply->request().attribute(QNetworkRequest::Attribute(HTTPRequestID)).toInt();
//        int nChapterID = reply->request().attribute(QNetworkRequest::Attribute(HTTPChapterID)).toInt();
//        int nFileID = reply->request().attribute(QNetworkRequest::Attribute(HTTPFileID)).toInt();
//        int nDowmLoadType = reply->request().attribute(QNetworkRequest::Attribute(HTTPDownloadType)).toInt();

        quint64 nbytesAvailable = reply->bytesAvailable();
        if(nbytesAvailable > 0)
        {
            qDebug("HttpDownload::OnFinished nbytesAvailable = %lld", nbytesAvailable);
        }
//        m_mapReply.remove(nActionID);
        reply->deleteLater();

//        //通知完成下载
        emit SigFinishedDownload(true);

//            if (m_pOta)
//            {
//                m_pOta->httpOtaDownLoadResult(true, nDowmLoadType, bReply);
//            }

    }
}

/************************************
* 名称: OnResumeDownload
* 功能: 重新下载
* 参数: [in]nRequestID 请求ID
* 参数: [in]itemData 下载项
* 返回:   void
* 时间:   2021/03/22
* 作者:   Joker
************************************/
void HttpDownload::OnResumeDownload(int nRequestID)
{
//    m_mapIteDada.insert(itemData.m_nItemID, itemData);
//    QNetworkRequest request;
//    request.setUrl(QUrl(itemData.m_sActDownloadURL));

//    QString strRange = "bytes " + QString::number(itemData.m_nHasDownloadedBytes) + "-";
//    request.setRawHeader("Range",strRange.toLatin1());

//    request.setAttribute(QNetworkRequest::Attribute(HTTPRequestID), (int)nRequestID);
//    request.setAttribute(QNetworkRequest::Attribute(HTTPFileID), itemData.m_nItemID);
//    QNetworkReply* reply = m_pNetManager->get(request);
//    connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
//            this, &HttpDownload::OnError);
//    connect(reply, &QNetworkReply::downloadProgress,
//            this, &HttpDownload::OnDownloadProgress);
//    connect(reply, &QNetworkReply::finished, this, &HttpDownload::OnFinished);
//    m_mapReply.insert(itemData.m_nItemID, reply);
}
/************************************
* 名称: SigDownloadOTAVersion
* 功能: 发送下载动作请求
* 参数: [in]nRequestID 请求id
* 参数: [in]strUrl 下载地址
* 参数: [in]downLoadType 下载类型
* 参数: [in]bReply 是否回复web
* 作者:   Joker
************************************/
void HttpDownload::OnDownloadOTAVersion(int nRequestID, const QString &strUrl, int downLoadType)
{
    QNetworkRequest request;
    request.setUrl(QUrl(strUrl));

    request.setAttribute(QNetworkRequest::Attribute(HTTPRequestID), (int)nRequestID);

    QNetworkReply* reply = m_pNetManager->get(request);
    connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
            this, &HttpDownload::OnError);
    connect(reply, &QNetworkReply::downloadProgress,
            this, &HttpDownload::OnDownloadProgress);
    connect(reply, &QNetworkReply::finished, this, &HttpDownload::OnFinished);
}

/************************************
* 名称: OnInit
* 功能: 初始化槽函数
* 返回:   void
* 时间:   2020/04/07
* 作者:   Joker
************************************/
void HttpDownload::OnInit()
{
    m_pNetManager = new QNetworkAccessManager();
}


