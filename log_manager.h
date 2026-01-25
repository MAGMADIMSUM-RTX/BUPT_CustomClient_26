#ifndef LOG_MANAGER_H
#define LOG_MANAGER_H

#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QMutex>
#include <QThread>
#include <QQueue>
#include <QWaitCondition>

class LogWorker : public QThread
{
public:
    LogWorker(const QString &fileName);
    ~LogWorker();
    void enqueue(const QString &msg);
    void stop();

protected:
    void run() override;

private:
    QString m_fileName;
    QQueue<QString> m_queue;
    QMutex m_mutex;
    QWaitCondition m_cond;
    bool m_stop;
};

enum class LogLevel {
    DEBUG,
    INFO,
    WARN,
    ERROR
};

class LogManager : public QObject
{
    Q_OBJECT
public:
    static LogManager& instance();

    // Standard Log Categories
    static constexpr char CATEGORY_MQTT[] = "MQTT";
    static constexpr char CATEGORY_VIDEO[] = "Video";
    static constexpr char CATEGORY_SYSTEM[] = "System";

    void writeLog(LogLevel level, const QString &category, const QString &message);

private:
    LogManager();
    ~LogManager();
    LogWorker *m_worker;
};

#endif // LOG_MANAGER_H
