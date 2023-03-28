#include "advanced_stack.h"
#include "ui_advanced_stack.h"
#include "ucar_device.h"

enum emWidgetIndex
{
    eWidgetIndexAdvanceSetting = 0,
    WidgetIndexOtaWidget = 1
};

Advanced_Stack::Advanced_Stack(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Advanced_Stack)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(eWidgetIndexAdvanceSetting);
//    connect(this, &Advanced_Stack::UpdateOtaModal, this, &otawidget::updateModalState);
    connect(ui->page_set, &AdvancedSetting::sigChangeOtawidget, ui->page_ota, &otawidget::onDoCheckOta);
    connect(ui->page_ota, &otawidget::sigReturnSetting, this, &Advanced_Stack::onChangeAdvancedSet);
    connect(this, &Advanced_Stack::sigCheckOta, ui->page_ota, &otawidget::onDoCheckOta);
    connect(ui->page_ota, &otawidget::sigShowOtaWidget, this, &Advanced_Stack::onChangeOtawidget);
    connect(ui->page_ota, &otawidget::setOtaWindowModal,
            this, &Advanced_Stack::setOtaWindowModal);
//    ui->page_ota->setParent(ui->stackedWidget);
//    ui->page_set->setParent(ui->stackedWidget);
}

Advanced_Stack::~Advanced_Stack()
{
    delete ui;
}

void Advanced_Stack::onChangeOtawidget(bool bNeedCheck)
{
    //tab切到高级设置页
    emit sigAdvancedStackTab();
    ui->stackedWidget->setCurrentIndex(WidgetIndexOtaWidget);
    UCarDevice::instance().enableDistributing(false);
    //检查升级，获取是否有最新版本
    if (bNeedCheck)
    {
         emit sigCheckOta(bNeedCheck);
    }
}

void Advanced_Stack::onChangeAdvancedSet()
{
    ui->stackedWidget->setCurrentIndex(eWidgetIndexAdvanceSetting);
}
