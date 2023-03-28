#ifndef ADVANCED_SETTING_CONTROLLER_H
#define ADVANCED_SETTING_CONTROLLER_H

#include <QObject>

class AdvancedSetting;

class AdvancedSettingController : public QObject
{
    Q_OBJECT
public:
    explicit AdvancedSettingController(AdvancedSetting* view,  QObject *parent = nullptr);
    ~AdvancedSettingController();

signals:
    void setWindowModal(bool on);

public slots:
    void onResetClicked();
    void onInstallationGuide();
    void onCheckOta();

private:
    AdvancedSetting* m_view;
};

#endif // ADVANCED_SETTING_CONTROLLER_H
