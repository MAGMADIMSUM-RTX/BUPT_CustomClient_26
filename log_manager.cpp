#include "log_manager.h"
#include <QDateTime>
#include <QDebug>
#include <QDir>

// --- LogWorker Implementation ---

LogWorker::LogWorker(const QString &fileName)
    : m_fileName(fileName), m_stop(false)
{
}

LogWorker::~LogWorker()
{
    stop();
    wait();
}

void LogWorker::enqueue(const QString &msg)
{
    QMutexLocker locker(&m_mutex);
    m_queue.enqueue(msg);
    m_cond.wakeOne();
}

void LogWorker::stop()
{
    QMutexLocker locker(&m_mutex);
    m_stop = true;
    m_cond.wakeOne();
}

void LogWorker::run()
{
    QFile logFile(m_fileName);
    if (!logFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) {
        qWarning() << "Failed to open log file in worker thread:" << m_fileName;
        return;
    }
    
    // Use system buffer, do not set strict buffering manually unless needed
    QTextStream stream(&logFile);
    stream.setEncoding(QStringConverter::Utf8);

    while (true) {
        QQueue<QString> localQueue;
        {
            QMutexLocker locker(&m_mutex);
            if (m_queue.isEmpty() && !m_stop) {
                m_cond.wait(&m_mutex);
            }
            if (m_stop && m_queue.isEmpty()) {
                break;
            }
            localQueue.swap(m_queue);
        }

        // Write all messages in batch
        while (!localQueue.isEmpty()) {
            stream << localQueue.dequeue() << "\n";
        }
        stream.flush(); // Flush after batch write
    }
    
    logFile.close();
}


// --- LogManager Implementation ---

LogManager& LogManager::instance()
{
    static LogManager instance;
    return instance;
}

LogManager::LogManager()
{
    QString fileName = QString("CustomClient-%1.log").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd-HH-mm-ss"));
    
    m_worker = new LogWorker(fileName);
    m_worker->start();
    
    writeLog(LogLevel::INFO, CATEGORY_SYSTEM, "Log system started (Async Mode). File: " + fileName);
}

LogManager::~LogManager()
{
    if (m_worker) {
        delete m_worker; // Destructor calls stop() and wait()
        m_worker = nullptr;
    }
}

void LogManager::writeLog(LogLevel level, const QString &category, const QString &message)
{
    QString levelStr;
    switch (level) {
        case LogLevel::DEBUG: levelStr = "DEBUG"; break;
        case LogLevel::INFO:  levelStr = "INFO";  break;
        case LogLevel::WARN:  levelStr = "WARN";  break;
        case LogLevel::ERROR: levelStr = "ERROR"; break;
        default:              levelStr = "UNKNOWN"; break;
    }

    QString timeStr = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz");
    // Format: [Timestamp] [Level] [Category] Message
    QString logLine = QString("[%1] [%2] [%3] %4").arg(timeStr).arg(levelStr).arg(category).arg(message);
    
    if (m_worker) {
        m_worker->enqueue(logLine);
    }
}
