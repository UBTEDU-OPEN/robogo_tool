#include "basic_setting_home.h"
#include "ui_basic_setting_home.h"

BasicSettingHome::BasicSettingHome(QWidget *parent)
    : QWidget(parent),
      m_ui(new Ui::BasicSettingHomeUi)
{
    m_ui->setupUi(this);
    connect(m_ui->pb_acceleration, &QPushButton::released,
            this, &BasicSettingHome::accelButtonPressed);
    connect(m_ui->pb_magnetic_field, &QPushButton::released,
            this, &BasicSettingHome::magButtonPressed);
    connect(m_ui->pb_perspective, &QPushButton::released,
            this, &BasicSettingHome::perspectiveButtonPressed);
    connect(m_ui->pb_coef, &QPushButton::released,
            this, &BasicSettingHome::cameraParamBtnPressed);
    connect(m_ui->lb_show_coef, &QLabel::linkActivated,
            this, &BasicSettingHome::cameraParamInfoBtnPressed);
    connect(m_ui->pb_restore_coef, &QPushButton::released,
            this, &BasicSettingHome::cameraParamRestoreBtnPressed);
    connect(m_ui->lb_show_pic, &QLabel::linkActivated,
            this, &BasicSettingHome::cameraPicBrowserBtnPressed);
    connect(m_ui->pb_rollback, &QPushButton::released,
            this, &BasicSettingHome::rollbackButtonPressed);
}

BasicSettingHome::~BasicSettingHome()
{

}

void BasicSettingHome::showEvent(QShowEvent *event)
{
    emit aboutToShow();
    QWidget::showEvent(event);
}
