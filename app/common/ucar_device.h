#ifndef UCAR_DEVICE_H
#define UCAR_DEVICE_H

#include "singleton.h"
#include <QObject>
#include <QThread>
#include <memory>
#include <thread>
#include <vector>

#define UCAR_DEFAULT_VELOCITY 32

class InferContactClient;

class UCarDevice : public QThread, public Singleton<UCarDevice>
{
    Q_OBJECT
public:
    ~UCarDevice();
    static UCarDevice &instance();


    void enableDistributing(bool enable);
    bool getMagneticOffsets(int32_t &x, int32_t &y, int32_t &z);
    bool getAccelOffsets(int32_t &x, int32_t &y, int32_t &z);
    bool startAccelAdjusting();
    bool startMagneticAdjusting();
    bool exitAdjusting();
    bool reset();
    bool unload();
    /**
     * @brief enterUpgrading
     *  进入升级模式, 执行其他升级指令前，要先进入升级模式
     * @return
     *  true: 成功
     *  false: 失败
     */
    bool enterUpgrading();

    /**
     * @brief startUpgrading
     *  开始升级，发送前置信息，包括数据包总包数
     * @param package_amount 固件数据包
     * @return
     *  true: 成功
     *  false: 失败
     */
    bool startUpgrading(const uint16_t package_amount);

    /**
     * @brief sendUpgradingData
     *  发送升级包数据
     * @param package_num 包序号，从1开始计数
     * @param package_data 包数据，长度最大为128字节
     * @return
     *  true: 成功
     *  false: 失败
     */
    bool sendUpgradingData(const uint16_t package_num, const std::vector<uint8_t> &package_data);

    /**
     * @brief finishUpgrading
     *  结束升级，发送固件文件MD5给副板进行校验
     * @param md5 16位md5字符串
     * @return
     *  true: 成功, 此时副板已经开始自升级
     *  false: 失败
     */
    bool finishUpgrading(const QString &md5);

    /**
     * @brief cancelUpgrading
     *  取消升级
     * @return
     *  true: 成功, 此时副板已经开始自升级
     *  false: 失败
     */
    bool cancelUpgrading();

    /**
     * @brief getSoftwareVersion
     *  获取底盘固件版本号
     * @param version 返回版本号字符串
     * @return
     *  true: 成功, false: 失败, 失败时version值无效
     */
    bool getSoftwareVersion(QString &version);

    /**
     * @brief setPerspective
     *  更新透视变化参数（外参）
     * @param value
     *  外参数值列表
     * @return
     *  true: 成功, false: 失败
     */
    bool setPerspective(const QList<uint16_t> &value);

    /**
     * @brief driveForward
     *  前进
     * @param int nVelocity 速度
     * @return
     *  true: 成功, false: 失败
     */
    bool driveForward(const int nVelocity);

    /**
     * @brief driveStop
     *  停止
     * @return
     *  true: 成功, false: 失败
     */
    bool driveStop();

    /**
     * @brief driveLeftDirection
     *  左转
     * @param int nAngle 角度
     * @return
     *  true: 成功, false: 失败
     */
    bool driveLeftDirection(const int nAngle);

    /**
     * @brief driveRightDirection
     *  右转
     * @param int nAngle 角度
     * @return
     *  true: 成功, false: 失败
     */
    bool driveRightDirection(const int nAngle);

    /**
     * @brief servoComebackCenter
     *  回中
     * @return
     *  true: 成功, false: 失败
     */
    bool servoComebackCenter();


    /**
     * @brief setServoAdjust
     *  设置回中补偿值
     * @param int 回中参数:1,左回中补偿，2,右回中补偿
     * @param int 返回回中补偿值
     * @return
     *  true: 成功, false: 失败
     */
    bool setServoAdjust(const int nDir, const int nAdjust);

    /**
     * @brief getServoAdjust
     *  获取回中补偿值
     * @param int 回中参数:1,左回中补偿，2,右回中补偿
     * @param int 返回回中补偿值
     * @return
     *  true: 成功, false: 失败
     */
    bool getServoAdjust(const int nDir, int &nAdjust);

    /**
     * @brief getVelCaliData
     *  获取巡线速度
     * @param int 返回巡线速度
     * @return
     *  true: 成功, false: 失败
     */
    bool getVelCaliData(int &nVelocity);

    /**************************************************************************
    * 函数名: CompareVersion
    * 功能: 比较版本大小
    * 参数: [@in] strVersion1
    * 参数: [@in] strVersion2
    * 返回值: strVersion1 >  strVersion2 返回1
    *        strVersion1 <  strVersion2 返回-1
    *        strVersion1 =  strVersion2 返回0
    * 时间:2021/03/24
    * 作者: Joker
    */
    int CompareVersion(QString strVersion1, QString strVersion2);

signals:
    void requestImuData();
    void accelData(float x, float y, float z);
    void magneticData(int32_t x, int32_t y, int32_t z);
    void angleData(float x, float y, float z);

    // QThread interface
protected:
    void run() override;

private slots:
    void onRequestImuData();

private:
    explicit UCarDevice(const QString ip, const uint32_t port, QObject *parent = nullptr);

    bool init();

    static void distributingThread(UCarDevice* ins);

    void getImuData(std::vector<float> &data);

    bool getOffsets(int32_t &x, int32_t &y, int32_t &z, const uint8_t start_index);

    enum AdjustingMode {
        AccelMode = 0,
        MagneticMode
    };
    bool startAdjusting(const AdjustingMode mode);
    bool grpcCallWithoutArgs(const std::string &func);

    static UCarDevice* m_instance;

    QString m_ip;
    uint32_t m_port;
    volatile bool m_require_distribute;
    std::unique_ptr<std::thread> m_thread;
    std::unique_ptr<InferContactClient> m_grpc_inst;
};

#endif // UCAR_DEVICE_H
