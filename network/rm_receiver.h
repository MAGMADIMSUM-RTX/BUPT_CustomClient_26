#ifndef RM_RECEIVER_H
#define RM_RECEIVER_H

#include <QObject>
#include <QtMqtt/QMqttClient>
#include "rm_protocol.qpb.h"
#include "../types.h"
#include "../log_manager.h"

class RMReceiver : public QObject
{
    Q_OBJECT
public:
    explicit RMReceiver(QObject *parent = nullptr);

    void processMessage(const QByteArray &message, const QMqttTopicName &topic);

signals:
    // Log signal for UI with log level
    void sigPacketLog(LogLevel level, bool isSend, const QString &topic, const QString &details);

    // --- Decoupled Signals for UI Updates ---
    // 2.2.2 GameStatus
    void sigGameStatus(u32 round, u32 totalRounds, u32 redScore, u32 blueScore, u32 stage, i32 time, i32 elapsed, bool paused); 
    // 2.2.3 GlobalUnitStatus
    void sigGlobalUnitStatus(u32 baseHp, u32 baseStatus, u32 baseShield, u32 outpostHp, u32 outpostStatus, const QList<u32> &robotHp, const QList<i32> &robotBullets, u32 dmgRed, u32 dmgBlue);
    // 2.2.4 GlobalLogisticsStatus
    void sigGlobalLogistics(u32 economy, u64 totalEconomy, u32 techLevel, u32 encryptLevel);
    // 2.2.5 GlobalSpecialMechanism
    void sigGlobalSpecialMech(const QList<u32> &ids, const QList<i32> &times);
    // 2.2.6 Event
    void sigEvent(i32 eventId, const QString &param);
    // 2.2.7 RobotInjuryStat
    void sigRobotInjury(u32 total, u32 collision, u32 smallProj, u32 largeProj, u32 dart, u32 moduleOff, u32 wifiOff, u32 penalty, u32 serverKill, u32 killerId);
    // 2.2.8 RobotRespawnStatus
    void sigRobotRespawn(bool pending, u32 totalProg, u32 curProg, bool canFree, u32 goldCost, bool canPay);
    // 2.2.9 RobotStaticStatus
    void sigRobotStatic(u32 connState, u32 fieldState, u32 aliveState, u32 id, u32 type, u32 perfShooter, u32 perfChassis, u32 level, u32 maxHp, u32 maxHeat, u32 maxPower, u32 maxChassisEnergy, u32 maxBufferEnergy, f32 heatCoolDown);
    // 2.2.10 RobotDynamicStatus
    void sigRobotDynamic(u32 hp, f32 heat, f32 fireRate, u32 chassisEnergy, u32 bufferEnergy, u32 exp, u32 upExp, u32 totalFired, u32 ammo, bool outCombat, u32 combatTime, bool canHeal, bool canRemoteAmmo);
    // 2.2.11 RobotModuleStatus
    void sigRobotModule(u32 power, u32 rfid, u32 light, u32 smallS, u32 bigS, u32 uwb, u32 armor, u32 video, u32 cap, u32 mainCtrl);
    // 2.2.12 RobotPosition
    void sigRobotPosition(f32 x, f32 y, f32 z, f32 yaw);
    // 2.2.13 Buff
    void sigBuff(u32 robotId, u32 type, i32 level, u32 maxTime, u32 leftTime, const QString &msgParams);
    // 2.2.14 PenaltyInfo
    void sigPenalty(u32 type, u32 sec, u32 total);
    // 2.2.15 RobotPathPlanInfo
    void sigRobotPathPlan(u32 intention, u32 sx, u32 sy, const QList<i32> &offX, const QList<i32> &offY, u32 sender);
    // 2.2.17 RaderInfoToClient
    void sigRadarInfo(u32 targetId, f32 x, f32 y, f32 angle, u32 highlight);
    // 2.2.18 CustomByteBlock (Incoming)
    void sigCustomData(const QByteArray &data);
    // 2.2.20 TechCoreMotionStateSync
    void sigTechCoreState(u32 maxDiff, u32 status);
    // 2.2.22 RobotPerformanceSelectionSync
    void sigPerfSelSync(u32 shooter, u32 chassis);
    // 2.2.24 DeployModeStatusSync
    void sigDeployModeSync(u32 status);
    // 2.2.26 RuneStatusSync
    void sigRuneStatus(u32 status, u32 arms, u32 avgRings);
    // 2.2.27 SentinelStatusSync
    void sigSentinelStatus(u32 posture, bool weakened);
    // 2.2.29 DartSelectTargetStatusSync
    void sigDartTargetSync(u32 targetId, bool open);
    // 2.2.31 GuardCtrlResult
    void sigGuardCtrlResult(u32 cmdId, u32 result);
    // 2.2.33 AirSupportStatusSync
    void sigAirSupportSync(u32 status, u32 time, u32 cost);
};

#endif // RM_RECEIVER_H
