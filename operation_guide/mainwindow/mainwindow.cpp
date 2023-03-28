#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mainwindow_controller.h"
#include "guide_step.h"
#include <QStyleOption>
#include <QPainter>
#include <QLoggingCategory>
#include <QMouseEvent>
#include <QTimer>
#include <assert.h>
#include <QBitmap>

Q_LOGGING_CATEGORY(mainwindow, "mainwindow")

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_last_point(-1,-1),
    m_holding_panel(false)
{
    ui->setupUi(this);

    ui->indicator->installEventFilter(this);

    setWindowFlags(Qt::FramelessWindowHint);
    setWindowIcon(QIcon(":/ui/images/robogo_icon.png"));
//    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_AlwaysStackOnTop);

    m_controller = std::make_unique<MainWindowController>(this);
    connect(ui->pb_close, &QPushButton::released, m_controller.get(), &MainWindowController::onCloseClicked);
    connect(ui->pb_ok, &QPushButton::released, m_controller.get(), &MainWindowController::onCloseClicked);

    QTimer::singleShot(1, this, &MainWindow::initGuideSteps);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    QBitmap bmp(this->size());
    bmp.fill();
    QPainter painter(&bmp);
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::black);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawRoundedRect(bmp.rect(), 13, 13);
    setMask(bmp);
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

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui->indicator && event->type() == QEvent::Paint)
    {
        QPainter painter;
        painter.begin(ui->indicator);
        painter.setRenderHint(QPainter::Antialiasing, true);

        auto w = ui->indicator->size().width();
        auto start = w / 2 - 45* ui->stackedWidget->count()/2.0;
        for (auto i = 0; i < ui->stackedWidget->count(); ++i)
        {
            if (i == ui->stackedWidget->currentIndex())
            {
                painter.setBrush(QBrush(QColor(0x0, 0x7A, 0xFF)));
                painter.setPen(QColor(0x0, 0x7A, 0xFF));
            }
            else
            {
                painter.setBrush(QBrush(QColor(0xF0, 0xF0, 0xF0)));
                painter.setPen(QColor(0xF0, 0xF0, 0xF0));
            }
            painter.drawRect(start + 50*i, 0, 40, 4);
        }
        painter.end();
        return true;
    }

    return false;

}

void MainWindow::initGuideSteps()
{
    std::vector<GuideStep* > steps;

    GuideStep *step_1 = new GuideStep(1, STEP1_TEXT, GuideStep::Image, ":/ui/images/img_magnetic_guide01.png", this, GuideStep::First);
    steps.push_back(step_1);
    GuideStep *step_2 = new GuideStep(2, STEP2_TEXT, GuideStep::Video, "/usr/bin/robogo_videos/video.mp4", this);
    steps.push_back(step_2);
#if 0
    GuideStep *step_3 = new GuideStep(3, STEP3_TEXT, GuideStep::Video, "/usr/bin/robogo_videos/video_y.mp4", this);
    steps.push_back(step_3);
    GuideStep *step_4 = new GuideStep(4, STEP4_TEXT, GuideStep::Video, "/usr/bin/robogo_videos/video_z.mp4", this);
    steps.push_back(step_4);
    QString locale = QLocale::system().name();//根据系统语言
    if (locale.contains("zh",Qt::CaseInsensitive))
    {
        GuideStep *step_5 = new GuideStep(5, STEP5_TEXT, GuideStep::Image, ":/ui/images/img_magnetic_guide05.png", this, GuideStep::Last);
        steps.push_back(step_5);
    }
    else if (locale.contains("en",Qt::CaseInsensitive))
    {
        GuideStep *step_5 = new GuideStep(5, STEP5_TEXT, GuideStep::Image, ":/ui/images/img_magnetic_guide05_en.png", this, GuideStep::Last);
        steps.push_back(step_5);
    }
#else
    QString locale = QLocale::system().name();//根据系统语言
    if (locale.contains("zh",Qt::CaseInsensitive))
    {
        GuideStep *step_3 = new GuideStep(3, STEP5_TEXT, GuideStep::Image, ":/ui/images/img_magnetic_guide05.png", this, GuideStep::Last);
        steps.push_back(step_3);
    }
    else if (locale.contains("en",Qt::CaseInsensitive))
    {
        GuideStep *step_3 = new GuideStep(3, STEP5_TEXT, GuideStep::Image, ":/ui/images/img_magnetic_guide05_en.png", this, GuideStep::Last);
        steps.push_back(step_3);
    }
#endif


    auto steps_count = steps.size();
    for(decltype(steps_count) i = 0; i < steps_count; ++i)
    {
        connect(steps[i], &GuideStep::goToNextStep, this, &MainWindow::onStepChanged);
        ui->stackedWidget->addWidget(steps[i]);
    }
}

void MainWindow::onStepChanged(int8_t direction)
{
    auto current_index = ui->stackedWidget->currentIndex();
    if (direction < 0 && current_index >= 1)
    {
        ui->stackedWidget->setCurrentIndex(current_index - 1);
    }
    else if (direction > 0 && current_index <= 3)
    {
        ui->stackedWidget->setCurrentIndex(current_index + 1);
    }

    ui->indicator->update();
}
