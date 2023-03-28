#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPoint>
#include <memory>

namespace Ui {
class MainWindow;
}

class MainWindowController;
class GuideStep;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    QString STEP1_TEXT{QObject::tr("步骤1:将小车（传感器）放置于空间远离磁场干扰的环境中，距离硬磁性物体（扬声器、磁铁等）大于30cm，"
                        "距离软磁性物体（铁、镍、地面内的钢筋、桌子下方的铁梁、铁钉等）10cm以上，最好是用手拿着悬空在空间中，"
                        "先将小车车头向着右手边。位置参考图如下：")};
     QString STEP2_TEXT{QObject::tr("步骤2:点击开始校准按钮，按照视频所示，旋转360度以上。旋转过程中尽量保持小车平稳。")};
     QString STEP3_TEXT{QObject::tr("步骤3:完成步骤2的旋转操作，且拟合出规整的椭圆后，按照视频所示继续拟合下一个椭圆。")};
     QString STEP4_TEXT{QObject::tr("步骤4:完成步骤3旋转操作，且拟合出规整的椭圆后，按照视频所示，继续拟合下一个椭圆。")};
     QString STEP5_TEXT{QObject::tr("注意：数据点尽量在绿色椭圆圈附近，如果不能拟合出椭圆，请远离磁场干扰，再参考校准视频，可以多转几圈。")};
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    // QObject interface
    bool eventFilter(QObject *watched, QEvent *event) override;

    // QWidget interface
protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private slots:
    void onStepChanged(int8_t direction);
    void initGuideSteps();

private:

    Ui::MainWindow *ui;
    std::unique_ptr<MainWindowController> m_controller;
    QPoint m_last_point;
    bool m_holding_panel;
};

#endif // MAINWINDOW_H
