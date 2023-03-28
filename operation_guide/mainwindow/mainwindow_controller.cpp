#include "mainwindow_controller.h"
#include "mainwindow.h"
#include <QLoggingCategory>
#include <assert.h>

Q_LOGGING_CATEGORY(mainwindow_controller, "mainwindow_controller")

MainWindowController::MainWindowController(MainWindow *view, QObject *parent)
    : QObject(parent),
      m_view(view)
{
    assert(m_view);
}

void MainWindowController::onCloseClicked()
{
    qCInfo(mainwindow_controller()) << __func__;
    m_view->close();
}
