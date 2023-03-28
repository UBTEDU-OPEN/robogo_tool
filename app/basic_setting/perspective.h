#ifndef PERSPECTIVE_H
#define PERSPECTIVE_H

#include <QWidget>
#include <QPixmap>
#include <memory>

namespace Ui {
class PerspectiveUi;
}

class PerspectiveController;

class Perspective : public QWidget
{
    Q_OBJECT

    const QString kTipsTextAuto{QObject::tr("请将摄像头画面中的两条红线与地图矩形框内的中线和底边重合，地图矩形框全部变绿则表明标定成功！")};
    const QString kTipsTextManual{QObject::tr("请从左到右依次点击画面内地图矩形框的四个角，根据摄像头内文字提示完成手动标定！")};


public:
    enum ManualSteps : uint8_t{
        ManualStep1 = 0,
        ManualStep2,
        ManualStep3,
        ManualStep4,
        ManualStepDone
    };
    explicit Perspective(QWidget *parent = nullptr);
    ~Perspective();

    void setManualMode(bool on);
    bool isManualMode();
    void setManualProgress(ManualSteps step, bool nextBtnEnabled=false);
    QSize getLiveWidgetSize();

signals:
    void backToPreviousView();
    void enterCalibration();
    void exitCalibration();
    void selectPoint(QPoint point);

private slots:
    void onUpdateLive(QPixmap frame);
    void onUpdateText(QString text);
    void onSetFinishBtnEnable(bool bEnable);

private:
    bool eventFilter(QObject *watched, QEvent *event) override;

    std::unique_ptr<Ui::PerspectiveUi> m_ui;
    std::unique_ptr<PerspectiveController> m_controller;

};

#endif // PERSPECTIVE_H
