#include "log.h"
#include <QLoggingCategory>
#include <QDateTime>
#include <QMutexLocker>
#include <QDir>
#include <QFile>
#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>

namespace {
const QString kLogPath{"/.cache/ucar_imu_tool/"};
const QString kLogFileName{"ucar_imu_tool.log"};
const qint64 kLogFileSize{100*1024*1024}; // 100M
const quint8 kLogFileNumber{5};
}

void logMessageFormatHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    // file name
    const char *file = context.file ? context.file : "";
    const char *base_name = strrchr(file, '/');
    if(base_name)
        file = base_name + 1;
    // timestamp
//    QString time_string = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz");

    timeval timeval_now;
    gettimeofday(&timeval_now, nullptr);

    tm *tm_now;
    tm_now = localtime(&timeval_now.tv_sec);

    char time_ch[100];
    strftime(time_ch, 100, "%F %T", tm_now);

    QString msec = QString("%1").arg(timeval_now.tv_usec / 1000, 3, 10, QLatin1Char('0'));
    QString time_string = QString(time_ch) + "." + msec;

    char level = 'F';
    switch (type) {
    case QtDebugMsg:
        level = 'D';
        break;
    case QtInfoMsg:
        level = 'I';
        break;
    case QtWarningMsg:
        level = 'W';
        break;
    case QtCriticalMsg:
        level = 'C';
        break;
    case QtFatalMsg:
        level = 'F';
        break;
    }

    QString log_message;
    log_message.sprintf("%s | %08x:%08lx | %c | %s:%u | %s \n", qPrintable(time_string), getpid(),
            pthread_self(), level, file, context.line, qPrintable(msg));

    Log::instance().enqueueMessage(log_message);
}

LogPrinter::LogPrinter(const QString &log_file)
    :m_log_file(log_file)
{
}

void LogPrinter::onNewMessageToPrint()
{
    QString message = Log::instance().dequeueMessage();
    if(message.isEmpty())
        return ;

    renameLogs();
//    fprintf(stderr, "%s\n", qPrintable(message));

    QFile log_file(m_log_file);
    log_file.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream text_stream(&log_file);
    text_stream << message;
    log_file.flush();
    log_file.close();
}

void LogPrinter::run()
{
    exec();
}


void Log::init()
{
    QDir dir;
    QString home_path = QDir::homePath();
    QString log_path = home_path + kLogPath;
    QString log_file = log_path + kLogFileName;

    if(false == dir.exists(log_path))
    {
        // create dir
        dir.mkpath(log_path);
    }

    if (true == QFile::exists(log_file))
    {
        QFile::remove(log_file);
    }

    m_printer_thread = new LogPrinter(log_file);

    connect(this, &Log::newMessageToPrint, m_printer_thread,
            &LogPrinter::onNewMessageToPrint, Qt::QueuedConnection);

    m_printer_thread->start();

    qInstallMessageHandler(logMessageFormatHandler);
}

void Log::enqueueMessage(const QString &message)
{
    QMutexLocker locker(&m_queue_mutex);

    m_queue.enqueue(message);
    emit newMessageToPrint();
}

QString Log::dequeueMessage()
{
    QMutexLocker locker(&m_queue_mutex);

    if(!m_queue.isEmpty())
    {
        return m_queue.dequeue();
    }
    else
    {
        return QString();
    }
}

void LogPrinter::renameLogs()
{
    QFileInfo logInfo(m_log_file);
    if (logInfo.exists() && logInfo.size() > kLogFileSize)
    {
        for (int i = kLogFileNumber - 1; i >=1; i--)
        {
            QString oldLogName = m_log_file + "." + QString::number(i);
            QString newLogName = m_log_file + "." + QString::number(i + 1);

            QFileInfo oldLogFile(oldLogName);
            if (oldLogFile.exists())
            {
                QString cmd = "mv " + oldLogName + " " + newLogName;
                system(cmd.toStdString().c_str());
            }
        }

        QString renameFileName = m_log_file + ".1";
        QString renameCmd = "mv " + m_log_file + " " + renameFileName;
        system(renameCmd.toStdString().c_str());
    }
}

