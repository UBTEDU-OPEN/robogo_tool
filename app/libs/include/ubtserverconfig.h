/*
* Copyright (c) 2020, 深圳市优必选科技有限公司
* All rights reserved.
*
* 文件名称：ubtserverconfig.h
* 创建时间：2020/12/13
* 文件标识：
* 文件摘要：服务配置
*
* 当前版本：1.0.0.0
* 作    者：Joker
* 完成时间：2020/12/13
* 版本摘要：
*/
#ifndef UBTSERVERCONFIG_H
#define UBTSERVERCONFIG_H

#include <iostream>
#include <vector>
//#include "ubtserviceconfig.h"


class  UBTServerConfig
{
public:
    UBTServerConfig();
//    /**************************************************************************
//    * 函数名: getEventTrackingServerUrl
//    * 功能: 获取数据埋点服务器地址
//    * 参数: void
//    * 返回值:
//    * 时间: 2020/03/17
//    * 作者: Leo
//    */
//    QString getEventTrackingServerUrl();

    std::string getHeaderUBTSign();
    std::string getLearnLessonSource();
    std::string generateRandSignSeed();
    std::string getHeaderXUBTSignV3(const std::string& strCurrentTimer, const std::string& getAppKey, const std::string& strXUBTDeviceId);
    std::vector<std::string> getFileListHttpHeader(const std::string &strDeviceId);
    std::vector<std::string> getNewCommonHttpHeader(const std::string &strXUBTSign, const std::string &strXUBTSource);

    std::string getImInfoServerUrl();
    std::string getImInfoChannel();
    std::string getAppId();
    std::string getDeviceID();
    std::string getAiboxVersion();
    std::string getLearnLessonLanguage();
private:
    std::string getFileServerUrl();
    std::string getLearnLessonAppKey();


};

#endif // UBTSERVERCONFIG_H
