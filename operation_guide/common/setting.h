#ifndef SETTING_H
#define SETTING_H

#include "singleton.h"
#include <QSettings>



class Setting : public QSettings, public Singleton<Setting>
{
    Q_OBJECT
public:
    /**
     * @brief loadDefaultStyle
     *  加载默认stylesheet文件
     */
    void loadDefaultStyle();

    /**
     * @brief init
     * 初始化配置文件，如果没有文件，则用默认配置生成配置文件，如果已经有配置文件，
     * 不会覆盖已有文件
     */
    void init();

    /**
     * @brief instance
     * 返回Setting类的单例对象
     * @return
     */
    static Setting &instance();

    /**
     * @brief getRpcServerAddr
     * 从配置文件中读取并返回rpc服务器的地址
     * @return
     * rpc服务器地址, 默认为'127.0.0.1'
     */
    QString getRpcServerAddr();

    /**
     * @brief getRpcPort
     * 从配置文件中读取并返回rpc服务器的端口号
     * @return
     * rpc服务器的端口号
     */
    uint32_t getRpcPort();

    /**
     * @brief getMatrix
     * 获取内参3*3矩阵值
     * @return
     * QList 参数值
     */
    QList<float> getMatrix();

    /**
     * @brief getCoef
     * 获取内参1*5矩阵值
     * @return
     * QList 参数值
     */
    QList<float> getCoef();

    /**
     * @brief setPerspective
     *  设置透视变幻参数值
     * @param value
     *  参数QList
     * @return
     *  成功返回true, 失败false
     */
    bool setPerspective(const QList<uint16_t> &value);


protected:
    Setting(const QString &fileName, QSettings::Format format, QObject *parent = nullptr);

private:
    QList<float> getFloatListValue(const QString &group, const QString &key);

    static Setting *m_instance;
};



#endif // SETTING_H
