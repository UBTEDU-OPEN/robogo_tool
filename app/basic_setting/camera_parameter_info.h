#ifndef CAMERA_PARAMETER_INFO_H
#define CAMERA_PARAMETER_INFO_H


#include <QDialog>
#include <memory>

namespace Ui {
    class CameraParameterInfoUi;
}


class CameraParameterInfo : public QDialog
{
    Q_OBJECT
public:
    explicit CameraParameterInfo(QWidget *parent = nullptr);
    ~CameraParameterInfo();

    void setMessage(const QString &message);

protected:
    void paintEvent(QPaintEvent *);

private:
    std::unique_ptr<Ui::CameraParameterInfoUi> m_ui;
};

#endif // CAMERA_PARAMETER_INFO_H
