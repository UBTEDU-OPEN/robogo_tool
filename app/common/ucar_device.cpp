#include "ucar_device.h"
#include "infer_contact_client.h"
#include "grpc++/grpc++.h"
#include <QLoggingCategory>
#include <assert.h>
#include <chrono>
#include <string>
#include <QMap>
#include <QByteArray>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>


Q_LOGGING_CATEGORY(ucar_device, "ucar_device");

namespace {
QString SERVER_IP{"127.0.0.1"};
uint32_t SERVER_PORT{34699};
QString SERVER_IP_PORT{"localhost:34699"};
}

using namespace std;

UCarDevice* UCarDevice::m_instance = nullptr;

UCarDevice::UCarDevice(const QString ip, const uint32_t port, QObject *parent)
    : QThread(parent),
      m_ip(ip),
      m_port(port),
      m_require_distribute(false),
      m_grpc_inst(new InferContactClient(grpc::CreateChannel(
        SERVER_IP_PORT.toStdString(), grpc::InsecureChannelCredentials())))
{
    qCInfo(ucar_device()) << __func__ << "ip->" << m_ip << " port->" << m_port;
    init();

    // 在其他线程中无法再次调用初始化好的m_py_instance
    connect(this, &UCarDevice::requestImuData, this, &UCarDevice::onRequestImuData, Qt::DirectConnection);
}

UCarDevice::~UCarDevice()
{
    unload();
}

UCarDevice& UCarDevice::instance()
{
    if(!m_instance)
    {
        m_instance = new UCarDevice(SERVER_IP, SERVER_PORT);
    }

    return *m_instance;
}

bool UCarDevice::init()
{
    qCInfo(ucar_device()) << __func__ ;
    bool ret = false;

    QJsonObject object;
    object.insert("module","oneai.robogo.ucar_device.ucar_device_inference");
    object.insert("func","UcarDeviceInference");
    object.insert("model", "ucar_device");
    object.insert("module_lock", "true");
    QJsonDocument document;
    document.setObject(object);
    QByteArray array = document.toJson(QJsonDocument::Compact);
    std::string ret_json =  m_grpc_inst->init(array.toStdString());
    qCDebug(ucar_device()) << "grpc return : " << ret_json.c_str();

    QJsonParseError jsonError;
    document = QJsonDocument::fromJson(QByteArray::fromStdString(ret_json), &jsonError); //转化为JSON文档
    if( !document.isNull() && (jsonError.error == QJsonParseError::NoError)) //解析未发生错误
    {
        if(document.isObject())
        {
            QJsonObject object = document.object();

            if(object.contains("return"))
            {
                QJsonValue value = object.value("return");
                if(value.isBool())
                {
                    ret = value.toBool();
                }
            }
        }
    }

    if (!ret)
    {
        return ret;
    }

    ret = grpcCallWithoutArgs("load");

    return ret;
}

bool UCarDevice::unload()
{
    qCInfo(ucar_device()) << __func__ ;
    bool ret = false;

    QJsonObject object;
    object.insert("func", "unload");
    object.insert("model", "ucar_device");
    QJsonDocument document;
    document.setObject(object);
    QByteArray array = document.toJson(QJsonDocument::Compact);
    std::string ret_json =  m_grpc_inst->unload(array.toStdString());
    qCDebug(ucar_device()) << "grpc return : " << ret_json.c_str();

    QJsonParseError jsonError;
    document = QJsonDocument::fromJson(QByteArray::fromStdString(ret_json), &jsonError); //转化为JSON文档
    if( !document.isNull() && (jsonError.error == QJsonParseError::NoError)) //解析未发生错误
    {
        if(document.isObject())
        {
            QJsonObject object = document.object();

            if(object.contains("return"))
            {
                QJsonValue value = object.value("return");
                if(value.isBool())
                {
                    ret = value.toBool();
                }
            }
        }
    }

    return ret;
}

void UCarDevice::enableDistributing(bool enable)
{
    qCInfo(ucar_device()) << __func__  <<  "enable -> " << enable;
    if (enable)
    {
        if (m_thread)
        {
            qCInfo(ucar_device()) << __func__  <<  " Stop running thread";
            m_require_distribute = false;
            m_thread->join();
            qCInfo(ucar_device()) << __func__  <<  " Stop running thread done";
        }
        m_require_distribute = true;
        m_thread = std::move(make_unique<std::thread>(UCarDevice::distributingThread, this));
    }
    else
    {
        if (m_thread)
        {
            m_require_distribute = false;
            m_thread->join();
            m_thread = nullptr;
        }
    }
}

void UCarDevice::distributingThread(UCarDevice *ins)
{
    qCInfo(ucar_device()) << __func__ << " start";
    assert(ins);
    while (ins->m_require_distribute)
    {
        qCDebug(ucar_device()) << __func__;
        emit ins->requestImuData();
        this_thread::sleep_for(chrono::milliseconds(100));
    }
    qCInfo(ucar_device()) << __func__ << " stop";
}

void UCarDevice::getImuData(std::vector<float> &data)
 {
    qCDebug(ucar_device()) << __func__ ;

    QJsonObject object;
    object.insert("func","get_imu_data");
    object.insert("model", "ucar_device");
    QJsonDocument document;
    document.setObject(object);
    QByteArray array = document.toJson(QJsonDocument::Compact);
    std::string ret_json =  m_grpc_inst->execute(array.toStdString());
//    qCDebug(ucar_device()) << "grpc return : " << ret_json.c_str();

    QJsonParseError jsonError;
    document = QJsonDocument::fromJson(QByteArray::fromStdString(ret_json), &jsonError); //转化为JSON文档
    if( !document.isNull() && (jsonError.error == QJsonParseError::NoError)) //解析未发生错误
    {
        if(document.isObject())
        {
            QJsonObject object = document.object();

            if(object.contains("return") && object.value("return").toBool())
            {
                if(object.contains("result"))
                {
                    auto jarray = object.value("result").toArray();
                    foreach(const QVariant & value, jarray)
                    {
                        data.push_back(value.toFloat());
                    }
                }
            }
        }
    }

    return ;
}

void UCarDevice::run()
{
    qCDebug(ucar_device()) << __func__ ;

    exec();
}

void UCarDevice::onRequestImuData()
{
    qCDebug(ucar_device()) << __func__ ;

    vector<float> data;
    getImuData(data);

    qCDebug(ucar_device()) << __func__ << " data size:" << data.size()
                          << " data: " << data;
    if (data.size() >= 12)
    {
        emit accelData(data[0], data[1], data[2]);
        emit magneticData(data[6], data[7], data[8]);
        emit angleData(data[9], data[10], data[11]);
    }
}

bool UCarDevice::getOffsets(int32_t &x, int32_t &y, int32_t &z, const uint8_t start_index)
{
    qCInfo(ucar_device()) << __func__ ;

    QJsonObject object;
    object.insert("func","get_imu_adjust_data");
    object.insert("model", "ucar_device");
    QJsonDocument document;
    document.setObject(object);
    QByteArray array = document.toJson(QJsonDocument::Compact);
    std::string ret_json =  m_grpc_inst->execute(array.toStdString());
    qCDebug(ucar_device()) << "grpc return : " << ret_json.c_str();

    QJsonParseError jsonError;
    document = QJsonDocument::fromJson(QByteArray::fromStdString(ret_json), &jsonError); //转化为JSON文档
    if( !document.isNull() && (jsonError.error == QJsonParseError::NoError)) //解析未发生错误
    {
        if(document.isObject())
        {
            QJsonObject object = document.object();

            if(object.contains("return") && object.value("return").toBool())
            {
                if(object.contains("result"))
                {
                    int16_t byte_h = 0, byte_l = 0;
                    auto jarray = object.value("result").toArray();

                    auto n = jarray.size();
                    if (n == 0)
                    {
                        qCInfo(ucar_device()) << "Emptry list returned";
                        return false;
                    }
                    else if (n < 18)
                    {
                        qCInfo(ucar_device()) << "Not enough items in list";
                        return false;
                    }

                    auto item = jarray[start_index + 0];
                    byte_l = static_cast<int16_t>(item.toInt());
                    item = jarray[start_index + 1];
                    byte_h = static_cast<int16_t>(item.toInt());
                    x = (byte_h << 8) | byte_l;

                    item = jarray[start_index + 2];
                    byte_l = static_cast<int16_t>(item.toInt());
                    item = jarray[start_index + 3];
                    byte_h = static_cast<int16_t>(item.toInt());
                    y = (byte_h << 8) | byte_l;

                    item = jarray[start_index + 4];
                    byte_l = static_cast<int16_t>(item.toInt());
                    item = jarray[start_index + 5];
                    byte_h = static_cast<int16_t>(item.toInt());
                    z = (byte_h << 8) | byte_l;

                    return true;
                }
            }
        }
    }

    return false;
}

bool UCarDevice::getMagneticOffsets(int32_t &x, int32_t &y, int32_t &z)
{
    qCInfo(ucar_device()) << __func__ ;
    return getOffsets(x, y, z, 12);
}

bool UCarDevice::getAccelOffsets(int32_t &x, int32_t &y, int32_t &z)
{
    qCInfo(ucar_device()) << __func__ ;
    return getOffsets(x, y, z, 0);
}

bool UCarDevice::startAdjusting(const AdjustingMode mode)
{
    qCInfo(ucar_device()) << __func__ << "mode -> " << uint8_t(mode);

    bool ret = false;

    QJsonObject object;
    object.insert("func","enter_adjust_imu");
    object.insert("model", "ucar_device");
    object.insert("imu_adjust_model", uint8_t(mode));
    QJsonDocument document;
    document.setObject(object);
    QByteArray array = document.toJson(QJsonDocument::Compact);
    std::string ret_json =  m_grpc_inst->execute(array.toStdString());
    qCDebug(ucar_device()) << "grpc return : " << ret_json.c_str();

    QJsonParseError jsonError;
    document = QJsonDocument::fromJson(QByteArray::fromStdString(ret_json), &jsonError); //转化为JSON文档
    if( !document.isNull() && (jsonError.error == QJsonParseError::NoError)) //解析未发生错误
    {
        if(document.isObject())
        {
            QJsonObject object = document.object();

            if(object.contains("return"))
            {
                QJsonValue value = object.value("return");
                if(value.isBool())
                {
                    ret = value.toBool();
                }
            }
        }
    }

    return ret;
}

bool UCarDevice::startAccelAdjusting()
{
    qCInfo(ucar_device()) << __func__ ;
    return startAdjusting(AccelMode);
}

bool UCarDevice::startMagneticAdjusting()
{
    qCInfo(ucar_device()) << __func__ ;
    return startAdjusting(MagneticMode);
}

bool UCarDevice::grpcCallWithoutArgs(const std::string &func)
{
    qCInfo(ucar_device()) << __func__ ;

    bool ret = false;

    QJsonObject object;
    object.insert("func", func.c_str());
    object.insert("model", "ucar_device");
    QJsonDocument document;
    document.setObject(object);
    QByteArray array = document.toJson(QJsonDocument::Compact);
    std::string ret_json =  m_grpc_inst->execute(array.toStdString());
    qCDebug(ucar_device()) << "grpc return : " << ret_json.c_str();

    QJsonParseError jsonError;
    document = QJsonDocument::fromJson(QByteArray::fromStdString(ret_json), &jsonError); //转化为JSON文档
    if( !document.isNull() && (jsonError.error == QJsonParseError::NoError)) //解析未发生错误
    {
        if(document.isObject())
        {
            QJsonObject object = document.object();

            if(object.contains("return"))
            {
                QJsonValue value = object.value("return");
                if(value.isBool())
                {
                    ret = value.toBool();
                }
            }
        }
    }

    return ret;
}

bool UCarDevice::exitAdjusting()
{
    qCInfo(ucar_device()) << __func__ ;

    return grpcCallWithoutArgs("save_adjust_imu");
}

bool UCarDevice::reset()
{
    qCInfo(ucar_device()) << __func__ ;

    return grpcCallWithoutArgs("imu_reset");
}

bool UCarDevice::enterUpgrading()
{
    qCInfo(ucar_device()) << __func__ ;

    return grpcCallWithoutArgs("enter_upgrading_mode");
}

bool UCarDevice::startUpgrading(const uint16_t package_amount)
{
    qCInfo(ucar_device()) << __func__ << " package amount : " << package_amount;

    bool ret = false;

    QJsonObject object;
    object.insert("func", "start_upgrading");
    object.insert("model", "ucar_device");
    object.insert("package_amount", package_amount);
    QJsonDocument document;
    document.setObject(object);
    QByteArray array = document.toJson(QJsonDocument::Compact);
    std::string ret_json =  m_grpc_inst->execute(array.toStdString());
    qCDebug(ucar_device()) << "grpc return : " << ret_json.c_str();

    QJsonParseError jsonError;
    document = QJsonDocument::fromJson(QByteArray::fromStdString(ret_json), &jsonError); //转化为JSON文档
    if( !document.isNull() && (jsonError.error == QJsonParseError::NoError)) //解析未发生错误
    {
        if(document.isObject())
        {
            QJsonObject object = document.object();

            if(object.contains("return"))
            {
                QJsonValue value = object.value("return");
                if(value.isBool())
                {
                    ret = value.toBool();
                }
            }
        }
    }

    return ret;
}

bool UCarDevice::sendUpgradingData(const uint16_t package_num, const std::vector<uint8_t> &package_data)
{
    auto data_size = package_data.size();
    qCInfo(ucar_device()) << __func__ << " package num: " << package_num << " package size: " << data_size;

    QJsonArray data_array;

    for (uint8_t data : package_data)
    {
        data_array.append(data);
    }

    bool ret = false;

    QJsonObject object;
    object.insert("func", "send_upgrading_data");
    object.insert("model", "ucar_device");
    object.insert("package_num", package_num);
    object.insert("package_data", data_array);
    QJsonDocument document;
    document.setObject(object);
    QByteArray array = document.toJson(QJsonDocument::Compact);
    std::string ret_json =  m_grpc_inst->execute(array.toStdString());
    qCDebug(ucar_device()) << "grpc return : " << ret_json.c_str();

    QJsonParseError jsonError;
    document = QJsonDocument::fromJson(QByteArray::fromStdString(ret_json), &jsonError); //转化为JSON文档
    if( !document.isNull() && (jsonError.error == QJsonParseError::NoError)) //解析未发生错误
    {
        if(document.isObject())
        {
            QJsonObject object = document.object();

            if(object.contains("return"))
            {
                QJsonValue value = object.value("return");
                if(value.isBool())
                {
                    ret = value.toBool();
                }
            }
        }
    }

    return ret;
}

bool UCarDevice::finishUpgrading(const QString &md5)
{
    qCInfo(ucar_device()) << __func__ << " md5: " << md5;

    if (md5.size() != 32)
    {
        qCCritical(ucar_device()) << "invalid md5 length: " << md5;
        return false;
    }

    bool ret = false;

    QJsonObject object;
    object.insert("func", "finish_upgrading");
    object.insert("model", "ucar_device");
    object.insert("md5", md5.toStdString().c_str());
    QJsonDocument document;
    document.setObject(object);
    QByteArray array = document.toJson(QJsonDocument::Compact);
    std::string ret_json =  m_grpc_inst->execute(array.toStdString());
    qCDebug(ucar_device()) << "grpc return : " << ret_json.c_str();

    QJsonParseError jsonError;
    document = QJsonDocument::fromJson(QByteArray::fromStdString(ret_json), &jsonError); //转化为JSON文档
    if( !document.isNull() && (jsonError.error == QJsonParseError::NoError)) //解析未发生错误
    {
        if(document.isObject())
        {
            QJsonObject object = document.object();

            if(object.contains("return"))
            {
                QJsonValue value = object.value("return");
                if(value.isBool())
                {
                    ret = value.toBool();
                }
            }
        }
    }

    return ret;
}

bool UCarDevice::cancelUpgrading()
{
    qCInfo(ucar_device()) << __func__ ;

    return grpcCallWithoutArgs("cancel_upgrading");
}

bool UCarDevice::getSoftwareVersion(QString &version)
{
    qCInfo(ucar_device()) << __func__ ;

    bool ret = false;

    QJsonObject object;
    object.insert("func", "get_version_info");
    object.insert("model", "ucar_device");
    QJsonDocument document;
    document.setObject(object);
    QByteArray array = document.toJson(QJsonDocument::Compact);
    std::string ret_json =  m_grpc_inst->execute(array.toStdString());
    qCDebug(ucar_device()) << "grpc return : " << ret_json.c_str();

    QJsonParseError jsonError;
    document = QJsonDocument::fromJson(QByteArray::fromStdString(ret_json), &jsonError); //转化为JSON文档
    if( !document.isNull() && (jsonError.error == QJsonParseError::NoError)) //解析未发生错误
    {
        if(document.isObject())
        {
            QJsonObject object = document.object();

            if(object.contains("return"))
            {
                QJsonValue value = object.value("return");
                if(value.isBool())
                {
                    ret = value.toBool();
                }
            }

            if(object.contains("result"))
            {
                QJsonValue value = object.value("result");
                if(value.isString())
                {
                    version = value.toString();
                }
            }
        }
    }

    return ret;
}

bool UCarDevice::setPerspective(const QList<uint16_t> &value)
{
    qCInfo(ucar_device()) << __func__ ;
    bool ret = false;

    QJsonObject object;
    object.insert("module","oneai.robogo.ucar_recognition.ucar_recognition_inference");
    object.insert("func","UcarRecognitionInference");
    object.insert("model", "ucar_recognition");
    object.insert("module_lock", "true");
    QJsonDocument document;
    document.setObject(object);
    QByteArray array = document.toJson(QJsonDocument::Compact);
    std::string ret_json =  m_grpc_inst->init(array.toStdString());
    qCDebug(ucar_device()) << "grpc return : " << ret_json.c_str();

    QJsonParseError jsonError;
    document = QJsonDocument::fromJson(QByteArray::fromStdString(ret_json), &jsonError); //转化为JSON文档
    if( !document.isNull() && (jsonError.error == QJsonParseError::NoError)) //解析未发生错误
    {
        if(document.isObject())
        {
            QJsonObject object = document.object();

            if(object.contains("return"))
            {
                QJsonValue value = object.value("return");
                if(value.isBool())
                {
                    ret = value.toBool();
                }
            }
        }
    }

    if (!ret)
    {
        return ret;
    }

    QString data_string;

    for (uint16_t data : value)
    {
        data_string.append(" " + QString::number(data));
    }

    object.empty();
    object.insert("func", "update_perspective_calibration");
    object.insert("model", "ucar_recognition");
    object.insert("perspective_calibration", data_string.toStdString().c_str());
    document.setObject(object);
    array = document.toJson(QJsonDocument::Compact);
    ret_json =  m_grpc_inst->execute(array.toStdString());
    qCDebug(ucar_device()) << "grpc return : " << ret_json.c_str();

    document = QJsonDocument::fromJson(QByteArray::fromStdString(ret_json), &jsonError); //转化为JSON文档
    if( !document.isNull() && (jsonError.error == QJsonParseError::NoError)) //解析未发生错误
    {
        if(document.isObject())
        {
            QJsonObject object = document.object();

            if(object.contains("return"))
            {
                QJsonValue value = object.value("return");
                if(value.isBool())
                {
                    ret = value.toBool();
                }
            }
        }
    }

    return ret;
}

bool UCarDevice::driveForward(const int nVelocity)
{
    qCInfo(ucar_device()) << __func__ ;

    bool ret = false;

    QJsonObject object;
    object.insert("func", "drive_forward");
    object.insert("model", "ucar_device");
    object.insert("extend_device_control_velocity_key", nVelocity);
    QJsonDocument document;
    document.setObject(object);
    QByteArray array = document.toJson(QJsonDocument::Compact);
    std::string ret_json =  m_grpc_inst->execute(array.toStdString());
    qCDebug(ucar_device()) << "grpc return : " << ret_json.c_str();

    QJsonParseError jsonError;
    document = QJsonDocument::fromJson(QByteArray::fromStdString(ret_json), &jsonError); //转化为JSON文档
    if( !document.isNull() && (jsonError.error == QJsonParseError::NoError)) //解析未发生错误
    {
        if(document.isObject())
        {
            QJsonObject object = document.object();

            if(object.contains("return"))
            {
                QJsonValue value = object.value("return");
                if(value.isBool())
                {
                    ret = value.toBool();
                }
            }
        }
    }

    return ret;
}

bool UCarDevice::driveStop()
{
    qCInfo(ucar_device()) << __func__ ;

    bool ret = false;

    QJsonObject object;
    object.insert("func", "drive_stop");
    object.insert("model", "ucar_device");
    QJsonDocument document;
    document.setObject(object);
    QByteArray array = document.toJson(QJsonDocument::Compact);
    std::string ret_json =  m_grpc_inst->execute(array.toStdString());
    qCDebug(ucar_device()) << "grpc return : " << ret_json.c_str();

    QJsonParseError jsonError;
    document = QJsonDocument::fromJson(QByteArray::fromStdString(ret_json), &jsonError); //转化为JSON文档
    if( !document.isNull() && (jsonError.error == QJsonParseError::NoError)) //解析未发生错误
    {
        if(document.isObject())
        {
            QJsonObject object = document.object();

            if(object.contains("return"))
            {
                QJsonValue value = object.value("return");
                if(value.isBool())
                {
                    ret = value.toBool();
                }
            }
        }
    }

    return ret;
}

bool UCarDevice::driveLeftDirection(const int nAngle)
{
    qCInfo(ucar_device()) << __func__ ;

    bool ret = false;

    QJsonObject object;
    object.insert("func", "drive_left_direction");
    object.insert("model", "ucar_device");
    object.insert("extend_device_direction_angle_key", nAngle);
    QJsonDocument document;
    document.setObject(object);
    QByteArray array = document.toJson(QJsonDocument::Compact);
    std::string ret_json =  m_grpc_inst->execute(array.toStdString());
    qCDebug(ucar_device()) << "grpc return : " << ret_json.c_str();

    QJsonParseError jsonError;
    document = QJsonDocument::fromJson(QByteArray::fromStdString(ret_json), &jsonError); //转化为JSON文档
    if( !document.isNull() && (jsonError.error == QJsonParseError::NoError)) //解析未发生错误
    {
        if(document.isObject())
        {
            QJsonObject object = document.object();

            if(object.contains("return"))
            {
                QJsonValue value = object.value("return");
                if(value.isBool())
                {
                    ret = value.toBool();
                }
            }
        }
    }

    return ret;
}

bool UCarDevice::driveRightDirection(const int nAngle)
{
    qCInfo(ucar_device()) << __func__ ;

    bool ret = false;

    QJsonObject object;
    object.insert("func", "drive_right_direction");
    object.insert("model", "ucar_device");
    object.insert("extend_device_direction_angle_key", nAngle);
    QJsonDocument document;
    document.setObject(object);
    QByteArray array = document.toJson(QJsonDocument::Compact);
    std::string ret_json =  m_grpc_inst->execute(array.toStdString());
    qCDebug(ucar_device()) << "grpc return : " << ret_json.c_str();

    QJsonParseError jsonError;
    document = QJsonDocument::fromJson(QByteArray::fromStdString(ret_json), &jsonError); //转化为JSON文档
    if( !document.isNull() && (jsonError.error == QJsonParseError::NoError)) //解析未发生错误
    {
        if(document.isObject())
        {
            QJsonObject object = document.object();

            if(object.contains("return"))
            {
                QJsonValue value = object.value("return");
                if(value.isBool())
                {
                    ret = value.toBool();
                }
            }
        }
    }

    return ret;
}

bool UCarDevice::servoComebackCenter()
{
    qCInfo(ucar_device()) << __func__ ;

    bool ret = false;

    QJsonObject object;
    object.insert("func", "servo_comeback_center");
    object.insert("model", "ucar_device");
    QJsonDocument document;
    document.setObject(object);
    QByteArray array = document.toJson(QJsonDocument::Compact);
    std::string ret_json =  m_grpc_inst->execute(array.toStdString());
    qCDebug(ucar_device()) << "grpc return : " << ret_json.c_str();

    QJsonParseError jsonError;
    document = QJsonDocument::fromJson(QByteArray::fromStdString(ret_json), &jsonError); //转化为JSON文档
    if( !document.isNull() && (jsonError.error == QJsonParseError::NoError)) //解析未发生错误
    {
        if(document.isObject())
        {
            QJsonObject object = document.object();

            if(object.contains("return"))
            {
                QJsonValue value = object.value("return");
                if(value.isBool())
                {
                    ret = value.toBool();
                }
            }
        }
    }

    return ret;
}

bool UCarDevice::setServoAdjust(const int nDir, const int nAdjust)
{
    qCInfo(ucar_device()) << __func__ ;

    bool ret = false;

    QJsonObject object;
    object.insert("func", "set_servo_adjust");
    object.insert("model", "ucar_device");
    object.insert("direction", nDir);
    object.insert("extend_device_direction_angle_key", nAdjust);
    QJsonDocument document;
    document.setObject(object);
    QByteArray array = document.toJson(QJsonDocument::Compact);
    std::string ret_json =  m_grpc_inst->execute(array.toStdString());
    qCDebug(ucar_device()) << "grpc return : " << ret_json.c_str();

    QJsonParseError jsonError;
    document = QJsonDocument::fromJson(QByteArray::fromStdString(ret_json), &jsonError); //转化为JSON文档
    if( !document.isNull() && (jsonError.error == QJsonParseError::NoError)) //解析未发生错误
    {
        if(document.isObject())
        {
            QJsonObject object = document.object();

            if(object.contains("return"))
            {
                QJsonValue value = object.value("return");
                if(value.isBool())
                {
                    ret = value.toBool();
                }
            }
        }
    }

    return ret;
}

bool UCarDevice::getServoAdjust(int nDir, int &nAdjust)
{
    qCInfo(ucar_device()) << __func__ ;

    bool ret = false;

    QJsonObject object;
    object.insert("func", "get_servo_adjust");
    object.insert("model", "ucar_device");
    object.insert("direction", nDir);
    QJsonDocument document;
    document.setObject(object);
    QByteArray array = document.toJson(QJsonDocument::Compact);
    std::string ret_json =  m_grpc_inst->execute(array.toStdString());
    qCDebug(ucar_device()) << "grpc return : " << ret_json.c_str();

    QJsonParseError jsonError;
    document = QJsonDocument::fromJson(QByteArray::fromStdString(ret_json), &jsonError); //转化为JSON文档
    if( !document.isNull() && (jsonError.error == QJsonParseError::NoError)) //解析未发生错误
    {
        if(document.isObject())
        {
            QJsonObject object = document.object();

            if(object.contains("return"))
            {
                QJsonValue value = object.value("return");
                if(value.isBool())
                {
                    ret = value.toBool();
                }
            }

            if(object.contains("extend_device_direction_angle_key"))
            {
                QJsonValue value = object.value("extend_device_direction_angle_key");
                if(value.isDouble())
                {
                    nAdjust = value.toInt();
                }
            }
        }
    }

    return ret;
}

bool UCarDevice::getVelCaliData(int &nVelocity)
{
    qCInfo(ucar_device()) << __func__ ;

    bool ret = false;

    QJsonObject object;
    object.insert("func", "get_vel_cali_data");
    object.insert("model", "ucar_device");
    QJsonDocument document;
    document.setObject(object);
    QByteArray array = document.toJson(QJsonDocument::Compact);
    std::string ret_json =  m_grpc_inst->execute(array.toStdString());
    qCDebug(ucar_device()) << "grpc return : " << ret_json.c_str();

    QJsonParseError jsonError;
    document = QJsonDocument::fromJson(QByteArray::fromStdString(ret_json), &jsonError); //转化为JSON文档
    if( !document.isNull() && (jsonError.error == QJsonParseError::NoError)) //解析未发生错误
    {
        if(document.isObject())
        {
            QJsonObject object = document.object();

            if(object.contains("return"))
            {
                QJsonValue value = object.value("return");
                if(value.isBool())
                {
                    ret = value.toBool();
                }
            }

            if(object.contains("Velocity"))
            {
                QJsonValue value = object.value("Velocity");
                if(value.isDouble())
                {
                    nVelocity = value.toInt();
                }
            }
        }
    }

    return ret;
}

int UCarDevice::CompareVersion(QString strVersion1, QString strVersion2)
{

    if (!strVersion1.compare(strVersion2))
    {
        return 0;
    }

    QStringList list1 = strVersion1.split( "." );
    QStringList list2 = strVersion2.split( "." );

    int iTotal1 = list1.count();
    int iTotal2 = list2.count();

    int iTotal=iTotal1>iTotal2?iTotal2:iTotal1;

    int iValue1 = 0, iValue2 = 0;
    bool ibOK1 = false, ibOK2 = false;
    for ( int iNum = 0; iNum < iTotal; ++iNum )
    {
        iValue1 = list1[iNum].toInt( &ibOK1 );
        if (!ibOK1)
        {
            iValue1=0;
        }

        iValue2 = list2[iNum].toInt( &ibOK2 );
        if (!ibOK2)
        {
            iValue2=0;
        }

        if (iValue1==iValue2)
        {
            continue;
        }
        else if ( iValue1 < iValue2 )
        {
            return -1;
        }
        else
        {
            return 1;
        }
    }
    return iTotal1<iTotal2?-1:1;
}
