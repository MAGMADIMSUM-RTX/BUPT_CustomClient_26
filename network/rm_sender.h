#ifndef RM_SENDER_H
#define RM_SENDER_H

#include <QObject>
#include <QtMqtt/QMqttClient>
#include "rm_protocol.qpb.h"
#include "../types.h"
#include "../log_manager.h"

class RMSender : public QObject
{
    Q_OBJECT
public:
    explicit RMSender(QMqttClient *client, QObject *parent = nullptr);

    void sendRemoteControl(i32 mx, i32 my, i32 mz, bool left, bool right, u32 keys, bool mid, const QByteArray &data);
    // Trigger-based Commands
    void sendMapClickInfo(u32 sendAll, const QByteArray &robotId, u32 mode, u32 enemyId, u32 ascii, u32 type, u32 sx, u32 sy, f32 mx, f32 my);
    void sendAssemblyCommand(u32 operation, u32 difficulty);
    void sendRobotPerformanceSelection(u32 shooter, u32 chassis);
    void sendHeroDeployMode(u32 mode);
    void sendRuneActivate(u32 activate);
    void sendDartCommand(u32 targetId, bool open);
    void sendGuardCtrl(u32 commandId);
    void sendAirSupport(u32 commandId);

signals:
    void sigPacketLog(LogLevel level, bool isSend, const QString &topic, const QString &details);

private:
    quint8 getQoS(const QString &topic);
    QMqttClient *m_client;
};

#endif // RM_SENDER_H
