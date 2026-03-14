#include "rm_client.h"
#include <QDebug>
#include <QTimer>
#include <QtProtobuf/QProtobufSerializer>

RMClient::RMClient(QObject *parent)
    : QObject(parent)
    , m_client(new QMqttClient(this))
{
    // Initialize Sender and Receiver
    m_sender = new RMSender(m_client, this);
    m_receiver = new RMReceiver(this);

    // Server Configuration
    m_client->setHostname(QStringLiteral("100.100.100.21"));
    m_client->setPort(3333);

    connect(m_client, &QMqttClient::connected, this, &RMClient::onConnected);
    connect(m_client, &QMqttClient::messageReceived, this, &RMClient::onMessageReceived);
    connect(m_client, &QMqttClient::stateChanged, this, &RMClient::sigConnectionStateChanged);

    // Auto-reconnect logic
    connect(m_client, &QMqttClient::disconnected, this, [this](){
        qInfo() << "Disconnected. Reconnecting in 2s...";
        emit sigPacketLog(LogLevel::ERROR, false, "Connection", "MQTT Disconnected");
        QTimer::singleShot(2000, this, &RMClient::connectToHost);
    });

    // Connect Receiver Signals to RMClient Signals
    connect(m_receiver, &RMReceiver::sigPacketLog, this, &RMClient::sigPacketLog);
    connect(m_receiver, &RMReceiver::sigGameStatus, this, &RMClient::sigGameStatus);
    connect(m_receiver, &RMReceiver::sigGlobalUnitStatus, this, &RMClient::sigGlobalUnitStatus);
    connect(m_receiver, &RMReceiver::sigGlobalLogistics, this, &RMClient::sigGlobalLogistics);
    connect(m_receiver, &RMReceiver::sigGlobalSpecialMech, this, &RMClient::sigGlobalSpecialMech);
    connect(m_receiver, &RMReceiver::sigEvent, this, &RMClient::sigEvent);
    connect(m_receiver, &RMReceiver::sigRobotInjury, this, &RMClient::sigRobotInjury);
    connect(m_receiver, &RMReceiver::sigRobotRespawn, this, &RMClient::sigRobotRespawn);
    connect(m_receiver, &RMReceiver::sigRobotStatic, this, &RMClient::sigRobotStatic);
    connect(m_receiver, &RMReceiver::sigRobotDynamic, this, &RMClient::sigRobotDynamic);
    connect(m_receiver, &RMReceiver::sigRobotModule, this, &RMClient::sigRobotModule);
    connect(m_receiver, &RMReceiver::sigRobotPosition, this, &RMClient::sigRobotPosition);
    connect(m_receiver, &RMReceiver::sigBuff, this, &RMClient::sigBuff);
    connect(m_receiver, &RMReceiver::sigPenalty, this, &RMClient::sigPenalty);
    connect(m_receiver, &RMReceiver::sigRobotPathPlan, this, &RMClient::sigRobotPathPlan);
    connect(m_receiver, &RMReceiver::sigRadarInfo, this, &RMClient::sigRadarInfo);
    connect(m_receiver, &RMReceiver::sigCustomData, this, &RMClient::sigCustomData);
    connect(m_receiver, &RMReceiver::sigTechCoreState, this, &RMClient::sigTechCoreState);
    connect(m_receiver, &RMReceiver::sigPerfSelSync, this, &RMClient::sigPerfSelSync);
    connect(m_receiver, &RMReceiver::sigDeployModeSync, this, &RMClient::sigDeployModeSync);
    connect(m_receiver, &RMReceiver::sigRuneStatus, this, &RMClient::sigRuneStatus);
    connect(m_receiver, &RMReceiver::sigSentinelStatus, this, &RMClient::sigSentinelStatus);
    connect(m_receiver, &RMReceiver::sigDartTargetSync, this, &RMClient::sigDartTargetSync);
    connect(m_receiver, &RMReceiver::sigGuardCtrlResult, this, &RMClient::sigGuardCtrlResult);
    connect(m_receiver, &RMReceiver::sigAirSupportSync, this, &RMClient::sigAirSupportSync);

    // Connect Sender Signals
    connect(m_sender, &RMSender::sigPacketLog, this, &RMClient::sigPacketLog);
}

void RMClient::connectToHost()
{
    if (m_client->state() == QMqttClient::Disconnected) {
        qInfo() << "Connecting to" << m_client->hostname() << "...";
        m_client->connectToHost();
    }
}

// --- Sender Methods (Proxy) ---

void RMClient::sendRemoteControl(i32 mx, i32 my, i32 mz, bool left, bool right, u32 keys, bool mid, const QByteArray &data)
{
    m_sender->sendRemoteControl(mx, my, mz, left, right, keys, mid, data);
}

void RMClient::sendMapClickInfo(u32 sendAll, const QByteArray &robotId, u32 mode, u32 enemyId, u32 ascii, u32 type, u32 sx, u32 sy, f32 mx, f32 my)
{
    m_sender->sendMapClickInfo(sendAll, robotId, mode, enemyId, ascii, type, sx, sy, mx, my);
}

void RMClient::sendAssemblyCommand(u32 operation, u32 difficulty)
{
    m_sender->sendAssemblyCommand(operation, difficulty);
}

void RMClient::sendRobotPerformanceSelection(u32 shooter, u32 chassis)
{
    m_sender->sendRobotPerformanceSelection(shooter, chassis);
}

void RMClient::sendHeroDeployMode(u32 mode)
{
    m_sender->sendHeroDeployMode(mode);
}

void RMClient::sendRuneActivate(u32 activate)
{
    m_sender->sendRuneActivate(activate);
}

void RMClient::sendDartCommand(u32 targetId, bool open)
{
    m_sender->sendDartCommand(targetId, open);
}

void RMClient::sendGuardCtrl(u32 commandId)
{
    m_sender->sendGuardCtrl(commandId);
}

void RMClient::sendAirSupport(u32 commandId)
{
    m_sender->sendAirSupport(commandId);
}


// --- Receiver Logic ---

void RMClient::onConnected()
{
    qInfo() << "Connected to RM Server!";
    
    struct TopicConfig {
        QString name;
        quint8 qos;
    };

    const TopicConfig topics[] = {
        {"GameStatus", 0}, {"GlobalUnitStatus", 0}, {"GlobalLogisticsStatus", 0}, {"GlobalSpecialMechanism", 0},
        {"Event", 0}, {"RobotInjuryStat", 0}, {"RobotRespawnStatus", 0}, {"RobotStaticStatus", 0},
        {"RobotDynamicStatus", 0}, {"RobotModuleStatus", 0}, {"RobotPosition", 0}, {"Buff", 0},
        {"PenaltyInfo", 0}, {"RobotPathPlanInfo", 0}, {"RaderInfoToClient", 0}, {"CustomByteBlock", 0},
        {"TechCoreMotionStateSync", 0}, {"RobotPerformanceSelectionSync", 0}, {"DeployModeStatusSync", 0},
        {"RuneStatusSync", 0}, {"SentinelStatusSync", 0}, {"DartSelectTargetStatusSync", 0},
        {"GuardCtrlResult", 0}, {"AirSupportStatusSync", 0}
    };

    for (const auto &config : topics) {
        auto sub = m_client->subscribe(config.name, config.qos);
        if (sub) {
            connect(sub, &QMqttSubscription::stateChanged, this, [this, config](QMqttSubscription::SubscriptionState state){
                if (state == QMqttSubscription::Subscribed) {
                     emit sigPacketLog(LogLevel::INFO, false, "Subscription", QString("Subscribed to %1 (QoS %2): Success").arg(config.name).arg(config.qos));
                } else if (state == QMqttSubscription::Error) {
                     emit sigPacketLog(LogLevel::WARN, false, "Subscription", QString("Subscribed to %1 (QoS %2): Failed").arg(config.name).arg(config.qos));
                }
            });
        } else {
             emit sigPacketLog(LogLevel::WARN, false, "Subscription", QString("Subscribed to %1 (QoS %2): Failed (Immediate)").arg(config.name).arg(config.qos));
        }
    }
}

void RMClient::onMessageReceived(const QByteArray &message, const QMqttTopicName &topic)
{
    m_receiver->processMessage(message, topic);
}
