#ifndef OTACONST_H
#define OTACONST_H
#include <QString>

const QString OTA_PRODUCT_NAME_ROBOTGO = "RobotGo";
const QString OTA_GROUP_NAME_ROBOTGO_AIBOX = "RobotGo-Aibox";
const QString OTA_MODULE_NAME_UCAR = "ucar";
const QString OTA_MODULE_NAME_AIBOX = "aibox";
const QString OTA_DOWNLOAD_FILE_PATH = "/tmp/robotgo_ota.tar.gz";

const int OTA_UPDATE_TIMEOUT = 180; //3分钟超时 180s

const int OTA_UPDATE_TIMEOUT_CALC = 5000; //5s检测一次

#define STRUCT_INIT(T)\
T()\
{\
    memset(this, 0, sizeof(T));\
}\
T(const T& obj)\
{\
    if(this != &obj)\
        memcpy(this, &obj, sizeof(T));\
}\
T& operator = (const T& obj)\
{\
    if(this != &obj)\
        memcpy(this, &obj, sizeof(T));\
    return *this;\
}


enum HTTPAttributeCode
{
    HTTPRequestID = QNetworkRequest::User + 1,//请求ID
    HTTPRequestType,       //请求接口类型
    HTTPDownloadType,      //下载类型
    HTTPOtaShowTips        //OTA升级版本提示
};
const char* const HTTP_REQUEST_GET_OTA_VERSION = "HttpRequestOtaVersion";//请求OTA版本

const int HTTP_RESULT_SUCESS = -2;
const int HTTP_RESULT_FAIL = -1;

const int HTTP_DEFAULT_REQUEST_ID = -1;  // 请求默认ID

const int POPPU_WIDGET_DELAY_TIME = 2000; // 弹窗延时时间
const int DELAY_TIME_100 = 100; // 延时时间100ms

#define OTA_KEY_MODULE_NAME            "moduleName"    //模块名
#define OTA_KEY_VERSION_NAME           "versionName"   //版本号
#define OTA_KEY_PACKAGE_URL            "packageUrl"    //版本下载url
#define OTA_KEY_PACKAGE_MD5            "packageMd5"    //文件Md5
#define OTA_KEY_PACKAGE_SIZE           "packageSize"   //文件大小
#define OTA_KEY_ISFORCED               "forced"      //是否强制升级
#define OTA_KEY_RELEASE_NOTE           "releaseNote"
#define OTA_KEY_HAS_NEW_VERSION        "hasNewVersion" //有新版本需要更新
#define OTA_KEY_ACTION_TYPE            "actionType" //有新版本需要更新
#define OTA_KEY_OTA_DOWNLOAD_STATE     "state" //OTA下载状态

//group相关
#define OTA_KEY_OTA_VERSION     "version"   //OTA group版本
#define OTA_KEY_OTA_UPDATE_LOG  "updateLog" //升级log
#define OTA_KEY_OTA_UMODULES    "modules"   //升级log
#define OTA_VALUE_MODULE_UCAR   "ucar"      //ucar模块
#define OTA_VALUE_MODULE_AIBOX  "aibox"     //aibox模块

enum emOtaStatus
{
    eOtaStatusUnknown = 0, //OTA未知状态
    eOtaStatusIdle, //OTAidle状态
    eOtaStatusDownLoading, //OTA下载中状态
};


#endif // OTACONST_H
