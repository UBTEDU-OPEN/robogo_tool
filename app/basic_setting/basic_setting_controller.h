#ifndef BASIC_SETTING_CONTROLLER_H
#define BASIC_SETTING_CONTROLLER_H

#include <QObject>

class BasicSetting;

class BasicSettingController : public QObject
{
    Q_OBJECT

    const QString PERSPECTIVE_CLOSE_CONFIRM_MESSAGE{QObject::tr("<html><head/><body><p>正在标定摄像头，若关闭工具，标定过程将终止，</p><p>是否关闭工具？</p></body></html>")};
    const QString PERSPECTIVE_TAB_FROBIDDEN_MESSAGE{QObject::tr("当前正在标定中，不可切换界面")};
    const QString TOAST_RESTORE_CAM_PARAM_SUCCESS{QObject::tr("恢复出厂内参成功！")};
    const QString TOAST_RESTORE_CAM_PARAM_ERROR{QObject::tr("恢复出厂内参失败！")};
public:
    explicit BasicSettingController(BasicSetting* view, QObject *parent = nullptr);

signals:
    void displaySubview(QWidget *view);
    void dismissSubview(QWidget *view);

public slots:
    /**
     * @brief onAccelerationPressed
     * 加速度按钮点击事件处理函数
     */
    void onAccelerationPressed();

    /**
     * @brief onMagneticFieldPressed
     * 磁场按钮点击事件处理函数
     */
    void onMagneticFieldPressed();

    /**
     * @brief onPerspectivePressed
     * 外参校准按钮点击事件处理函数
     */
    void onPerspectivePressed();

    /**
     * 前轮回中校准按钮点击事件处理函数
     * @brief onRollbackButtonPressed
     */
    void onRollbackButtonPressed();

    /**
     * @brief onHomeViewShowed
     * 主界面显示事件
     */
    void onHomeViewShowed();

    /**
     * @brief onCameraParameterCalibrationPressed
     * 内参校准按钮点击事件处理函数
     */
    void onCameraParameterCalibrationPressed();

    /**
     * @brief onCameraParamInfoPressed
     * 内参参数查看按钮点击事件处理函数
     */
    void onCameraParamInfoPressed();

    /**
     * @brief onCameraParamInfoPressed
     * 内参参数还原按钮点击事件处理函数
     */
    void onCameraParamRestorePressed();

    /**
     * @brief onCameraPicBrowserPressed
     * 内参图片查看
     */
    void onCameraPicBrowserPressed();

private:
    BasicSetting* m_view;
};

#endif // BASIC_SETTING_CONTROLLER_H
