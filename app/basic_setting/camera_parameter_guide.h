#ifndef CAMERA_PARAMETER_GUIDE_H
#define CAMERA_PARAMETER_GUIDE_H

#include <QDialog>
#include <memory>

namespace Ui {
    class CameraParameterGuideUi;
}

class CameraParameterGuide : public QDialog
{
    Q_OBJECT
public:
    explicit CameraParameterGuide(QWidget *parent = nullptr);
    ~CameraParameterGuide();

protected:
    void paintEvent(QPaintEvent *);

private:
    std::unique_ptr<Ui::CameraParameterGuideUi> m_ui;
};

#endif // CAMERA_PARAMETER_GUIDE_H
