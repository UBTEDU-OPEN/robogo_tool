#include "rollback.h"
#include "ui_rollback.h"
#include "guide_rollback.h"
#include <QLoggingCategory>
#include <QSettings>
#include <QTimer>
#include <thread>
#include "ucar_device.h"
#include "guide_disconnect.h"
#include "toast_manager.h"
#include "guide_version.h"

enum emRollbackType
{
    eRollbackLeft = 1,
    eRollBackRight
};

#define DRIVE_TEST_TIME 2000 //直线测试时间，2s
#define TURN_AND_BACK_TIME 1000 //转向和回中时间1s
#define DEFAULT_TURN_ANGLE 15 //回中校准默认调15度
#define MIN_UCAR_OFFSET   0// 最小回中偏转角
#define MAX_UCAR_OFFSET   15// 最大回中偏转角
#define UCAR_MIN_VERSION "V5.8"

Q_LOGGING_CATEGORY(rollbackinfo, "rollbackinfo")

rollback::rollback(QWidget *parent) :
    ui(new Ui::rollback)
    , m_view(parent)
    , m_nCurType(eRollbackLeft)
    , m_nLeftDefault(0)
    , m_nRightDefault(0)
    , m_bIslineTest(false)
{
    ui->setupUi(this);
    connect(ui->pb_back, &QPushButton::released,
            this, &rollback::onBackButtonClicked);
    connect(ui->pb_guide, &QPushButton::released,
            this, &rollback::onGuideButtonClicked);
    connect(ui->pb_left, &QPushButton::released,
            this, &rollback::onLeftClicked);
    connect(ui->pb_right, &QPushButton::released,
            this, &rollback::onRightClicked);

    connect(ui->pb_reduce, &QPushButton::released,
            this, &rollback::onReduceClicked);
    connect(ui->pb_add, &QPushButton::released,
            this, &rollback::onAddClicked);

    connect(ui->pb_line_test, &QPushButton::released,
            this, &rollback::onDriveTestClicked);

    connect(ui->pb_cancel, &QPushButton::released,
            this, &rollback::onCancelAdjust);
    ui->pb_cancel->setEnabled(false);
    setRollbackStype(m_nCurType);
    QTimer::singleShot(100, this, &rollback::onCheckUcarVersion);
}

rollback::~rollback()
{
    if (m_bIslineTest)
    {
        UCarDevice::instance().driveStop();//执行一次停止，防止直线行驶不停止
        UCarDevice::instance().servoComebackCenter();//执行一次回中，防止角度转了回不来
    }
    delete ui;
}

void rollback::onGuideButtonClicked()
{
    RollbackGuidelines guide(m_view);
    guide.exec();
}


void rollback::onBackButtonClicked()
{
    qCInfo(rollbackinfo()) << __func__;
    if (m_bIslineTest)
    {
        UCarDevice::instance().driveStop();//执行一次停止，防止直线行驶不停止
        UCarDevice::instance().servoComebackCenter();//执行一次回中，防止角度转了回不来
    }

    emit backToPreviousView();

}

void rollback::onLeftClicked()
{
    if (onCheckUcarVersion())
    {
        return;
    }
     ui->pb_cancel->setEnabled(false);
    if(!setRollbackStype(eRollbackLeft))
    {
        showdisconnectDlg();
    }
//    bool bRet = false;
//    do
//    {
//        bRet = UCarDevice::instance().driveLeftDirection(DEFAULT_TURN_ANGLE);
//        if (!bRet)
//        {
//            break;
//        }
//        else
//        {
//            QTimer::singleShot(TURN_AND_BACK_TIME, this, &rollback::onComebackCenter);
//        }

//    }while(0);
//    if (!bRet)
//    {
//        showdisconnectDlg();
//        ui->pb_left->setEnabled(true);
//    }
//    else
//    {
//        setRollbackStype(eRollbackLeft);
//    }

}

void rollback::onRightClicked()
{
    if (onCheckUcarVersion())
    {
        return;
    }
    ui->pb_cancel->setEnabled(false);
    if(!setRollbackStype(eRollBackRight))
    {
        showdisconnectDlg();
    }

//    bool bRet = false;
//    do
//    {
//        bRet = UCarDevice::instance().driveRightDirection(DEFAULT_TURN_ANGLE);
//        if (!bRet)
//        {
//            break;
//        }
//        else
//        {
//            QTimer::singleShot(TURN_AND_BACK_TIME, this, &rollback::onComebackCenter);
//        }

//    }while(0);
//    if (!bRet)
//    {
//        showdisconnectDlg();
//        ui->pb_right->setEnabled(true);
//    }
//    else
//    {
//         setRollbackStype(eRollBackRight);
//    }

}

void rollback::onReduceClicked()
{
    if (onCheckUcarVersion())
    {
        return;
    }
    ui->pb_reduce->setEnabled(false);
    int nAdjustGet = 0;
    bool bRet = false;
    do
    {
        bRet = UCarDevice::instance().getServoAdjust(m_nCurType, nAdjustGet);//读取当前校准值
        if (!bRet)
        {
            break;
        }
        if (nAdjustGet <= MIN_UCAR_OFFSET)
        {
            ToastManager::instance().show(tr("已到最小设置角度！"));
            break;
        }
        bRet = UCarDevice::instance().setServoAdjust(m_nCurType, --nAdjustGet);//减1
        if (!bRet)
        {
            break;
        }
        ui->label_speed->setText(QString::number(nAdjustGet));

    }while(0);
    if (!bRet)
    {
         showdisconnectDlg();
    }
    else
    {
         if (!ui->pb_cancel->isEnabled())
         ui->pb_cancel->setEnabled(true);
    }
    ui->pb_reduce->setEnabled(true);
}

void rollback::onAddClicked()
{
    if (onCheckUcarVersion())
    {
        return;
    }
    ui->pb_add->setEnabled(false);
    int nAdjustGet = 0;
    bool bRet = false;
    do
    {
        bRet = UCarDevice::instance().getServoAdjust(m_nCurType, nAdjustGet);//读取当前校准值
        if (!bRet)
        {
            break;
        }
        if (nAdjustGet >= MAX_UCAR_OFFSET)
        {
            ToastManager::instance().show(tr("已到最大设置角度！"));
            break;
        }
        bRet = UCarDevice::instance().setServoAdjust(m_nCurType, ++nAdjustGet);// 加1
        if (!bRet)
        {
            break;
        }
        ui->label_speed->setText(QString::number(nAdjustGet));
    }while(0);
    if (!bRet)
    {
         showdisconnectDlg();
    }
    else
    {
        if (!ui->pb_cancel->isEnabled())
        ui->pb_cancel->setEnabled(true);
    }
    ui->pb_add->setEnabled(true);
}

void rollback::onDriveTestClicked()
{
    if (onCheckUcarVersion())
    {
        return;
    }
    ui->pb_line_test->setEnabled(false);
    m_bIslineTest = true;
    bool bRet = false;
    do
    {
        if (m_nCurType == eRollbackLeft)
        {
            bRet = UCarDevice::instance().driveLeftDirection(DEFAULT_TURN_ANGLE);
        }
        else
        {
            bRet = UCarDevice::instance().driveRightDirection(DEFAULT_TURN_ANGLE);
        }

        if (!bRet)
        {
            break;
        }
        else
        {
            QTimer::singleShot(TURN_AND_BACK_TIME, this, &rollback::onComebackCenter);
        }

    }while(0);
    if (!bRet)
    {
        DriveTestFail();
    }
}

void rollback::onCancelAdjust()
{
    if (onCheckUcarVersion())
    {
        return;
    }
    int nDefaultOffset =  getDefaultOffset(m_nCurType);
    bool bRet = UCarDevice::instance().setServoAdjust(m_nCurType, nDefaultOffset);
    if (bRet)
    {
        ui->label_speed->setText(QString::number(nDefaultOffset));
        ui->pb_cancel->setEnabled(false);
    }
    else
    {
        showdisconnectDlg();
    }
}

void rollback::onDriveFoward()
{
    if (onCheckUcarVersion())
    {
        return;
    }
    bool bRet = false;
    do
    {
        int nVelocity = 0;
        bRet = UCarDevice::instance().getVelCaliData(nVelocity);
        if (!bRet)
        {
            break;
        }
        if (bRet && (nVelocity <= 0))
        {
            nVelocity = UCAR_DEFAULT_VELOCITY;
        }

        UCarDevice::instance().driveForward(nVelocity);
        QTimer::singleShot(DRIVE_TEST_TIME, this, &rollback::onDriveStop);
    }while(0);
    if (!bRet)
    {
        DriveTestFail();
    }
}

void rollback::onDriveStop()
{
    if (onCheckUcarVersion())
    {
        return;
    }
     bool bRet = UCarDevice::instance().driveStop();
     if (!bRet)
     {
         DriveTestFail();
     }
     else
     {
         ui->pb_line_test->setEnabled(true);
     }

}

void rollback::onComebackCenter()
{
    if (onCheckUcarVersion())
    {
        return;
    }
    bool bRet = UCarDevice::instance().servoComebackCenter();
    if (!bRet)
    {
         DriveTestFail();
    }
    else
    {
        QTimer::singleShot(TURN_AND_BACK_TIME, this, &rollback::onDriveFoward);
    }
}

void rollback::onUcarDisconnet()
{
    showdisconnectDlg();
}

bool rollback::setDefaultOffset(int nDir)
{
    int nOffset = 0;
    bool bRet = UCarDevice::instance().getServoAdjust(nDir, nOffset);
    if (bRet)
    {
        if (nDir == eRollbackLeft)
        {
           m_nLeftDefault = nOffset;
        }
        else
        {
            m_nRightDefault = nOffset;
        }
        ui->label_speed->setText(QString::number(nOffset));
    }
    return bRet;
}

int rollback::getDefaultOffset(int nDir)
{
    if (nDir == eRollbackLeft)
    {
       return m_nLeftDefault;
    }
    else
    {
        return m_nRightDefault;
    }
}

bool rollback::setRollbackStype(int nType)
{
    if(setDefaultOffset(nType))
    {
        m_nCurType = nType;
        if (nType == eRollbackLeft)
        {
            ui->pb_left->setEnabled(false);
            ui->pb_right->setEnabled(true);
        }
        else
        {
            ui->pb_left->setEnabled(true);
            ui->pb_right->setEnabled(false);
        }
        return true;
    }
    else
    {
        return false;
    }
    //保持默认的校准值
}

void rollback::showdisconnectDlg()
{
    GuideDisconnect disconDlg(m_view);
    disconDlg.exec();
}

void rollback::DriveTestFail()
{
    showdisconnectDlg();
    m_bIslineTest = false;
    ui->pb_line_test->setEnabled(true);
}

bool rollback::onCheckUcarVersion()
{
    QString strCurVersion = "";
    UCarDevice::instance().getSoftwareVersion(strCurVersion);

    if (strCurVersion.isEmpty())
    {
        return false;
    }
    if (!strCurVersion.isEmpty() && strCurVersion == "error")
    {
        return false;
    }

    if (UCarDevice::instance().CompareVersion(strCurVersion, UCAR_MIN_VERSION) < 0)
    {
        //小车版本过低，不支持回中校准
        GuideUcarVersion guide(strCurVersion, m_view);
        guide.exec();
        return true;
    }
    return false;
}
