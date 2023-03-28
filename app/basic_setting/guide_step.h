#ifndef GUIDE_STEP_H
#define GUIDE_STEP_H

#include <QWidget>
#include <memory>

namespace Ui {
class GuideStepUi;
}

//class VideoWidget;

class GuideStep : public QWidget
{
    Q_OBJECT
public:
    enum GuideStepType {
        Image,
        Video
    };

    enum GuidePosition {
        First,
        Common,
        Last,
        Single
    };

    explicit GuideStep(uint8_t index, QString text, GuideStepType guide_type, QString resource, QWidget *parent = nullptr, GuidePosition pos = Common);
    ~GuideStep();

    GuideStepType guideType() const
    {
        return m_type;
    }

signals:
    /**
     * @brief goToNextStep
     * @param direction
     *  >0  go forward
     *  <0  go backward
     */
    void goToNextStep(int8_t direction);

public slots:
    void onParentClosed();

private slots:
    void onChangingSteps();

private:
    std::unique_ptr<Ui::GuideStepUi> m_ui;
    QString m_text;
    GuideStepType m_type;
    QString m_resource;
//    VideoWidget* m_video;
};

#endif // GUIDE_STEP_H
