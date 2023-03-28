#include "setting.h"
#include <QFile>
#include <QApplication>
#include <QMap>
#include <QTranslator>
#include <QLoggingCategory>
#include <QWidget>
#include <QList>
#include <QVariant>

Q_LOGGING_CATEGORY(setting, "tool.setting");

namespace {
const QString DEFAULT_STYLESHEET_FILE{":/qss/default.qss"};
//QString CONFIG_FILE{"/home/oneai/.config/robogo_imu.conf"};
//QString CONFIG_FILE{"/usr/local/lib/python3.6/dist-packages/oneAI-0.1.0-py3.6.egg/oneai/common/config/device_config.ini"};
const QString CONFIG_FILE{"/home/oneai/.config/robogo/robogo_config.ini"};
const QString KEY_RPC_SERVER{"RpcServer"};
const QString KEY_RPC_PORT{"RpcPort"};
const QString GROUP_CAMERA{"camera"};
const QString DEFAULT_RPC_SERVER{"127.0.0.1"};
const uint32_t DEFAULT_RPC_PORT{34699};
const QString KEY_MATRIX{"matrix"};
const QString KEY_COEF{"coef"};
const QString KEY_PERSPECTIVE{"perspective"};
const QString KEY_MATRIX_DEFAULT{"matrix_default"};
const QString KEY_COEF_DEFAULT{"coef_default"};
const QString PIC_FOLDER_PATH{"/home/oneai/Pictures/camera_calibration"};
}

Setting* Setting::m_instance = nullptr;

Q_DECLARE_METATYPE(QList<double>)

Setting::Setting(const QString &fileName, QSettings::Format format, QObject *parent)
    : QSettings(fileName, format, parent)
{
    qCDebug(setting()) << __func__;
    qRegisterMetaTypeStreamOperators<QList<double> >("QList<double>");
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

QList<double> Setting::getMatrix()
{
    qCDebug(setting()) << __func__;

    return getFloatListValue(GROUP_CAMERA, KEY_MATRIX);
}

QList<double> Setting::getCoef()
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

QList<double> Setting::getFloatListValue(const QString &group, const QString &key)
{
    qCDebug(setting()) << __func__ << " Key -> " << key;

    QList<double> result;
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
                result.append(v.toDouble());
            }
        }
        qCDebug(setting()) << "float list: " << result;
    }

    return result;
}

bool Setting::setMatrixAndCoef(const QList<double> &matrix, const QList<double> &coef)
{
    qCInfo(setting()) << __func__ << " Matrix: " << matrix << " Coef: " << coef;
    if (matrix.size() < 9 || coef.size() < 5)
    {
        return false;
    }

    QString str_matrix;
    for (auto v : matrix)
    {
        str_matrix += QString::number(v);
        str_matrix += " ";
    }
    str_matrix = str_matrix.trimmed();

    QString str_coef;
    for (auto v : coef)
    {
        str_coef += QString::number(v);
        str_coef += " ";
    }
    str_coef = str_coef.trimmed();

    beginGroup(GROUP_CAMERA);
    setValue(KEY_MATRIX, str_matrix);
    setValue(KEY_COEF, str_coef);
    endGroup();

    sync();
    return true;
}

bool Setting::restorMatrixAndCoef()
{
    auto default_matrix = getFloatListValue(GROUP_CAMERA, KEY_MATRIX_DEFAULT);
    auto default_coef = getFloatListValue(GROUP_CAMERA, KEY_COEF_DEFAULT);

    return setMatrixAndCoef(default_matrix, default_coef);
}

QString Setting::getCameraPicFolderPath()
{
    return PIC_FOLDER_PATH;
}
