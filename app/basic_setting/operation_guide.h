#ifndef OPERATION_GUIDE_H
#define OPERATION_GUIDE_H

#include <QDialog>
#include <memory>

namespace Ui {
class OperationGuideUi;
}

class GuideStep;

class OperationGuide : public QDialog
{
    Q_OBJECT
public:
    explicit OperationGuide(QWidget *parent = nullptr);
    ~OperationGuide();
    // QObject interface
    bool eventFilter(QObject *watched, QEvent *event) override;

    void setTitle(const QString &title);
    void initGuideSteps(std::vector<GuideStep *> steps);

signals:
    void dialogClosed();

private slots:
    void onStepChanged(int8_t direction);

    // QWidget interface
protected:
    void paintEvent(QPaintEvent *event) override;

private:
    std::unique_ptr<Ui::OperationGuideUi> m_ui;
};

#endif // OPERATION_GUIDE_H
