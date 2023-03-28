#include "camera_parameter_info.h"
#include "ui_camera_parameter_info.h"
#include "setting.h"
#include <QStyleOption>
#include <QPainter>
#include <QBitmap>

CameraParameterInfo::CameraParameterInfo(QWidget *parent)
    : QDialog(parent),
      m_ui(new Ui::CameraParameterInfoUi)
{
    m_ui->setupUi(this);
    Qt::WindowFlags flags = Qt::FramelessWindowHint | Qt::Dialog;
    this->setWindowFlags(flags);

    connect(m_ui->pb_close, &QPushButton::released,
            this, [this](){
        this->close();
     }
    );

    connect(m_ui->pb_ok, &QPushButton::released,
            this, [this](){
        this->close();
     }
    );

    if(parent)
    {
        QPoint global_pos = parent->mapToGlobal(QPoint(0, 0));
        auto parent_size = parent->size();
        auto my_size = this->size();
        move(global_pos.x() + (parent->width()-my_size.width())/2, global_pos.y() + (parent_size.height()-my_size.height())/2);
    }

    QString text;
    text.append(QString("<p>camera matrix:</p>"));
    QList<double> matrix_list = Setting::instance().getMatrix();
    if (matrix_list.size() >= 9)
    {
        for (auto i = 0; i < 3; ++i)
        {
            text.append(QString("<p>[%1  %2  %3]</p>").arg(
                            QString::number(matrix_list[i*3]), QString::number(matrix_list[i*3 + 1]), QString::number(matrix_list[i*3 + 2])));
        }
    }
    QList<double> coef_list = Setting::instance().getCoef();
    if (coef_list.size() >= 5)
    {
        text.append(QString("<p>distortion coefficients: [%1  %2  %3  %4  %5]</p>").arg(
                        QString::number(coef_list[0]), QString::number(coef_list[1]), QString::number(coef_list[2]),
                QString::number(coef_list[3]), QString::number(coef_list[4])));
    }
    setMessage(text);
}

CameraParameterInfo::~CameraParameterInfo()
{
}

void CameraParameterInfo::setMessage(const QString &message)
{
    m_ui->tb_info_browser->setText(message);
}

void CameraParameterInfo::paintEvent(QPaintEvent *)
{
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
