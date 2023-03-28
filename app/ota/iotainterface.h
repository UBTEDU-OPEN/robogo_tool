/*
* Copyright (c) 2020, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：ifilecynchronize.h
* 创建时间：2021/03/24
* 文件标识：
* 文件摘要：文件数据操作类接口
*
* 当前版本：1.0.0.0
* 作    者：Joker
* 完成时间：2021/03/24
* 版本摘要：
*/
#ifndef IFILESYNCHRONIZE_H
#define IFILESYNCHRONIZE_H
#include <QString>
#include <QObjectUserData>

class  IOtaEvent
{
public:

    /**************************************************************************
    * 函数名: IOtaEvent_Show_Download_Process
    * 功能:展示下载进度
    * 参数: [@in] nReadSize 已下载大小
    * 参数: [@in] nTotalSize 总大小
    * 返回值:void
    * 时间:2021/03/24
    * 作者: Joker
    */
    virtual void IOtaEvent_Show_Download_Process(long long nReadSize, long long nTotalSize) = 0;


    /**************************************************************************
    * 函数名: IOtaEvent_Ota_Download_Status
    * 功能:  上报web app OTA升级包下载状态
    * 参数: [@in] bSuc 是否下载成功
    * 参数: [@in] strVersionName 版本号
    * 返回值:bool
    * 时间:2021/03/24
    * 作者: Joker
    */
    virtual bool IOtaEvent_Ota_Download_Status(bool bSuc, const QString &strVersionName) = 0;

};

class  IOtaInterface
{
public:
    virtual bool connect(IOtaEvent* pNotify) = 0;
    virtual void disconnect() = 0;


    /**************************************************************************
    * 函数名: httpOtaDownLoadResult
    * 功能: OTA文件http下载结果
    * 参数: [@in] bSuccess 下载是否成功
    * 参数: [@in] nDowmLoadType  下载类型
    * 返回值:void
    * 时间:2021/03/24
    * 作者: Joker
    */
    virtual void httpOtaDownLoadResult(bool bSuccess, int nDowmLoadType) = 0;


    /**************************************************************************
    * 函数名: OnGetOtaNewVersion
    * 功能: 获取到OTA新版本
    * 参数: [@in] bHasNewVersion 是否有新版本
    * 参数: [@in] otaInfo OTA信息
    * 返回值:void
    * 时间:2021/03/24
    * 作者: Joker
    */
    virtual void OnGetOtaNewVersion(bool bHasNewVersion, const OTAVersionInfo &otaInfo) = 0;

    /**************************************************************************
    * 函数名: QueryOTAVersion
    * 功能:查询OTA版本
    * 参数: [@in] strModuleName 模块名
    * 参数: [@in] stVersion     版本名
    * 参数: [@in] strLanguage   语言名
    * 返回值:void
    * 时间:2021/03/24
    * 作者: Joker
    */
    virtual  int QueryOTAVersion(const QString& strModuleName, const QString & stVersion, const QString strLanguage) = 0;

    /**************************************************************************
    * 函数名: SearchOtaVersion
    * 功能: 查询OTA版本
    * 参数: [@in]
    * 返回值:void
    * 时间:2021/03/24
    * 作者: Joker
    */
    virtual void SearchOtaVersion() = 0;

    /**************************************************************************
    * 函数名: Show_Download_Process
    * 功能:展示下载进度
    * 参数: [@in] nReadSize 已下载大小
    * 参数: [@in] nTotalSize 总大小
    * 返回值:void
    * 时间:2021/03/24
    * 作者: Joker
    */
    virtual void Show_Download_Process(long long nReadSize, long long nTotalSize) = 0;

};

#endif // IFILESYNCHRONIZE_H
