#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPoint>

#include <memory>
//#include <functional>

namespace Ui {
class MainWindow;
}

class MainWindowController;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
//    typedef std::function<void (uint8_t index)> TabChangedCallback;
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    /*
    void setTabChangedCallback(const TabChangedCallback &cb)
    {
        m_tab_changed_callback = cb;
    }
    */
public:
    void setCalibrationState(bool on);
    void confirmCloseEvent();
    void setTabForbiddenMsg(const QString message);
    bool isUpdating() const;

public slots:
    void setModalState(bool on);
    void setOtaModalState(bool on);

    // QWidget interface
protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

signals:
    void tabIndexChanged(uint8_t index);


private:
    void onTabButtonClicked();

    Ui::MainWindow *ui;
//    TabChangedCallback m_tab_changed_callback;
    std::unique_ptr<MainWindowController> m_controller;
    QPoint m_last_point;
    bool m_holding_panel;
    bool m_on_calibration;
    bool m_close_confirmed;
    bool m_bIsUpdating;
    QString m_tab_forbidden_msg;
};

#endif // MAINWINDOW_H
