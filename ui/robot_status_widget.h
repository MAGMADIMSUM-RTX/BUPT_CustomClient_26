#ifndef ROBOT_STATUS_WIDGET_H
#define ROBOT_STATUS_WIDGET_H

#include <QWidget>
#include <QLabel>
#include <QTreeWidget>
#include <QGroupBox>
#include <QGridLayout>
#include "../types.h"

class RobotStatusWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RobotStatusWidget(QWidget *parent = nullptr);

public slots:
    void updateRobotDynamic(u32 hp, f32 heat, f32 fireRate, u32 chassisEnergy, u32 bufferEnergy, u32 exp, u32 upExp, u32 totalFired, u32 ammo, bool outCombat, u32 combatTime, bool canHeal, bool canRemoteAmmo);
    void updateRobotPosition(f32 x, f32 y, f32 z, f32 yaw);
    void updateRobotStatic(u32 connState, u32 fieldState, u32 aliveState, u32 id, u32 type, u32 perfShooter, u32 perfChassis, u32 level, u32 maxHp, u32 maxHeat, u32 maxPower, u32 maxChassisEnergy, u32 maxBufferEnergy, f32 heatCoolDown);
    void updateRobotInjury(u32 total, u32 collision, u32 smallProj, u32 largeProj, u32 dart, u32 moduleOff, u32 wifiOff, u32 penalty, u32 serverKill, u32 killerId);
    void updateRobotRespawn(bool pending, u32 totalProg, u32 curProg, bool canFree, u32 goldCost, bool canPay);
    void updateRobotModule(u32 power, u32 rfid, u32 light, u32 smallS, u32 bigS, u32 uwb, u32 armor, u32 video, u32 cap, u32 mainCtrl);
    void updateBuff(u32 robotId, u32 type, i32 level, u32 maxTime, u32 leftTime, const QString &msgParams);
    void updatePenalty(u32 type, u32 sec, u32 total);

private:
    void setupUi();

    // Static Status
    QLabel *lblMyRobotId;
    QLabel *lblMyRobotLevel;
    QLabel *lblMyRobotConn;
    QLabel *lblMyRobotField;
    QLabel *lblMyRobotAlive;
    QLabel *lblMyRobotPerf;
    QLabel *lblMyRobotMaxStats;
    
    // Dynamic Status
    QLabel *lblMyRobotHp;
    QLabel *lblMyRobotHeat;
    QLabel *lblMyRobotAmmo;
    QLabel *lblMyRobotPos;
    QLabel *lblMyRobotState; // Combat/Out
    QLabel *lblMyRobotExp;
    QLabel *lblMyRobotBuffer;
    QLabel *lblMyRobotChassisEnergy;
    QLabel *lblMyRobotFire;
    QLabel *lblMyRobotCanHeal;
    QLabel *lblMyRobotCanRefill;

    // Other Info
    QLabel *lblRespawnInfo;
    QLabel *lblPenaltyInfo;
    
    QTreeWidget *treeInjury;
    QTreeWidget *treeModules;
    QTreeWidget *treeBuffs;
};

#endif // ROBOT_STATUS_WIDGET_H
