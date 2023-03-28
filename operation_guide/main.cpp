#include "mainwindow.h"
#include "setting.h"
#include <QApplication>
#include <QLoggingCategory>
#include <QSharedMemory>
#include <QTranslator>


Q_LOGGING_CATEGORY(main_entry, "main");

int main(int argc, char *argv[])
{
    QSharedMemory uniqueness_check("OperationGuide");
    if(uniqueness_check.attach())
    {
        return 0;
    }
    uniqueness_check.create(1);

    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("Ubtech");
    QCoreApplication::setApplicationName("Operation Guide");

    auto &setting = Setting::instance();
    setting.init();
    setting.loadDefaultStyle();

//    QString strQmPath = "./operation_en.qm";
    QString strLanguagePath = "/home/oneai/.config/robogo/";
    QString locale = QLocale::system().name();//根据系统语言
    QString strQmPath = strLanguagePath + "operation_" + locale;
    QTranslator translator;
    translator.load(strQmPath);
    a.installTranslator(&translator);

    MainWindow w;

    if(argc > 4)
    {
        int x = atoi(argv[1]);
        int y = atoi(argv[2]);
        int width = atoi(argv[3]);
        int height = atoi(argv[4]);

        w.move(x + width/2 - w.width()/2,
            y + height/2 - w.height()/2);
    }

    w.show();

    auto ret = a.exec();
    uniqueness_check.detach();

    return ret;
}
