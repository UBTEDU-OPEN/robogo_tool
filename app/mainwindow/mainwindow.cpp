#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mainwindow_controller.h"
#include "toast_manager.h"
#include <QStyleOption>
#include <QPainter>
#include <QLoggingCategory>
#include <QMouseEvent>
#include <assert.h>
#include <opencv2/opencv.hpp>
#include <QDesktopWidget>

Q_LOGGING_CATEGORY(mainwindow, "mainwindow")

Q_DECLARE_METATYPE(cv::Mat)

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_last_point(-1,-1),
    m_holding_panel(false),
    m_on_calibration(false),
    m_close_confirmed(false),
    m_bIsUpdating(false),
    m_tab_forbidden_msg("")
{
    ui->setupUi(this);

    setWindowFlags(Qt::FramelessWindowHint);
    setWindowIcon(QIcon(":/ui/images/robogo_icon.png"));

    m_controller = std::make_unique<MainWindowController>(this);

    connect(ui->pb_home, &QPushButton::released, this, &MainWindow::onTabButtonClicked);
    connect(ui->pb_basic, &QPushButton::released, this, &MainWindow::onTabButtonClicked);
    connect(ui->pb_advance, &QPushButton::released, this, &MainWindow::onTabButtonClicked);
    connect(ui->page_advanced, &Advanced_Stack::sigAdvancedStackTab, ui->pb_advance, &QPushButton::released);
    connect(this, &MainWindow::tabIndexChanged, m_controller.get(), &MainWindowController::onTabIndexChanged);
    connect(ui->pb_minimize, &QPushButton::released, m_controller.get(), &MainWindowController::onMinimizeClicked);
    connect(ui->pb_close, &QPushButton::released, m_controller.get(), &MainWindowController::onCloseClicked);
    connect(ui->page_basic, &BasicSetting::enterCalibration, m_controller.get(), &MainWindowController::onEnterCalibration);
    connect(ui->page_basic, &BasicSetting::exitCalibration, m_controller.get(), &MainWindowController::onExitCalibration);
    connect(ui->page_home, &Homepage::updateModalState, this, &MainWindow::setModalState);
    connect(ui->page_home, &Homepage::sigCheckOta, ui->page_advanced, &Advanced_Stack::sigCheckOta);
    connect(ui->page_advanced, &Advanced_Stack::setWindowModal,this , &MainWindow::setModalState);
    connect(ui->page_advanced, &Advanced_Stack::setOtaWindowModal,this , &MainWindow::setOtaModalState);

    qRegisterMetaType<cv::Mat>();
    QRect rectMain = QApplication::desktop()->availableGeometry();

    move(rectMain.width()/2 - this->width()/2, rectMain.height()/2 - this->height()/2);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    QWidget::paintEvent(event);
}

void MainWindow::onTabButtonClicked()
{
    qCInfo(mainwindow()) << __func__;
    if (m_on_calibration)
    {
        qCInfo(mainwindow()) << "Calibration is in progress, tab button will not act";
        if (m_tab_forbidden_msg.length() > 0)
        {
            ToastManager::instance().show(m_tab_forbidden_msg);
        }
        return ;
    }

    QObject* obj = sender();

    QPushButton* button = qobject_cast<QPushButton*>(obj);

    if (!button)
    {
        qCWarning(mainwindow()) << "Sender is not button";
        return;
    }

    qCDebug(mainwindow()) << "home button status -> " << ui->pb_home->property("selected").toBool();
    qCDebug(mainwindow()) << "basic button status -> " << ui->pb_basic->property("selected").toBool();
    qCDebug(mainwindow()) << "advance button status -> " << ui->pb_advance->property("selected").toBool();
    ui->pb_home->setProperty("selected", false);
    ui->pb_basic->setProperty("selected", false);
    ui->pb_advance->setProperty("selected", false);

    QVariant index = button->property("tab_index");
    uint32_t index_num = index.toUInt();
    qCInfo(mainwindow()) << "Clicked on tab " << index_num;
    button->setProperty("selected", true);
    ui->stackedWidget->setCurrentIndex(index_num);
    emit tabIndexChanged(index_num);
    ui->pb_home->style()->unpolish(ui->pb_home);
    ui->pb_basic->style()->unpolish(ui->pb_basic);
    ui->pb_advance->style()->unpolish(ui->pb_advance);
    ui->pb_home->style()->polish(ui->pb_home);
    ui->pb_basic->style()->polish(ui->pb_basic);
    ui->pb_advance->style()->polish(ui->pb_advance);
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (ui->panel->rect().contains(event->globalPos() - this->frameGeometry().topLeft()))
    {
        m_holding_panel = true;
        m_last_point = event->globalPos();
    }

    QMainWindow::mousePressEvent(event);
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    m_holding_panel = false;

    QMainWindow::mouseReleaseEvent(event);
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (m_holding_panel)
    {
        auto current_pos = this->pos();
        this->move(current_pos + event->globalPos() - m_last_point);
        m_last_point = event->globalPos();
    }
}

void MainWindow::setCalibrationState(bool on)
{
    qCInfo(mainwindow()) << __func__ << " is on -> " << on;
    m_on_calibration = on;
}

void MainWindow::setModalState(bool on)
{
     ui->pb_home->setDisabled(on);
    ui->pb_basic->setDisabled(on);
    ui->pb_advance->setDisabled(on);
    ui->pb_minimize->setDisabled(on);
    ui->pb_close->setDisabled(on);
}

void MainWindow::setOtaModalState(bool on)
{
    qCInfo(mainwindow()) << __func__ << " is on -> " << on;
    ui->pb_home->setDisabled(on);
    ui->pb_basic->setDisabled(on);
    ui->pb_advance->setDisabled(on);
    m_bIsUpdating = on;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    qCInfo(mainwindow()) << __func__;

    if(false == m_close_confirmed)
    {
        emit ui->pb_close->released();
        event->ignore();
        return ;
    }
    event->accept();
}

void MainWindow::confirmCloseEvent()
{
    m_close_confirmed = true;
}

void MainWindow::setTabForbiddenMsg(const QString message)
{
    m_tab_forbidden_msg = message;
}

bool MainWindow::isUpdating() const
{
    return m_bIsUpdating;
}
