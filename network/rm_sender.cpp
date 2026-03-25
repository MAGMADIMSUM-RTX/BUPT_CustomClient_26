#include "../log_manager.h"
#include "rm_sender.h"
#include <QtProtobuf/QProtobufSerializer>
#include <QStringBuilder>

RMSender::RMSender(QMqttClient *client, QObject *parent)
    : QObject(parent)
    , m_client(client)
{
}

quint8 RMSender::getQoS(const QString &topic)
{
    // Define QoS for sending topics here. Default is 0.
    // Modify the value to change QoS for specific topics.
    if (topic == "RemoteControl") return 0;
    if (topic == "MapClickInfoNotify") return 0;
    if (topic == "AssemblyCommand") return 0;
    if (topic == "RobotPerformanceSelectionCommand") return 0;
    if (topic == "HeroDeployModeEventCommand") return 0;
    if (topic == "RuneActivateCommand") return 0;
    if (topic == "DartCommand") return 0;
    if (topic == "GuardCtrlCommand") return 0;
    if (topic == "AirSupportCommand") return 0;

    return 0; // Default
}

void RMSender::sendRemoteControl(i32 mx, i32 my, i32 mz, bool left, bool right, u32 keys, bool mid, const QByteArray &data)
{
    if (m_client->state() != QMqttClient::Connected) return;
    
    robo_master::RemoteControl msg;
    msg.setMouseX(mx);
    msg.setMouseY(my);
    msg.setMouseZ(mz);
    msg.setLeftButtonDown(left);
    msg.setRightButtonDown(right);
    msg.setKeyboardValue(keys);
    msg.setMidButtonDown(mid);
    msg.setData(data);

    QProtobufSerializer serializer;
    QString topic = QStringLiteral("RemoteControl");
    m_client->publish(topic, msg.serialize(&serializer), getQoS(topic));
    
    QString details = "mouseX=" % QString::number(mx) %
                      " mouseY=" % QString::number(my) %
                      " mouseZ=" % QString::number(mz) %
                      " leftButtonDown=" % QString::number(left) %
                      " rightButtonDown=" % QString::number(right) %
                      " keyboardValue=" % QString::number(keys) %
                      " midButtonDown=" % QString::number(mid) %
                      " data=" % QString(data.toHex());
                      
    emit sigPacketLog(LogLevel::INFO, true, "RemoteControl", details);
}

void RMSender::sendMapClickInfo(u32 sendAll, const QByteArray &robotId, u32 mode, u32 enemyId, u32 ascii, u32 type, u32 sx, u32 sy, f32 mx, f32 my)
{
    if (m_client->state() != QMqttClient::Connected) {
        emit sigPacketLog(LogLevel::WARN, true, "MapClickInfoNotify", "Failed: Not Connected");
        return;
    }
    robo_master::MapClickInfoNotify msg;
    msg.setIsSendAll(sendAll);
    msg.setRobotId(robotId);
    msg.setMode(mode);
    msg.setEnemyId(enemyId);
    msg.setAscii(ascii);
    msg.setType(type);
    msg.setScreenX(sx);
    msg.setScreenY(sy);
    msg.setMapX(mx);
    msg.setMapY(my);
    QProtobufSerializer serializer;
    QString topic = QStringLiteral("MapClickInfoNotify");
    m_client->publish(topic, msg.serialize(&serializer), getQoS(topic));
    
    QString details = "isSendAll=" % QString::number(sendAll) %
                      " robotId=" % QString(robotId.toHex()) %
                      " mode=" % QString::number(mode) %
                      " enemyId=" % QString::number(enemyId) %
                      " ascii=" % QString::number(ascii) %
                      " type=" % QString::number(type) %
                      " screenX=" % QString::number(sx) %
                      " screenY=" % QString::number(sy) %
                      " mapX=" % QString::number(mx) %
                      " mapY=" % QString::number(my);
                      
    emit sigPacketLog(LogLevel::INFO, true, "MapClickInfoNotify", details);
}

void RMSender::sendAssemblyCommand(u32 operation, u32 difficulty)
{
    if (m_client->state() != QMqttClient::Connected) {
        emit sigPacketLog(LogLevel::WARN, true, "AssemblyCommand", "Failed: Not Connected");
        return;
    }
    robo_master::AssemblyCommand msg;
    msg.setOperation(operation);
    msg.setDifficulty(difficulty);
    QProtobufSerializer serializer;
    QString topic = QStringLiteral("AssemblyCommand");
    m_client->publish(topic, msg.serialize(&serializer), getQoS(topic));
    
    QString details = "operation=" % QString::number(operation) % " difficulty=" % QString::number(difficulty);
    emit sigPacketLog(LogLevel::INFO, true, "AssemblyCommand", details);
}

void RMSender::sendRobotPerformanceSelection(u32 shooter, u32 chassis)
{
    if (m_client->state() != QMqttClient::Connected) {
        emit sigPacketLog(LogLevel::WARN, true, "RobotPerformanceSelectionCommand", "Failed: Not Connected");
        return;
    }
    
    // Validation
    if (shooter < 1 || shooter > 4 || chassis < 1 || chassis > 4) {
        emit sigPacketLog(LogLevel::WARN, true, "RobotPerformanceSelectionCommand", 
            QString("Invalid Enum Value: shooter=%1, chassis=%2").arg(shooter).arg(chassis));
        // Continue sending anyway? The user said "throw WARN", implying detection.
        // Usually we might want to stop, but for debugging we might want to see what happens.
        // I will proceed to send, but the log is already emitted.
    }

    robo_master::RobotPerformanceSelectionCommand msg;
    msg.setShooter(shooter);
    msg.setChassis(chassis);
    QProtobufSerializer serializer;
    QString topic = QStringLiteral("RobotPerformanceSelectionCommand");
    m_client->publish(topic, msg.serialize(&serializer), getQoS(topic));
    
    QString details = "shooter=" % QString::number(shooter) % " chassis=" % QString::number(chassis);
    emit sigPacketLog(LogLevel::INFO, true, "RobotPerformanceSelectionCommand", details);
}

void RMSender::sendHeroDeployMode(u32 mode)
{
    if (m_client->state() != QMqttClient::Connected) {
        emit sigPacketLog(LogLevel::WARN, true, "HeroDeployModeEventCommand", "Failed: Not Connected");
        return;
    }
    robo_master::HeroDeployModeEventCommand msg;
    msg.setMode(mode);
    QProtobufSerializer serializer;
    QString topic = QStringLiteral("HeroDeployModeEventCommand");
    m_client->publish(topic, msg.serialize(&serializer), getQoS(topic));
    
    QString details = "mode=" % QString::number(mode);
    emit sigPacketLog(LogLevel::INFO, true, "HeroDeployModeEventCommand", details);
}

void RMSender::sendCommonCommand(u32 type, u32 param)
{
    if (m_client->state() != QMqttClient::Connected) {
        emit sigPacketLog(LogLevel::WARN, true, "CommonCommand", "Failed: Not Connected");
        return;
    }
    robo_master::CommonCommand msg;
    msg.setCmdType(type);
    msg.setParam(param);
    
    QProtobufSerializer serializer;
    QString topic = QStringLiteral("CommonCommand");
    m_client->publish(topic, msg.serialize(&serializer), getQoS(topic));
    
    QString details = "cmd_type=" % QString::number(type) %
                      " param=" % QString::number(param);
                      
    emit sigPacketLog(LogLevel::INFO, true, "CommonCommand", details);
}

void RMSender::sendRuneActivate(u32 activate)
{
    if (m_client->state() != QMqttClient::Connected) {
        emit sigPacketLog(LogLevel::WARN, true, "RuneActivateCommand", "Failed: Not Connected");
        return;
    }
    robo_master::RuneActivateCommand msg;
    msg.setActivate(activate);
    QProtobufSerializer serializer;
    QString topic = QStringLiteral("RuneActivateCommand");
    m_client->publish(topic, msg.serialize(&serializer), getQoS(topic));
    
    QString details = "activate=" % QString::number(activate);
    emit sigPacketLog(LogLevel::INFO, true, "RuneActivateCommand", details);
}

void RMSender::sendDartCommand(u32 targetId, bool open)
{
    if (m_client->state() != QMqttClient::Connected) {
        emit sigPacketLog(LogLevel::WARN, true, "DartCommand", "Failed: Not Connected");
        return;
    }
    robo_master::DartCommand msg;
    msg.setTargetId(targetId);
    msg.setOpen(open);
    QProtobufSerializer serializer;
    QString topic = QStringLiteral("DartCommand");
    m_client->publish(topic, msg.serialize(&serializer), getQoS(topic));
    
    QString details = "targetId=" % QString::number(targetId) % " open=" % QString::number(open);
    emit sigPacketLog(LogLevel::INFO, true, "DartCommand", details);
}

void RMSender::sendGuardCtrl(u32 commandId)
{
    if (m_client->state() != QMqttClient::Connected) {
        emit sigPacketLog(LogLevel::WARN, true, "GuardCtrlCommand", "Failed: Not Connected");
        return;
    }
    robo_master::GuardCtrlCommand msg;
    msg.setCommandId(commandId);
    QProtobufSerializer serializer;
    QString topic = QStringLiteral("GuardCtrlCommand");
    m_client->publish(topic, msg.serialize(&serializer), getQoS(topic));
    
    QString details = "commandId=" % QString::number(commandId);
    emit sigPacketLog(LogLevel::INFO, true, "GuardCtrlCommand", details);
}

void RMSender::sendAirSupport(u32 commandId)
{
    if (m_client->state() != QMqttClient::Connected) {
        emit sigPacketLog(LogLevel::WARN, true, "AirSupportCommand", "Failed: Not Connected");
        return;
    }
    robo_master::AirSupportCommand msg;
    msg.setCommandId(commandId);
    QProtobufSerializer serializer;
    QString topic = QStringLiteral("AirSupportCommand");
    m_client->publish(topic, msg.serialize(&serializer), getQoS(topic));
    
    QString details = "commandId=" % QString::number(commandId);
    emit sigPacketLog(LogLevel::INFO, true, "AirSupportCommand", details);
}


