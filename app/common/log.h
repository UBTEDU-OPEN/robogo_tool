#ifndef LOG_H
#define LOG_H

#include "singleton.h"
#include <QObject>
#include <QQueue>
#include <QMutex>
#include <QThread>

class LogPrinter : public QThread
{
    Q_OBJECT
public:
    explicit LogPrinter(const QString &log_file);

public slots:
    void onNewMessageToPrint();

protected:
    void run() override;

    void renameLogs();

private:
    QString m_log_file;
};

class Log : public QObject,  public Singleton<Log>
{
    Q_OBJECT

    friend class Singleton;
    friend class LogPrinter;
    friend void logMessageFormatHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

public:
    void init();

signals:
    void newMessageToPrint();

protected:
    /**
     * @brief enqueueMessage
     * 将消息插入队列尾部
     * @param message
     * log message
     */
    void enqueueMessage(const QString &message);

    /**
     * @brief dequeueMessage
     * 从队列头部移除并返回消息
     * @return
     *  log message
     */
    QString dequeueMessage();


private:
    QQueue<QString> m_queue;
    QMutex m_queue_mutex;
    LogPrinter *m_printer_thread;
};



#endif // LOG_H
