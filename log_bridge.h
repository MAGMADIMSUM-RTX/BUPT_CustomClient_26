#ifndef LOG_BRIDGE_H
#define LOG_BRIDGE_H

#include <QObject>
#include "log_manager.h"

class LogBridge : public QObject
{
    Q_OBJECT
public:
    explicit LogBridge(QObject *parent = nullptr);

public slots:
    void onPacketLog(LogLevel level, bool isSend, const QString &topic, const QString &details);
    void onConnectionStateChanged(int state);
    void onVideoLog(const QString &msg);
};

#endif // LOG_BRIDGE_H
