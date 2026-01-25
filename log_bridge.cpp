#include "log_bridge.h"
#include <QDebug>

LogBridge::LogBridge(QObject *parent) : QObject(parent)
{
}

void LogBridge::onPacketLog(LogLevel level, bool isSend, const QString &topic, const QString &details)
{
    // Determine log level (logic preserved from LogPanelWidget)
    LogLevel finalLevel = level;
    if (topic.contains("ERROR", Qt::CaseInsensitive) || details.contains("Failed", Qt::CaseInsensitive)) {
        finalLevel = LogLevel::ERROR;
    } else if (topic.contains("WARN", Qt::CaseInsensitive) || details.contains("Warning", Qt::CaseInsensitive)) {
        finalLevel = LogLevel::WARN;
    } else if (topic.contains("DEBUG", Qt::CaseInsensitive) || details.contains("Debug", Qt::CaseInsensitive)) {
        finalLevel = LogLevel::DEBUG;
    }
    
    // Write to log file
    QString dirPrefix = isSend ? "[Send]" : "[Recv]";
    QString msgContent = QString("%1 %2 - %3").arg(dirPrefix).arg(topic).arg(details);
    LogManager::instance().writeLog(finalLevel, LogManager::CATEGORY_MQTT, msgContent);
    
    // Sync to command line
    QString levelStr = "INFO";
    if (finalLevel == LogLevel::ERROR) levelStr = "ERROR";
    else if (finalLevel == LogLevel::WARN) levelStr = "WARN";
    else if (finalLevel == LogLevel::DEBUG) levelStr = "DEBUG";
    
    qInfo().noquote() << QString("[%1] [%2] %3").arg(levelStr).arg(LogManager::CATEGORY_MQTT).arg(msgContent);
}

void LogBridge::onConnectionStateChanged(int state)
{
    QString stateStr;
    if(state == 0) stateStr = "Disconnected";
    else if(state == 1) stateStr = "Connecting";
    else if(state == 2) stateStr = "Connected";
    
    LogManager::instance().writeLog(LogLevel::INFO, LogManager::CATEGORY_MQTT, "[Connection] State changed to: " + stateStr);
    qInfo() << "Connection State:" << stateStr;
}

void LogBridge::onVideoLog(const QString &msg)
{
    LogManager::instance().writeLog(LogLevel::INFO, LogManager::CATEGORY_VIDEO, msg);
    qInfo().noquote() << QString("[INFO] [%1] %2").arg(LogManager::CATEGORY_VIDEO).arg(msg);
}
