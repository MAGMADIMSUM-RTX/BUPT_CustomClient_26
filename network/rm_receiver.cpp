#include "../log_manager.h"
#include "rm_receiver.h"
#include <QtProtobuf/QProtobufSerializer>
#include <QStringBuilder>

RMReceiver::RMReceiver(QObject *parent)
    : QObject(parent)
{
}

void RMReceiver::processMessage(const QByteArray &message, const QMqttTopicName &topic)
{
    QProtobufSerializer serializer;
    QString name = topic.name();
    QString details;

    if (name == "GameStatus") {
        robo_master::GameStatus msg;
        if (msg.deserialize(&serializer, message)) {
            emit sigGameStatus(msg.currentRound(), msg.totalRounds(), msg.redScore(), msg.blueScore(), msg.currentStage(), msg.stageCountdownSec(), msg.stageElapsedSec(), msg.isPaused());
            details = "currentRound=" % QString::number(msg.currentRound()) %
                      " totalRounds=" % QString::number(msg.totalRounds()) %
                      " redScore=" % QString::number(msg.redScore()) %
                      " blueScore=" % QString::number(msg.blueScore()) %
                      " currentStage=" % QString::number(msg.currentStage()) %
                      " stageCountdownSec=" % QString::number(msg.stageCountdownSec()) %
                      " stageElapsedSec=" % QString::number(msg.stageElapsedSec()) %
                      " isPaused=" % QString::number(msg.isPaused());
        }
    }
    else if (name == "GlobalUnitStatus") {
        robo_master::GlobalUnitStatus msg;
        if (msg.deserialize(&serializer, message)) {
            QList<u32> hpList;
            for(auto h : msg.robotHealth()) hpList.append(h);
            QList<i32> bulletsList;
            for(auto b : msg.robotBullets()) bulletsList.append(b);
            emit sigGlobalUnitStatus(msg.baseHealth(), msg.baseStatus(), msg.baseShield(), msg.outpostHealth(), msg.outpostStatus(), hpList, bulletsList, msg.totalDamageRed(), msg.totalDamageBlue());
            
            QString hpStr; for(auto h : hpList) hpStr += QString::number(h) + " ";
            QString bulletStr; for(auto b : bulletsList) bulletStr += QString::number(b) + " ";
            
            details = "baseHealth=" % QString::number(msg.baseHealth()) %
                      " baseStatus=" % QString::number(msg.baseStatus()) %
                      " baseShield=" % QString::number(msg.baseShield()) %
                      " outpostHealth=" % QString::number(msg.outpostHealth()) %
                      " outpostStatus=" % QString::number(msg.outpostStatus()) %
                      " robotHealth=[" % hpStr.trimmed() %
                      "] robotBullets=[" % bulletStr.trimmed() %
                      "] totalDamageRed=" % QString::number(msg.totalDamageRed()) %
                      " totalDamageBlue=" % QString::number(msg.totalDamageBlue());
        }
    }
    else if (name == "GlobalLogisticsStatus") {
        robo_master::GlobalLogisticsStatus msg;
        if (msg.deserialize(&serializer, message)) {
            emit sigGlobalLogistics(msg.remainingEconomy(), msg.totalEconomyObtained(), msg.techLevel(), msg.encryptionLevel());
            details = "remainingEconomy=" % QString::number(msg.remainingEconomy()) %
                      " totalEconomyObtained=" % QString::number(msg.totalEconomyObtained()) %
                      " techLevel=" % QString::number(msg.techLevel()) %
                      " encryptionLevel=" % QString::number(msg.encryptionLevel());
        }
    }
    else if (name == "GlobalSpecialMechanism") {
        robo_master::GlobalSpecialMechanism msg;
        if (msg.deserialize(&serializer, message)) {
            QList<u32> ids; for(auto i : msg.mechanismId()) ids.append(i);
            QList<i32> times; for(auto t : msg.mechanismTimeSec()) times.append(t);
            emit sigGlobalSpecialMech(ids, times);
            
            QString idsStr; for(auto i : ids) idsStr += QString::number(i) + " ";
            QString timesStr; for(auto t : times) timesStr += QString::number(t) + " ";

            details = "mechanismId=[" % idsStr.trimmed() % "] mechanismTimeSec=[" % timesStr.trimmed() % "]";
        }
    }
    else if (name == "Event") {
        robo_master::Event msg;
        if (msg.deserialize(&serializer, message)) {
            emit sigEvent(msg.eventId(), msg.param());
            details = "eventId=" % QString::number(msg.eventId()) % " param=" % msg.param();
        }
    }
    else if (name == "RobotInjuryStat") {
        robo_master::RobotInjuryStat msg;
        if (msg.deserialize(&serializer, message)) {
            emit sigRobotInjury(msg.totalDamage(), msg.collisionDamage(), msg.smallProjectileDamage(), msg.largeProjectileDamage(), msg.dartSplashDamage(), msg.moduleOfflineDamage(), msg.wifiOfflineDamage(), msg.penaltyDamage(), msg.serverKillDamage(), msg.killerId());
            details = "totalDamage=" % QString::number(msg.totalDamage()) %
                      " collisionDamage=" % QString::number(msg.collisionDamage()) %
                      " smallProjectileDamage=" % QString::number(msg.smallProjectileDamage()) %
                      " largeProjectileDamage=" % QString::number(msg.largeProjectileDamage()) %
                      " dartSplashDamage=" % QString::number(msg.dartSplashDamage()) %
                      " moduleOfflineDamage=" % QString::number(msg.moduleOfflineDamage()) %
                      " wifiOfflineDamage=" % QString::number(msg.wifiOfflineDamage()) %
                      " penaltyDamage=" % QString::number(msg.penaltyDamage()) %
                      " serverKillDamage=" % QString::number(msg.serverKillDamage()) %
                      " killerId=" % QString::number(msg.killerId());
        }
    }
    else if (name == "RobotRespawnStatus") {
        robo_master::RobotRespawnStatus msg;
        if (msg.deserialize(&serializer, message)) {
            emit sigRobotRespawn(msg.isPendingRespawn(), msg.totalRespawnProgress(), msg.currentRespawnProgress(), msg.canFreeRespawn(), msg.goldCostForRespawn(), msg.canPayForRespawn());
            details = "isPendingRespawn=" % QString::number(msg.isPendingRespawn()) %
                      " totalRespawnProgress=" % QString::number(msg.totalRespawnProgress()) %
                      " currentRespawnProgress=" % QString::number(msg.currentRespawnProgress()) %
                      " canFreeRespawn=" % QString::number(msg.canFreeRespawn()) %
                      " goldCostForRespawn=" % QString::number(msg.goldCostForRespawn()) %
                      " canPayForRespawn=" % QString::number(msg.canPayForRespawn());
        }
    }
    else if (name == "RobotStaticStatus") {
        robo_master::RobotStaticStatus msg;
        if (msg.deserialize(&serializer, message)) {
            emit sigRobotStatic(msg.connectionState(), msg.fieldState(), msg.aliveState(), msg.robotId(), msg.robotType(), msg.performanceSystemShooter(), msg.performanceSystemChassis(), msg.level(), msg.maxHealth(), msg.maxHeat(), msg.maxPower(), msg.maxChassisEnergy(), msg.maxBufferEnergy(), msg.heatCooldownRate());
            details = "connectionState=" % QString::number(msg.connectionState()) %
                      " fieldState=" % QString::number(msg.fieldState()) %
                      " aliveState=" % QString::number(msg.aliveState()) %
                      " robotId=" % QString::number(msg.robotId()) %
                      " robotType=" % QString::number(msg.robotType()) %
                      " performanceSystemShooter=" % QString::number(msg.performanceSystemShooter()) %
                      " performanceSystemChassis=" % QString::number(msg.performanceSystemChassis()) %
                      " level=" % QString::number(msg.level()) %
                      " maxHealth=" % QString::number(msg.maxHealth()) %
                      " maxHeat=" % QString::number(msg.maxHeat()) %
                      " maxPower=" % QString::number(msg.maxPower()) %
                      " maxChassisEnergy=" % QString::number(msg.maxChassisEnergy()) %
                      " maxBufferEnergy=" % QString::number(msg.maxBufferEnergy()) %
                      " heatCooldownRate=" % QString::number(msg.heatCooldownRate());
        }
    }
    else if (name == "RobotDynamicStatus") {
        robo_master::RobotDynamicStatus msg;
        if (msg.deserialize(&serializer, message)) {
            emit sigRobotDynamic(msg.currentHealth(), msg.currentHeat(), msg.lastProjectileFireRate(), msg.currentChassisEnergy(), msg.currentBufferEnergy(), msg.currentExperience(), msg.experienceForUpgrade(), msg.totalProjectilesFired(), msg.remainingAmmo(), msg.isOutOfCombat(), msg.outOfCombatCountdown(), msg.canRemoteHeal(), msg.canRemoteAmmo());
            details = "currentHealth=" % QString::number(msg.currentHealth()) %
                      " currentHeat=" % QString::number(msg.currentHeat()) %
                      " lastProjectileFireRate=" % QString::number(msg.lastProjectileFireRate()) %
                      " currentChassisEnergy=" % QString::number(msg.currentChassisEnergy()) %
                      " currentBufferEnergy=" % QString::number(msg.currentBufferEnergy()) %
                      " currentExperience=" % QString::number(msg.currentExperience()) %
                      " experienceForUpgrade=" % QString::number(msg.experienceForUpgrade()) %
                      " totalProjectilesFired=" % QString::number(msg.totalProjectilesFired()) %
                      " remainingAmmo=" % QString::number(msg.remainingAmmo()) %
                      " isOutOfCombat=" % QString::number(msg.isOutOfCombat()) %
                      " outOfCombatCountdown=" % QString::number(msg.outOfCombatCountdown()) %
                      " canRemoteHeal=" % QString::number(msg.canRemoteHeal()) %
                      " canRemoteAmmo=" % QString::number(msg.canRemoteAmmo());
        }
    }
    else if (name == "RobotModuleStatus") {
        robo_master::RobotModuleStatus msg;
        if (msg.deserialize(&serializer, message)) {
            emit sigRobotModule(msg.powerManager(), msg.rfid(), msg.lightStrip(), msg.smallShooter(), msg.bigShooter(), msg.uwb(), msg.armor(), msg.videoTransmission(), msg.capacitor(), msg.mainController());
            details = "powerManager=" % QString::number(msg.powerManager()) %
                      " rfid=" % QString::number(msg.rfid()) %
                      " lightStrip=" % QString::number(msg.lightStrip()) %
                      " smallShooter=" % QString::number(msg.smallShooter()) %
                      " bigShooter=" % QString::number(msg.bigShooter()) %
                      " uwb=" % QString::number(msg.uwb()) %
                      " armor=" % QString::number(msg.armor()) %
                      " videoTransmission=" % QString::number(msg.videoTransmission()) %
                      " capacitor=" % QString::number(msg.capacitor()) %
                      " mainController=" % QString::number(msg.mainController());
        }
    }
    else if (name == "RobotPosition") {
        robo_master::RobotPosition msg;
        if (msg.deserialize(&serializer, message)) {
            emit sigRobotPosition(msg.x(), msg.y(), msg.z(), msg.yaw());
            details = "x=" % QString::number(msg.x()) %
                      " y=" % QString::number(msg.y()) %
                      " z=" % QString::number(msg.z()) %
                      " yaw=" % QString::number(msg.yaw());
        }
    }
    else if (name == "Buff") {
        robo_master::Buff msg;
        if (msg.deserialize(&serializer, message)) {
            emit sigBuff(msg.robotId(), msg.buffType(), msg.buffLevel(), msg.buffMaxTime(), msg.buffLeftTime(), msg.msgParams());
            details = "robotId=" % QString::number(msg.robotId()) %
                      " buffType=" % QString::number(msg.buffType()) %
                      " buffLevel=" % QString::number(msg.buffLevel()) %
                      " buffMaxTime=" % QString::number(msg.buffMaxTime()) %
                      " buffLeftTime=" % QString::number(msg.buffLeftTime()) %
                      " msgParams=" % msg.msgParams();
        }
    }
    else if (name == "PenaltyInfo") {
        robo_master::PenaltyInfo msg;
        if (msg.deserialize(&serializer, message)) {
            emit sigPenalty(msg.penaltyType(), msg.penaltyEffectSec(), msg.totalPenaltyNum());
            details = "penaltyType=" % QString::number(msg.penaltyType()) %
                      " penaltyEffectSec=" % QString::number(msg.penaltyEffectSec()) %
                      " totalPenaltyNum=" % QString::number(msg.totalPenaltyNum());
        }
    }
    else if (name == "RobotPathPlanInfo") {
        robo_master::RobotPathPlanInfo msg;
        if (msg.deserialize(&serializer, message)) {
            QList<i32> ox; for(auto v : msg.offsetX()) ox.append(v);
            QList<i32> oy; for(auto v : msg.offsetY()) oy.append(v);
            emit sigRobotPathPlan(msg.intention(), msg.startPosX(), msg.startPosY(), ox, oy, msg.senderId());
            
            QString oxStr; for(auto v : ox) oxStr += QString::number(v) + " ";
            QString oyStr; for(auto v : oy) oyStr += QString::number(v) + " ";

            details = "intention=" % QString::number(msg.intention()) %
                      " startPosX=" % QString::number(msg.startPosX()) %
                      " startPosY=" % QString::number(msg.startPosY()) %
                      " offsetX=[" % oxStr.trimmed() %
                      "] offsetY=[" % oyStr.trimmed() %
                      "] senderId=" % QString::number(msg.senderId());
        }
    }
    else if (name == "RaderInfoToClient") {
        robo_master::RaderInfoToClient msg;
        if (msg.deserialize(&serializer, message)) {
            emit sigRadarInfo(msg.targetRobotId(), msg.targetPosX(), msg.targetPosY(), msg.torwardAngle(), msg.isHighLight());
            details = "targetRobotId=" % QString::number(msg.targetRobotId()) %
                      " targetPosX=" % QString::number(msg.targetPosX()) %
                      " targetPosY=" % QString::number(msg.targetPosY()) %
                      " torwardAngle=" % QString::number(msg.torwardAngle()) %
                      " isHighLight=" % QString::number(msg.isHighLight());
        }
    }
    else if (name == "CustomByteBlock") {
        robo_master::CustomByteBlock msg;
        if (msg.deserialize(&serializer, message)) {
            emit sigCustomData(msg.data());
            details = "dataSize=" % QString::number(msg.data().size()) % " data=" % QString(msg.data().toHex());
        }
    }
    else if (name == "TechCoreMotionStateSync") {
        robo_master::TechCoreMotionStateSync msg;
        if (msg.deserialize(&serializer, message)) {
            emit sigTechCoreState(msg.maximumDifficultyLevel(), msg.status());
            details = "maximumDifficultyLevel=" % QString::number(msg.maximumDifficultyLevel()) %
                      " status=" % QString::number(msg.status());
        }
    }
    else if (name == "RobotPerformanceSelectionSync") {
        robo_master::RobotPerformanceSelectionSync msg;
        if (msg.deserialize(&serializer, message)) {
            // Validation
            if (msg.shooter() < 1 || msg.shooter() > 4 || msg.chassis() < 1 || msg.chassis() > 4) {
                 emit sigPacketLog(LogLevel::WARN, false, "RobotPerformanceSelectionSync", 
                    QString("Invalid Enum Value: shooter=%1, chassis=%2").arg(msg.shooter()).arg(msg.chassis()));
            }

            emit sigPerfSelSync(msg.shooter(), msg.chassis());
            details = "shooter=" % QString::number(msg.shooter()) % " chassis=" % QString::number(msg.chassis());
        }
    }
    else if (name == "DeployModeStatusSync") {
        robo_master::DeployModeStatusSync msg;
        if (msg.deserialize(&serializer, message)) {
            emit sigDeployModeSync(msg.status());
            details = "status=" % QString::number(msg.status());
        }
    }
    else if (name == "RuneStatusSync") {
        robo_master::RuneStatusSync msg;
        if (msg.deserialize(&serializer, message)) {
            emit sigRuneStatus(msg.runeStatus(), msg.activatedArms(), msg.averageRings());
            details = "runeStatus=" % QString::number(msg.runeStatus()) %
                      " activatedArms=" % QString::number(msg.activatedArms()) %
                      " averageRings=" % QString::number(msg.averageRings());
        }
    }
    else if (name == "SentinelStatusSync") {
        robo_master::SentinelStatusSync msg;
        if (msg.deserialize(&serializer, message)) {
            emit sigSentinelStatus(msg.postureId(), msg.isWeakened());
            details = "postureId=" % QString::number(msg.postureId()) % " isWeakened=" % QString::number(msg.isWeakened());
        }
    }
    else if (name == "DartSelectTargetStatusSync") {
        robo_master::DartSelectTargetStatusSync msg;
        if (msg.deserialize(&serializer, message)) {
            emit sigDartTargetSync(msg.targetId(), msg.open());
            details = "targetId=" % QString::number(msg.targetId()) % " open=" % QString::number(msg.open());
        }
    }
    else if (name == "GuardCtrlResult") {
        robo_master::GuardCtrlResult msg;
        if (msg.deserialize(&serializer, message)) {
            emit sigGuardCtrlResult(msg.commandId(), msg.resultCode());
            details = "commandId=" % QString::number(msg.commandId()) % " resultCode=" % QString::number(msg.resultCode());
        }
    }
    else if (name == "AirSupportStatusSync") {
        robo_master::AirSupportStatusSync msg;
        if (msg.deserialize(&serializer, message)) {
            emit sigAirSupportSync(msg.airsupportStatus(), msg.leftTime(), msg.costCoins());
            details = "airsupportStatus=" % QString::number(msg.airsupportStatus()) %
                      " leftTime=" % QString::number(msg.leftTime()) %
                      " costCoins=" % QString::number(msg.costCoins());
        }
    }
    else {
        details = "Unknown Topic, Payload: " % QString(message.toHex());
    }
    
    if (details.isEmpty()) {
        details = "Deserialization Failed, Payload: " % QString(message.toHex());
    }

    emit sigPacketLog(LogLevel::INFO, false, name, details);
}
