#include "ota_handler.h"
#include "ucar_device.h"
#include <QLoggingCategory>
#include <assert.h>

Q_LOGGING_CATEGORY(ota_handler, "ota_handler")

namespace {
const uint16_t PACKAGE_SIZE{128};
}

using namespace std;

OtaHandler::OtaHandler()
    : m_thread_id(-1)
{
    qRegisterMetaType<ErrorType>("ErrorType");
}

void OtaHandler::startUpgrade(QString file, QString md5)
{
    qCInfo(ota_handler()) << __func__ << " file path: " << file << " md5: " << md5;
    m_file.setFileName(file);
    bool ret = m_file.open(QIODevice::ReadOnly);
    if (!ret)
    {
        qCInfo(ota_handler()) << "Failed to open file";
        emit failedToUpgrade(ErrorType::FileNotFound);
        return ;
    }

    if (md5.size() < 32)
    {
        qCInfo(ota_handler()) << "md5 too short";
        emit failedToUpgrade(ErrorType::InvalidMd5);
        return ;
    }
    m_md5 = md5;

    std::thread upgrade_thread(OtaHandler::upgradeThreadBody, this);
    m_thread_id = upgrade_thread.native_handle();
    upgrade_thread.detach();

    return ;
}

void OtaHandler::cancelUpgrade()
{
    qCInfo(ota_handler()) << __func__;
    if (m_thread_id != -1)
    {
        pthread_cancel(m_thread_id);
    }

    emit failedToUpgrade(ErrorType::UserCancelled);
}

void OtaHandler::run()
{
    exec();
}

void OtaHandler::upgradeThreadBody(OtaHandler* ins)
{
    assert(ins);

    bool ret = UCarDevice::instance().enterUpgrading();
    if (!ret)
    {
        emit ins->failedToUpgrade(ErrorType::StateErrorEnterMode);
        qCInfo(ota_handler()) << "Failed to enter upgrade mode";
        return ;
    }

    auto file_size = ins->m_file.size();
    uint16_t amount = file_size / PACKAGE_SIZE + (file_size % PACKAGE_SIZE > 0 ? 1 : 0);

    ret = UCarDevice::instance().startUpgrading(amount);
    if (!ret)
    {
        qCInfo(ota_handler()) << "Failed to start upgrade";
        emit ins->failedToUpgrade(ErrorType::NotEnoughSpace);
        return ;
    }

    uint8_t error_count = 0;
    char buf[PACKAGE_SIZE];
    for (auto i = 1; i <= amount; ++i)
    {
        auto bytes_read = ins->m_file.read(buf, PACKAGE_SIZE);
        printf("read %ld bytes from file\n", bytes_read);
        vector<uint8_t> data_vec(buf, buf+bytes_read);

        ret = UCarDevice::instance().sendUpgradingData(i, data_vec);
        if (!ret)
        {
            qCInfo(ota_handler()) << "send package error " << i ;
            if (error_count > 1)
            {
                emit ins->failedToUpgrade(ErrorType::SendingData);
                qCInfo(ota_handler()) << "Failed to send package " << i;
                return ;
            }

            error_count++;

            if (i % 2 == 0)
            {
                ins->m_file.seek(PACKAGE_SIZE * (i - 2));
                i -= 3;
            }
            else
            {
                ins->m_file.seek(PACKAGE_SIZE * (i - 1));
                i -= 2;
            }
            continue;
        }
        else
        {
            error_count = 0;
            emit ins->progress(i, amount);
        }
    }

    ret = UCarDevice::instance().finishUpgrading(ins->m_md5);
    if (!ret)
    {
        emit ins->failedToUpgrade(ErrorType::Md5NotMatch);
        qCInfo(ota_handler()) << "Md5 not match";
        return ;
    }

    emit ins->upgradeFinished();
}
