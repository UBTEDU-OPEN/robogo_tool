#ifndef MAINWINDOW_CONTROLLER_H
#define MAINWINDOW_CONTROLLER_H

#include <QObject>

class MainWindow;

class MainWindowController : public QObject
{
    Q_OBJECT
public:
    explicit MainWindowController(MainWindow* view, QObject *parent = nullptr);

public slots:
    void onCloseClicked();

private:
    MainWindow* m_view;
};

#endif // MAINWINDOW_CONTROLLER_H
