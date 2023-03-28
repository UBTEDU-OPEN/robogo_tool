#include "otawidget.h"
#include "ui_otawidget.h"
#include <QtNetwork>
#include "ubtserverconfig.h"
#include "ota_aibox_guide.h"
#include "ota_ucar_guide.h"
#include "ucar_device.h"
#include <QFile>
#include <thread>
#include "ucar_device.h"
#include "connection_guidelines.h"

#include <QLoggingCategory>

Q_LOGGING_CATEGORY(ota_widget, "ota_widget")

namespace {
const uint16_t PACKAGE_SIZE{128};
}

//const QString g_sServer= "https://test79.ubtrobot.com"; //测试环境
const QString g_sServer= "https://oms.ubtrobot.com"; //正式环境
otawidget::otawidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::otawidget),
    m_current_pkg_num(1)
{
    m_pHttpTrans = new HttpTransfer();
    connect(m_pHttpTrans, &HttpTransfer::sigOtaResult, this, &otawidget::onOtaResult);
    m_pHttpDownload = new HttpDownload();
    connect(m_pHttpDownload, &HttpDownload::SigDownloadProgress, this, &otawidget::OnDownloadProgress);
    connect(m_pHttpDownload, &HttpDownload::SigFinishedDownload, this, &otawidget::OnFinishedDownload);


    connect(&m_timer, &QTimer::timeout, this, &otawidget::onOtaUpdateTimeout);
    m_timer.setInterval(OTA_UPDATE_TIMEOUT_CALC);

//    connect(&m_sending_timer, &QTimer::timeout, this, &otawidget::onTimeoutForSending);
//    m_sending_timer.setInterval(50);

    qRegisterMetaType<emConnectType>("emConnectType");
    connect(this, &otawidget::sigTimeoutForSending, &m_sender, &otaFileSending::onTimeoutForSending);
    connect(&m_sender, &otaFileSending::sigSendingFinished, this, &otawidget::onSendingFinished);
    connect(&m_sender, &otaFileSending::sigSetOtaProgessProcess, this, &otawidget::SetOtaProgessProcess);
    connect(&m_sender, &otaFileSending::sigUpgradeError, this, &otawidget::UpdateFailed);


    ui->setupUi(this);
    ui->pbDoUpdate->setEnabled(false);
    ui->progressBarOta->setRange(0,100);
    SetOtaProgessProcess(0);
    ui->progressBarOta->hide();
    ui->lb_process->hide();
    m_nTimerCalc = 0;
    m_bFirstCheckOta = true;
//    ui->tE_versionInfo->setLineWidth(40);

    QTextBlockFormat blockFormat;
    blockFormat.setLineHeight(20, QTextBlockFormat::LineDistanceHeight);
    auto textCursor = ui->tE_versionInfo->textCursor();
    textCursor.setBlockFormat(blockFormat);
    ui->tE_versionInfo->setTextCursor(textCursor);

//    ui->tE_versionInfo->setVerticalScrollBarPolicy();
}

int otawidget::QueryOTAVersion(const QString &strProductName, const QString strLanguage, const QString &strGroupVersion, bool bHomePageCheck)
{
    QString sRequestURL;
    //    https://test79.ubtrobot.com/v1/upgrade-rest/version/upgradable?productName=ufinder&moduleNames=web&versionNames=v1.0.0.1&languageName=en
    sRequestURL.clear();

    sRequestURL = g_sServer + "/v2/" + "upgrade-rest/group/upgradable?productName=" + strProductName + "&languageName=" + strLanguage + "&groupVersion=" + strGroupVersion;
    qDebug("FileSynchronize::QueryOTAVersion sRequestURL = %s\n", sRequestURL.toUtf8().data());

    return m_pHttpTrans->getQueryOTAVersion(sRequestURL, HTTP_DEFAULT_REQUEST_ID,  bHomePageCheck); //临时请求ID
}

bool otawidget::checkIsOnline()
{
    bool bHasAddress  = false;
    foreach(const QHostAddress& hostAddress, QNetworkInterface::allAddresses())
        if ( hostAddress != QHostAddress::LocalHost && !isLinkLocalAddress(hostAddress))//过滤本机ip、私有ip
        {
            quint32 ip = hostAddress.toIPv4Address();
            if (ip)
            {
                QString strAddress = QString("%1.%2.%3.%4")
                        .arg((ip >> 24) & 0xFF)
                        .arg((ip >> 16) & 0xFF)
                        .arg((ip >> 8) & 0xFF)
                        .arg(ip & 0xFF);
                if (!strAddress.isEmpty())
                {
                    QStringList ipList = strAddress.split('.',QString::SkipEmptyParts);
                    if (ipList.size() == 4)
                    {
                        QString strLastIp = ipList[3];
                        if (strLastIp.toInt() != 1)//过滤网关ip，忽略
                        {
                            bHasAddress = true;
                            break;
                        }
                    }
                }
            }
        }
    return bHasAddress;
}


bool otawidget::isLinkLocalAddress(QHostAddress addr)
{
    quint32 nIPv4 = addr.toIPv4Address();
    quint32 nMinRange = QHostAddress("169.254.1.0").toIPv4Address();
    quint32 nMaxRange = QHostAddress("169.254.254.255").toIPv4Address();
    if ((nIPv4 >= nMinRange) && (nIPv4 <= nMaxRange)) {
        return true;
    } else {
        return false;
    }
}

void otawidget::onDoCheckOta(bool bHomePageCheck)
{
    QString strCurVersion = "";
    if (bHomePageCheck && !m_bFirstCheckOta)
    {
        return;
    }
    UCarDevice::instance().getSoftwareVersion(strCurVersion);

    if (!strCurVersion.isEmpty() && strCurVersion == "error")
    {
        UCarDevice::instance().cancelUpgrading();
        UCarDevice::instance().getSoftwareVersion(strCurVersion);
    }

    if (!bHomePageCheck && strCurVersion.isEmpty())//手动检查升级流程，需要提示地盘未连接
    {
        OtaConnectGuide connectGuide(this, eConnectTypeUcar);
        connectGuide.exec();
        qDebug("onDoCheckOta connect ucar failed\n");
        return;
    }

    if (!bHomePageCheck && !checkIsOnline()) //手动检查升级流程，需要提示网络未链接
    {
        //showwindow
        OtaConnectGuide connectGuide(this, eConnectTypeNetwork);
        connectGuide.exec();
        qDebug("onDoCheckOta checkIsOnline failed\n");
        return;
    }
    if (!bHomePageCheck)
    {
         emit sigShowOtaWidget(false);
    }
    //初始化状态
    strCurVersion.remove(QChar('V'), Qt::CaseInsensitive);
    ui->lb_curVersion->setText(strCurVersion);
    m_strLocalUcarVersion = strCurVersion;
    SetOtaProgessProcess(0);
    ui->progressBarOta->hide();
    ui->lb_process->hide();
    m_strUrl = "";
    m_strMd5 = "";
    m_strNewAiboxVersion = "";
    ui->tE_versionInfo->clear();
//    m_strReleaseNote = "";
//    if (ui->pbDoUpdate->isHidden())
//    {
//        ui->pbDoUpdate->show();
//    }
    //获取本地版本并显示
    QString locale = QLocale::system().name();
    QueryOTAVersion(OTA_PRODUCT_NAME_ROBOTGO, locale, m_strLocalUcarVersion, bHomePageCheck);//test
}

void otawidget::onOtaResult(bool bSuccess, const OTAVersionInfo &otaData, bool bHomePageCheck)
{
    if (!bSuccess)
    {
        if (!bHomePageCheck)//非首页检查，需要提示网络错误
        {
            OtaConnectGuide connectGuide(this, eConnectTypeNetwork);
            connectGuide.exec();
            qDebug("otawidget onOtaResult failed\n");
        }
        return;

    }
    else
    {
        QString strLocalUcarVersion = m_strLocalUcarVersion; //从底盘获取ucar版本
        QString strUcarVerson = otaData.ucarVersion;//版本要删除最后一位0;
        strUcarVerson = strUcarVerson.left(strUcarVerson.lastIndexOf("."));
        strUcarVerson = strUcarVerson.replace(" ", "");//去掉空格
        QString strAiboxVersion = otaData.AiboxVersion;
        m_strNewAiboxVersion = strAiboxVersion;
        if (strUcarVerson.isEmpty() || strAiboxVersion.isEmpty())
        {
            //版本过高，忽略
            //            ui->lb_newVersion->setText(strUcarVerson);
            ui->lb_CurVersionTitle->setText(tr("当前为最新版本："));
            ui->lb_newVersion->hide();
            ui->lb_newVersionTitle->hide();
            ui->tE_versionInfo->insertPlainText(otaData.releaseNote);
            ui->lbReleaseNote->setText(tr("版本说明："));
            ui->pbDoUpdate->hide();
        }
        else
        {
            if (UCarDevice::instance().CompareVersion(strLocalUcarVersion, strUcarVerson) < 0)
            {
                //uCar版本过低，提示升级ucar
                ui->lb_CurVersionTitle->setText(tr("当前版本："));
                ui->lb_newVersion->show();
                ui->lb_newVersion->setText(strUcarVerson);
                ui->lb_newVersionTitle->show();
                ui->tE_versionInfo->show();
                ui->lbReleaseNote->show();
                ui->lbReleaseNote->setText(tr("最新版本说明："));
                ui->pbDoUpdate->show();
                ui->tE_versionInfo->insertPlainText(otaData.releaseNote);
                if (bHomePageCheck)
                {
                    if (m_bFirstCheckOta)
                    {
                        m_bFirstCheckOta = false;//首页只弹一次
                    }
                    bool bImportant = otaData.isForced;
                    OtaUcarGuide otaUcarguid(bImportant, this);
                    if(otaUcarguid.exec() == QDialog::Rejected)
                    {
                        //返回首页，无需处理
                        return;
                    }
                    else
                    {
                         emit sigShowOtaWidget(false);
                    }
                }
                ui->pbDoUpdate->setEnabled(true);
                m_strUrl = otaData.packageUrl;
                m_strMd5 = otaData.packageMd5;
            }
            else
            {
                ui->lb_CurVersionTitle->setText(tr("当前为最新版本："));
                ui->lb_newVersion->hide();
                ui->lb_newVersionTitle->hide();
                ui->tE_versionInfo->setText(otaData.releaseNote);
                ui->lbReleaseNote->setText(tr("版本说明："));
                ui->pbDoUpdate->hide();
                UBTServerConfig config;
                QString strLocalAiboxVersion =config.getAiboxVersion().c_str();
                if (UCarDevice::instance().CompareVersion(strLocalAiboxVersion, strAiboxVersion) < 0)
                {
                    //aibox版本过低，提示升级aibox
                    if (bHomePageCheck && m_bFirstCheckOta)
                    {
                        m_bFirstCheckOta = false;
                        OtaAiboxGuide otaGuide(strLocalAiboxVersion, strAiboxVersion, this);
                        otaGuide.exec();
                    }

                }
            }

        }
    }
}

void otawidget::OnDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
//    int nValue = bytesTotal*50/bytesTotal;
////    ui->progressBarOta->setValue(nValue);
//    SetOtaProgessProcess(nValue);
}

void otawidget::OnFinishedDownload(bool bsuccess)
{
    qDebug()<< "enter" << __func__;

    bool bFileSuccess = bsuccess;
    if (bsuccess)
    {
        //        std::thread ([this]()
        //        {
        SetOtaProgessProcess(1);
        if(handleVerifyMd5(OTA_DOWNLOAD_FILE_PATH, m_strMd5))
        {
            qDebug()<<"handleVerifyMd5 success" ;
            //启动定时器查看
            if (m_timer.isActive())
            {
                m_timer.stop();
            }
            if (doBurning())
            {
//                m_timer.start(); //每3s轮询一次版本，看是否是否升级成功
                ui->pbDoUpdate->setEnabled(false);
                ui->pb_return->setEnabled(false);
            }
            else
            {
                UpdateFailed(eOtaUpdateFailedTips);
                return;
            }

        }
        else
        {
            bFileSuccess = false;
        }
    }
    if (!bFileSuccess)
    {
        UpdateFailed(eConnectTypeNetwork);
    }

}

void otawidget::onOtaUpdateTimeout()
{
    qCDebug(ota_widget())<< "enter"<< __func__;
    QString strCurVersion = "";
    UCarDevice::instance().getSoftwareVersion(strCurVersion);
    strCurVersion.remove(QChar('V'), Qt::CaseInsensitive);
    qCDebug(ota_widget())<< "onOtaUpdateTimeout"<< strCurVersion << "newVersion" << ui->lb_newVersion->text();
    if(!strCurVersion.isEmpty())//查询ucar版本
    {
        m_nTimerCalc = 0;
        if (strCurVersion == ui->lb_newVersion->text())
        {
            //升级成功
//            ui->progressBarOta->setValue(100);
            SetOtaProgessProcess(100);
            emit setOtaWindowModal(false);
            ui->pbDoUpdate->setEnabled(false);
            ui->pb_return->setEnabled(true);
//            //版本页面更新
//            ui->lb_CurVersionTitle->setText("当前为最新版本");
//            ui->lb_newVersion->hide();
//            ui->lb_newVersionTitle->hide();
//            ui->tE_versionInfo->hide();
            ui->pbDoUpdate->hide();
//            ui->lbReleaseNote->setText("版本说明");
            ui->lb_curVersion->setText(strCurVersion);
            m_timer.stop();

            UBTServerConfig config;
            QString strLocalAiboxVersion =config.getAiboxVersion().c_str();
            if (UCarDevice::instance().CompareVersion(strLocalAiboxVersion, m_strNewAiboxVersion) < 0)
            {
                //aibox版本过低，提示升级aibox

                OtaAiboxGuide otaGuide(strLocalAiboxVersion, m_strNewAiboxVersion, this);
                otaGuide.exec();
            }
        }
        else
        {
            UpdateFailed(eOtaUpdateFailedTips);
        }
        return;

    }
    m_nTimerCalc+=OTA_UPDATE_TIMEOUT_CALC;
    if (m_nTimerCalc >= OTA_UPDATE_TIMEOUT * 1000)
    {
        UpdateFailed(eOtaUpdateFailedTips);
    }
}

bool otawidget::doBurning()
{
    //调用升级接口
    qCInfo(ota_widget()) << __func__;
    m_ota_file.setFileName(OTA_DOWNLOAD_FILE_PATH);

    bool ret = m_ota_file.open(QIODevice::ReadOnly);
    if (!ret)
    {
        qCInfo(ota_widget()) << "Failed to open file";
        return false ;
    }

    if (m_strMd5.size() < 32)
    {
        qCInfo(ota_widget()) << "md5 too short";
        return  false;
    }

    ret = UCarDevice::instance().enterUpgrading();
    if (!ret)
    {
        qCInfo(ota_widget()) << "Failed to enter upgrade mode";
        return false;
    }

    auto file_size = m_ota_file.size();
    m_amount = file_size / PACKAGE_SIZE + (file_size % PACKAGE_SIZE > 0 ? 1 : 0);

    ret = UCarDevice::instance().startUpgrading(m_amount);
    if (!ret)
    {
        qCInfo(ota_widget()) << "Failed to start upgrade";
        return false;
    }
    m_current_pkg_num = 1;

    emit sigTimeoutForSending(&m_ota_file, m_amount);
//    m_sending_timer.start();

    return true;
}

otawidget::~otawidget()
{
    delete ui;
}

void otawidget::onPBReturnClicked()
{
    emit sigReturnSetting();
}

void otawidget::on_pb_return_clicked()
{
    emit sigReturnSetting();
}

void otawidget::on_pbDoUpdate_clicked()
{
    ui->pbDoUpdate->setEnabled(false);
    ui->pb_return->setEnabled(false);
    ui->progressBarOta->show();
    ui->lb_process->show();
    //禁用tab
    emit setOtaWindowModal(true);
    QFile::remove(OTA_DOWNLOAD_FILE_PATH);

    //下载文件
   m_pHttpDownload->DownloadOTAVersion(m_strUrl, HTTPDownloadType);

//    doBurning();
}

/**************************************************************************
* 函数名: handleVerifyMd5
* 功能:
* 参数:strMd5Http: md5 from http
* 返回值:  bool
* 时间: 2017/01/07
* 作者: Joker
*/
bool otawidget::handleVerifyMd5(const QString &strFilePath, const QString &strMd5)
{
    //    qCDebug(server()) << "orignal md5sum: " << md5sumOrignal;
    bool bRet = false;
    QString fullPath = strFilePath;
    QString cmd = "md5sum " + fullPath;

    qDebug() << "cmd " << cmd;
    FILE *fp = popen(cmd.toStdString().data(), "r");
    if (fp)
    {
        char md5sumBuf[128] = {0};
        fread(md5sumBuf, 1, sizeof(md5sumBuf), fp);

        pclose(fp);

        QString md5sumNew = QString(md5sumBuf);
        int pos = md5sumNew.indexOf(" ");
        md5sumNew = md5sumNew.mid(0, pos);

        qDebug() << "new md5sum: " << md5sumNew;

        if (!strMd5.compare(QString(md5sumNew)))
        {
            qDebug() << "md5sum success";
            bRet =  true;
        }
    }
    return bRet;
}

void otawidget::UpdateFailed(emConnectType nType)
{
    qDebug()<<"enter"<<__func__;
    OtaConnectGuide connectGuide(this, nType);
    connectGuide.exec();
    qDebug("otawidget UpdateFailed failed\n");
    m_nTimerCalc = 0;
    SetOtaProgessProcess(0);
    emit setOtaWindowModal(false);
    ui->pb_return->setEnabled(true);
    if (m_timer.isActive())
    {
        m_timer.stop();
    }
    //检查小车是否链接
    QString strCurVersion = "";
    UCarDevice::instance().getSoftwareVersion(strCurVersion);
    strCurVersion.remove(QChar('V'), Qt::CaseInsensitive);
    if(strCurVersion.isEmpty())//查询ucar版本判断小车是否在线
    {
        if (nType == eOtaUpdateFailedTips)
        {
            ConnectionGuidelines guide(this);
            guide.exec();//升级失败后，弹出提示
        }
        ui->pbDoUpdate->setEnabled(false); //小车未连接，升级按钮置灰
    }
    else
    {
        ui->pbDoUpdate->setEnabled(true);
    }

    if (nType == eOtaUpdateFailedTips)
    {
        UCarDevice::instance().cancelUpgrading();
    }
}

void otawidget::SetOtaProgessProcess(int nProcess)
{
    qDebug() << "enter" << __func__ << nProcess;
    ui->progressBarOta->setValue(nProcess);
    int nWidth = ui->wt_Process->width();
    int nProcessWidth = ui->lb_process->width() - 12;
//    ui->hs_progress_left->changeSize(nWidth/ 100 * 50, 20, QSizePolicy::Fixed, QSizePolicy::Fixed);
//    ui->hs_progress_right->changeSize(nWidth - nWidth/ 100 * 50 - nProcessWidth, 20, QSizePolicy::Fixed, QSizePolicy::Fixed);
    QString strValue = QString::number(nProcess)+ "%";
    ui->lb_process->setText(strValue);
//    this->update();

    int nIncress = -10 + nProcess/3;

    if (nProcess < 100)
    {
        ui->lb_process->move((nWidth - nProcessWidth)/ 100 * nProcess + nProcessWidth + nIncress,  2);
    }
    else
    {
        ui->lb_process->move((nWidth - nProcessWidth)/ 100 * nProcess + nProcessWidth + nIncress - 5,  2);
    }


}

#if 0
void otawidget::onTimeoutForSending()
{
    bool bRun= true;
    do
    {
         uint8_t error_count = 0;
        char buf[PACKAGE_SIZE];
        if(m_current_pkg_num <= m_amount)
        {
            auto bytes_read = m_ota_file.read(buf, PACKAGE_SIZE);
            qCDebug(ota_widget()) << "read " << bytes_read << " bytes from file";
            std::vector<uint8_t> data_vec(buf, buf+bytes_read);

            bool ret = UCarDevice::instance().sendUpgradingData(m_current_pkg_num, data_vec);
            if (!ret)
            {
                qCInfo(ota_widget()) << "send package error " << m_current_pkg_num ;
                if (error_count > 1)
                {
                    qCInfo(ota_widget()) << "Failed to send package " << m_current_pkg_num;
//                    if (m_sending_timer.isActive())
//                    m_sending_timer.stop();
                    UpdateFailed(eOtaUpdateFailedTips);
                    bRun = false;
                    break ;
                }

                error_count++;
                m_ota_file.seek(PACKAGE_SIZE * (m_current_pkg_num - 1));
                return;
            }
            else
            {
                error_count = 0;
                SetOtaProgessProcess(int(1 + m_current_pkg_num*1.0/m_amount * 98));
                qCDebug(ota_widget()) << "current pkg num : " << m_current_pkg_num;
            }
        }
        else
        {
//            if (m_sending_timer.isActive())
//            m_sending_timer.stop();
            onSendingFinished();
            bRun = false;
            break ;
        }
        m_current_pkg_num++;
    }while(bRun);


}
#endif

void otawidget::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    QWidget::paintEvent(event);
}

void otawidget::onSendingFinished()
{
    qCInfo(ota_widget()) << "enter" << __func__;
    bool ret = UCarDevice::instance().finishUpgrading(m_strMd5);
    if (!ret)
    {
        qCInfo(ota_widget()) << "Md5 not match";
//        UpdateFailed(eOtaUpdateFailedTips);
//        return ;
        //此处存在地盘md5校验超时，但是仍然能正常升级的情况，这里先不提示，统一等超时处理
    }
//    SetOtaProgessProcess(99);
    //启动定时器查看
    if (m_timer.isActive())
    {
        m_timer.stop();
    }
    m_timer.start(); //每3s轮询一次版本，看是否是否升级成功
    ui->pbDoUpdate->setEnabled(false);
    ui->pb_return->setEnabled(false);
    qCInfo(ota_widget()) << "onSendingFinished end";
    //提示升级失败

}

otaFileSending::otaFileSending()
{
   moveToThread(&m_oThread);
   m_oThread.start();
}

otaFileSending::~otaFileSending()
{
    m_oThread.quit();
    m_oThread.wait();
}

void otaFileSending::onTimeoutForSending(QFile *ota_file, int nAmount)
{
    if (!ota_file)
    {
        return;
    }
    bool bRun= true;
    uint8_t error_count = 0;
    int current_pkg_num = 1;
    do
    {
        char buf[PACKAGE_SIZE];
        if(current_pkg_num <= nAmount)
        {
            auto bytes_read = ota_file->read(buf, PACKAGE_SIZE);
            qCDebug(ota_widget()) << "read " << bytes_read << " bytes from file";
            std::vector<uint8_t> data_vec(buf, buf+bytes_read);

            bool ret = UCarDevice::instance().sendUpgradingData(current_pkg_num, data_vec);
            if (!ret)
            {
                qCInfo(ota_widget()) << "send package error " << current_pkg_num ;
                if (error_count > 1)
                {
                    qCInfo(ota_widget()) << "Failed to send package " << current_pkg_num;
//                    if (m_sending_timer.isActive())
//                    m_sending_timer.stop();
                    emit sigUpgradeError(eOtaUpdateFailedTips);
                    bRun = false;
                    break ;
                }

                error_count++;
                ota_file->seek(PACKAGE_SIZE * (current_pkg_num - 1));
                continue;
            }
            else
            {
                error_count = 0;
                emit sigSetOtaProgessProcess(int(1 + current_pkg_num*1.0/nAmount * 98));
                qCDebug(ota_widget()) << "current pkg num : " << current_pkg_num;
            }
        }
        else
        {
            emit sigSendingFinished();
            bRun = false;
            break ;
        }
        current_pkg_num++;
    }while(bRun);
}
