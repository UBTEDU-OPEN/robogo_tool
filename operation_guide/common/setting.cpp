#include "setting.h"
#include <QFile>
#include <QApplication>
#include <QMap>
#include <QTranslator>
#include <QLoggingCategory>
#include <QWidget>

Q_LOGGING_CATEGORY(setting, "tool.setting");

namespace {
QString DEFAULT_STYLESHEET_FILE{":/qss/default.qss"};
//QString CONFIG_FILE{"/home/oneai/.config/robogo_imu.conf"};
//QString CONFIG_FILE{"/usr/local/lib/python3.6/dist-packages/oneAI-0.1.0-py3.6.egg/oneai/common/config/device_config.ini"};
QString CONFIG_FILE{"/home/oneai/.config/robogo/robogo_config.ini"};
QString KEY_RPC_SERVER{"RpcServer"};
QString KEY_RPC_PORT{"RpcPort"};
QString GROUP_CAMERA{"camera"};
QString DEFAULT_RPC_SERVER{"127.0.0.1"};
uint32_t DEFAULT_RPC_PORT{34699};
QString KEY_MATRIX{"matrix"};
QString KEY_COEF{"coef"};
QString KEY_PERSPECTIVE{"perspective"};
}

Setting* Setting::m_instance = nullptr;

Setting::Setting(const QString &fileName, QSettings::Format format, QObject *parent)
    : QSettings(fileName, format, parent)
{
    qCDebug(setting()) << __func__;
}

void Setting::loadDefaultStyle()
{
    QFile file(DEFAULT_STYLESHEET_FILE);
    file.open(QFile::ReadOnly);
    QString styleSheet = QString::fromLatin1(file.readAll());
    if(!styleSheet.isEmpty())
    {
        qApp->setStyleSheet(styleSheet);
    }
    else
    {
        qCWarning(setting()) << "no qss found in " << DEFAULT_STYLESHEET_FILE;
    }
    file.close();
}

void Setting::init()
{
    if (QSettings::childGroups().contains(GROUP_CAMERA))
    {
        return ;
    }
    qCInfo(setting()) << "Generating default rpc server info";

    /*
    // 默认配置
    setValue(KEY_RPC_SERVER, DEFAULT_RPC_SERVER);
    setValue(KEY_RPC_PORT, DEFAULT_RPC_PORT);

    sync();
    */
}

Setting &Setting::instance()
{
    if (!m_instance)
    {
        m_instance = new Setting(CONFIG_FILE, IniFormat);
    }

    return *m_instance;
}

QString Setting::getRpcServerAddr()
{
    qCDebug(setting()) << __func__;

    QString addr = this->value(KEY_RPC_SERVER, DEFAULT_RPC_SERVER).toString();
    qCDebug(setting()) << "Rpc server address -> " << addr;

    return addr;
}

uint32_t Setting::getRpcPort()
{
    qCDebug(setting()) << __func__;

    uint32_t port = this->value(KEY_RPC_PORT, DEFAULT_RPC_PORT).toUInt();
    qCDebug(setting()) << "Rpc server port -> " << port;

    return port;

}

QList<float> Setting::getMatrix()
{
    qCDebug(setting()) << __func__;

    return getFloatListValue(GROUP_CAMERA, KEY_MATRIX);
}

QList<float> Setting::getCoef()
{
    qCDebug(setting()) << __func__;

    return getFloatListValue(GROUP_CAMERA, KEY_COEF);
}

bool Setting::setPerspective(const QList<uint16_t> &value)
{
    QString str_value;
    for (auto v : value)
    {
        str_value += QString::number(v);
        str_value += " ";
    }

    beginGroup(GROUP_CAMERA);
    setValue(KEY_PERSPECTIVE, str_value);
    endGroup();

    sync();
    return true;
}

QList<float> Setting::getFloatListValue(const QString &group, const QString &key)
{
    qCDebug(setting()) << __func__ << " Key -> " << key;

    QList<float> result;
    if (childGroups().contains(group))
    {
        beginGroup(group);
        QString value = this->value(key, "").toString();
        endGroup();
        qCDebug(setting()) << "value: " << value;
        if(value.size() > 0)
        {
            for(QString v: value.split(" "))
            {
                result.append(v.toFloat());
            }
        }
        qCDebug(setting()) << "float list: " << result;
    }

    return result;
}
