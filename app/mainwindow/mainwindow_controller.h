#ifndef MAINWINDOW_CONTROLLER_H
#define MAINWINDOW_CONTROLLER_H

#include <QObject>

class MainWindow;

class MainWindowController : public QObject
{
    Q_OBJECT
public:
    explicit MainWindowController(MainWindow* view, QObject *parent = nullptr);

signals:

public slots:
    void onTabIndexChanged(uint8_t index);
    void onCloseClicked();
    void onMinimizeClicked();
    void onEnterCalibration(QString toast_message, QString dialog_message);
    void onExitCalibration();
private:
    MainWindow* m_view;
    bool m_on_calibration;
    QString m_close_dialog_msg;
};

#endif // MAINWINDOW_CONTROLLER_H
