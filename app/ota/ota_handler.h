#ifndef OTA_HANDLER_H
#define OTA_HANDLER_H

#include <QThread>
#include <QFile>
#include <memory>
#include <thread>

class OtaHandler : public QThread
{
    Q_OBJECT

    friend void upgradeThreadBody(OtaHandler* ins);
public:
    OtaHandler();

    enum class ErrorType {
        FileNotFound = 0,
        InvalidMd5,
        UserCancelled,
        StateErrorEnterMode,
        NotEnoughSpace,
        SendingData,
        Md5NotMatch,
        Unknown
    };

public slots:
    /**
     * @brief startUpgrade
     *  开始升级
     * @param file 升级文件路径
     * @param md5 固件MD5值
     */
    void startUpgrade(QString file, QString md5);

    /**
     * @brief cancelUpgrade
     *  取消升级
     */
    void cancelUpgrade();

signals:
    /**
     * @brief enteredUpgradeMode
     *  成功进入升级模式
     */
    void enteredUpgradeMode();

    /**
     * @brief failedToUpgrade
     *  升级失败
     * @param error 失败原因，错误号
     */
    void failedToUpgrade(ErrorType err);

    /**
     * @brief progress
     *  更新升级进度
     * @param num 当前包序号
     * @param total 总包数
     */
    void progress(uint16_t num, uint16_t total);

    /**
     * @brief upgradeFinished
     */
    void upgradeFinished();


    // QThread interface
protected:
    void run() override;

    static void upgradeThreadBody(OtaHandler* ins);

    QFile m_file;
    QString m_md5;
    pthread_t m_thread_id;
};

#endif // OTA_HANDLER_H
