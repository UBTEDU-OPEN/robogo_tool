#include "mainwindow.h"
#include "log.h"
#include "setting.h"
#include "ucar_device.h"
#include "toast_manager.h"
#include <QApplication>
#include <QLoggingCategory>
#include <QSharedMemory>
#include <memory>
#include <QLockFile>
#include <QDir>
#include <QThread>
#include <QTranslator>

Q_LOGGING_CATEGORY(main_entry, "main");

int main(int argc, char *argv[])
{
//    QSharedMemory uniqueness_check("RoboGoTool");
//    if(uniqueness_check.attach())
//    {
//        return 0;
//    }
//    uniqueness_check.create(1);
   QString path = QDir::temp().absoluteFilePath("RoboGoTool.lock");
   QLockFile lockFile(path);

   if (!lockFile.tryLock(500))
   {
       qDebug() << "process is already in running";
       return 0;
   }

    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("Ubtech");
    QCoreApplication::setApplicationName("RoboGo Tool");
    Log::instance().init();
    qCDebug(main_entry()) << "Version: 1.5.6";
    auto &setting = Setting::instance();
    setting.init();
    setting.loadDefaultStyle();
    UCarDevice &imu_dev = UCarDevice::instance();
    imu_dev.start();
    imu_dev.enableDistributing(true);

    QString strLanguagePath = "/home/oneai/.config/robogo/";
//    QString locale = "en_US";//根据系统语言
//    QString locale = "zh_CN";//根据系统语言
    QString locale = QLocale::system().name();//根据系统语言



    QString strQmPath = strLanguagePath + "app_" + locale;
    QTranslator translator;

    translator.load(strQmPath);
    a.installTranslator(&translator);

    QString strQmPath_qtBase = strLanguagePath + "qt_base_" + locale;
    QTranslator translator_qt;

    translator_qt.load(strQmPath_qtBase);
    a.installTranslator(&translator_qt);

    auto w = std::make_shared<MainWindow>();
    ToastManager::instance().setRootWidget(w);

    w->show();
    qCDebug(main_entry()) << "show mainwindow finish";
    auto ret = a.exec();

    ToastManager::instance().release();
//    uniqueness_check.detach();

    return ret;
}
