#include "robot_status_widget.h"
#include <QVBoxLayout>
#include <QDebug>

RobotStatusWidget::RobotStatusWidget(QWidget *parent) : QWidget(parent)
{
    setupUi();
}

void RobotStatusWidget::setupUi()
{
    QGridLayout *layout = new QGridLayout(this);

    // Status
    QGroupBox *grpStatus = new QGroupBox("自身状态");
    QGridLayout *statLayout = new QGridLayout(grpStatus);
    
    // Static
    lblMyRobotId = new QLabel("ID: 0");
    lblMyRobotLevel = new QLabel("等级: 0");
    lblMyRobotConn = new QLabel("连接: 未连接");
    lblMyRobotField = new QLabel("上场: 未上场");
    lblMyRobotAlive = new QLabel("存活: 未知");
    lblMyRobotPerf = new QLabel("性能: 发射机构 0, 底盘 0");
    lblMyRobotMaxStats = new QLabel("最大值: 血量 0, 热量 0, 功率 0");

    // Dynamic
    lblMyRobotHp = new QLabel("血量: 0/0");
    lblMyRobotHeat = new QLabel("热量: 0/0 (冷却: 0.0)");
    lblMyRobotAmmo = new QLabel("弹丸: 0");
    lblMyRobotPos = new QLabel("位置: (0,0,0) 朝向: 0");
    lblMyRobotState = new QLabel("状态: 正常");
    lblMyRobotExp = new QLabel("经验: 0 (升级: 0)");
    lblMyRobotBuffer = new QLabel("缓冲: 0");
    lblMyRobotChassisEnergy = new QLabel("底盘能量: 0");
    lblMyRobotFire = new QLabel("发射: 0 (射速: 0)");
    lblMyRobotCanHeal = new QLabel("远程补血: 否");
    lblMyRobotCanRefill = new QLabel("远程补弹: 否");
    
    // Respawn & Penalty
    lblRespawnInfo = new QLabel("复活: N/A");
    lblPenaltyInfo = new QLabel("判罚: 无");

    int row = 0;
    statLayout->addWidget(lblMyRobotId, row, 0);
    statLayout->addWidget(lblMyRobotLevel, row, 1);
    statLayout->addWidget(lblMyRobotConn, row, 2);
    statLayout->addWidget(lblMyRobotField, row, 3);
    row++;
    statLayout->addWidget(lblMyRobotAlive, row, 0);
    statLayout->addWidget(lblMyRobotPerf, row, 1, 1, 2);
    statLayout->addWidget(lblMyRobotMaxStats, row, 3);
    row++;
    statLayout->addWidget(lblMyRobotHp, row, 0);
    statLayout->addWidget(lblMyRobotHeat, row, 1);
    statLayout->addWidget(lblMyRobotAmmo, row, 2);
    statLayout->addWidget(lblMyRobotPos, row, 3);
    row++;
    statLayout->addWidget(lblMyRobotState, row, 0);
    statLayout->addWidget(lblMyRobotExp, row, 1);
    statLayout->addWidget(lblMyRobotBuffer, row, 2);
    statLayout->addWidget(lblMyRobotChassisEnergy, row, 3);
    row++;
    statLayout->addWidget(lblMyRobotFire, row, 0);
    statLayout->addWidget(lblMyRobotCanHeal, row, 1);
    statLayout->addWidget(lblMyRobotCanRefill, row, 2);
    row++;
    statLayout->addWidget(lblRespawnInfo, row, 0, 1, 2);
    statLayout->addWidget(lblPenaltyInfo, row, 2, 1, 2);

    layout->addWidget(grpStatus, 0, 0, 1, 2);

    // Modules
    QGroupBox *grpMod = new QGroupBox("模块状态");
    QVBoxLayout *modLayout = new QVBoxLayout(grpMod);
    treeModules = new QTreeWidget();
    treeModules->setHeaderLabels({"模块", "状态"});
    modLayout->addWidget(treeModules);
    layout->addWidget(grpMod, 1, 0);

    // Injury
    QGroupBox *grpInj = new QGroupBox("伤害统计");
    QVBoxLayout *injLayout = new QVBoxLayout(grpInj);
    treeInjury = new QTreeWidget();
    treeInjury->setHeaderLabels({"来源", "数值"});
    injLayout->addWidget(treeInjury);
    layout->addWidget(grpInj, 1, 1);

    // Buffs
    QGroupBox *grpBuff = new QGroupBox("增益详情");
    QVBoxLayout *bufLayout = new QVBoxLayout(grpBuff);
    treeBuffs = new QTreeWidget();
    treeBuffs->setHeaderLabels({"类型", "等级", "剩余时间", "参数"});
    bufLayout->addWidget(treeBuffs);
    layout->addWidget(grpBuff, 2, 0, 1, 2);
}

void RobotStatusWidget::updateRobotDynamic(u32 hp, f32 heat, f32 fireRate, u32 chassisEnergy, u32 bufferEnergy, u32 exp, u32 upExp, u32 totalFired, u32 ammo, bool outCombat, u32 combatTime, bool canHeal, bool canRemoteAmmo)
{
    qDebug() << "updateRobotDynamic:" 
             << "hp=" << hp 
             << "heat=" << heat 
             << "fireRate=" << fireRate 
             << "chassisEnergy=" << chassisEnergy 
             << "bufferEnergy=" << bufferEnergy 
             << "exp=" << exp 
             << "upExp=" << upExp 
             << "totalFired=" << totalFired 
             << "ammo=" << ammo 
             << "outCombat=" << outCombat 
             << "combatTime=" << combatTime 
             << "canHeal=" << canHeal 
             << "canRemoteAmmo=" << canRemoteAmmo;

    if(lblMyRobotHp) lblMyRobotHp->setText(QString("血量: %1").arg(hp));
    if(lblMyRobotHeat) lblMyRobotHeat->setText(QString("热量: %1").arg(heat));
    if(lblMyRobotAmmo) lblMyRobotAmmo->setText(QString("弹丸: %1").arg(ammo));
    if(lblMyRobotState) lblMyRobotState->setText(outCombat ? QString("脱战 (%1s)").arg(combatTime) : "交战中");
    if(lblMyRobotExp) lblMyRobotExp->setText(QString("经验: %1 (升级: %2)").arg(exp).arg(upExp));
    if(lblMyRobotBuffer) lblMyRobotBuffer->setText(QString("缓冲: %1").arg(bufferEnergy));
    if(lblMyRobotChassisEnergy) lblMyRobotChassisEnergy->setText(QString("底盘能量: %1").arg(chassisEnergy));
    if(lblMyRobotFire) lblMyRobotFire->setText(QString("发射: %1 (射速: %2)").arg(totalFired).arg(fireRate));
    if(lblMyRobotCanHeal) lblMyRobotCanHeal->setText(canHeal ? "远程补血: 是" : "远程补血: 否");
    if(lblMyRobotCanRefill) lblMyRobotCanRefill->setText(canRemoteAmmo ? "远程补弹: 是" : "远程补弹: 否");
}

void RobotStatusWidget::updateRobotPosition(f32 x, f32 y, f32 z, f32 yaw)
{
    qDebug() << "updateRobotPosition:" << "x=" << x << "y=" << y << "z=" << z << "yaw=" << yaw;
    if(lblMyRobotPos) lblMyRobotPos->setText(QString("位置: (%1, %2, %3) 朝向: %4")
                                             .arg(x, 0, 'f', 2)
                                             .arg(y, 0, 'f', 2)
                                             .arg(z, 0, 'f', 2)
                                             .arg(yaw, 0, 'f', 2));
}

void RobotStatusWidget::updateRobotStatic(u32 connState, u32 fieldState, u32 aliveState, u32 id, u32 type, u32 perfShooter, u32 perfChassis, u32 level, u32 maxHp, u32 maxHeat, u32 maxPower, u32 maxChassisEnergy, u32 maxBufferEnergy, f32 heatCoolDown)
{
    qDebug() << "updateRobotStatic:" 
             << "connState=" << connState 
             << "fieldState=" << fieldState 
             << "aliveState=" << aliveState 
             << "id=" << id 
             << "type=" << type 
             << "perfShooter=" << perfShooter 
             << "perfChassis=" << perfChassis 
             << "level=" << level 
             << "maxHp=" << maxHp 
             << "maxHeat=" << maxHeat 
             << "maxPower=" << maxPower 
             << "maxChassisEnergy=" << maxChassisEnergy 
             << "maxBufferEnergy=" << maxBufferEnergy 
             << "heatCoolDown=" << heatCoolDown;

    if(lblMyRobotId) lblMyRobotId->setText(QString("ID: %1 (类型: %2)").arg(id).arg(type));
    if(lblMyRobotLevel) lblMyRobotLevel->setText(QString("等级: %1").arg(level));
    
    QString connStr = (connState == 1) ? "已连接" : "未连接";
    if(lblMyRobotConn) lblMyRobotConn->setText(QString("连接: %1").arg(connStr));
    
    QString fieldStr = (fieldState == 0) ? "已上场" : "未上场";
    if(lblMyRobotField) lblMyRobotField->setText(QString("上场: %1").arg(fieldStr));
    
    QString aliveStr;
    if(aliveState == 1) aliveStr = "存活";
    else if(aliveState == 2) aliveStr = "死亡";
    else aliveStr = "未知";
    if(lblMyRobotAlive) lblMyRobotAlive->setText(QString("存活: %1").arg(aliveStr));
    
    if(lblMyRobotPerf) lblMyRobotPerf->setText(QString("性能: 发射机构 %1, 底盘 %2").arg(perfShooter).arg(perfChassis));
    
    if(lblMyRobotMaxStats) lblMyRobotMaxStats->setText(QString("最大值: 血量 %1, 热量 %2, 功率 %3, 缓冲 %4, 底盘 %5").arg(maxHp).arg(maxHeat).arg(maxPower).arg(maxBufferEnergy).arg(maxChassisEnergy));
    
    if(lblMyRobotHeat) lblMyRobotHeat->setText(QString("热量: ? (冷却: %1)").arg(heatCoolDown)); // Update cool down part
}

void RobotStatusWidget::updateRobotInjury(u32 total, u32 collision, u32 smallProj, u32 largeProj, u32 dart, u32 moduleOff, u32 wifiOff, u32 penalty, u32 serverKill, u32 killerId)
{
    if(treeInjury) {
        treeInjury->clear();
        new QTreeWidgetItem(treeInjury, QStringList() << "累计总计" << QString::number(total));
        new QTreeWidgetItem(treeInjury, QStringList() << "撞击伤害" << QString::number(collision));
        new QTreeWidgetItem(treeInjury, QStringList() << "17mm弹丸伤害" << QString::number(smallProj));
        new QTreeWidgetItem(treeInjury, QStringList() << "42mm弹丸伤害" << QString::number(largeProj));
        new QTreeWidgetItem(treeInjury, QStringList() << "飞镖溅射伤害" << QString::number(dart));
        new QTreeWidgetItem(treeInjury, QStringList() << "模块离线扣血" << QString::number(moduleOff));
        new QTreeWidgetItem(treeInjury, QStringList() << "WiFi离线扣血" << QString::number(wifiOff));
        new QTreeWidgetItem(treeInjury, QStringList() << "判罚扣血" << QString::number(penalty));
        new QTreeWidgetItem(treeInjury, QStringList() << "服务器强制战亡" << QString::number(serverKill));
        new QTreeWidgetItem(treeInjury, QStringList() << "击杀者ID" << QString::number(killerId));
    }
}

void RobotStatusWidget::updateRobotRespawn(bool pending, u32 totalProg, u32 curProg, bool canFree, u32 goldCost, bool canPay)
{
    if(lblRespawnInfo) {
        QString txt = QString("复活: %1 (进度: %2/%3)").arg(pending ? "待复活" : "否").arg(curProg).arg(totalProg);
        txt += QString(" 免费: %1, 金币: %2 (可支付: %3)").arg(canFree ? "是" : "否").arg(goldCost).arg(canPay ? "是" : "否");
        lblRespawnInfo->setText(txt);
    }
}

void RobotStatusWidget::updateRobotModule(u32 power, u32 rfid, u32 light, u32 smallS, u32 bigS, u32 uwb, u32 armor, u32 video, u32 cap, u32 mainCtrl)
{
    if(treeModules) {
        treeModules->clear();
        auto addMod = [&](const QString &name, u32 status) {
            new QTreeWidgetItem(treeModules, QStringList() << name << (status == 1 ? "在线" : "离线"));
        };
        addMod("电源管理模块", power);
        addMod("RFID模块", rfid);
        addMod("灯条模块", light);
        addMod("小弹丸发射机构", smallS);
        addMod("大弹丸发射机构", bigS);
        addMod("UWB模块", uwb);
        addMod("装甲板模块", armor);
        addMod("图传模块", video);
        addMod("超级电容", cap);
        addMod("主控模块", mainCtrl);
    }
}

void RobotStatusWidget::updateBuff(u32 robotId, u32 type, i32 level, u32 maxTime, u32 leftTime, const QString &msgParams)
{
    if(treeBuffs) {
        // If leftTime is large, it might be persistent
        new QTreeWidgetItem(treeBuffs, QStringList() << QString::number(type) << QString::number(level) << QString::number(leftTime) << msgParams);
    }
}

void RobotStatusWidget::updatePenalty(u32 type, u32 sec, u32 total)
{
    if(lblPenaltyInfo) {
        QString typeStr;
        switch(type) {
            case 1: typeStr = "黄牌"; break;
            case 2: typeStr = "双方黄牌"; break;
            case 3: typeStr = "红牌"; break;
            case 4: typeStr = "超功率"; break;
            case 5: typeStr = "超热量"; break;
            case 6: typeStr = "超射速"; break;
            default: typeStr = QString::number(type); break;
        }
        lblPenaltyInfo->setText(QString("判罚: 类型[%1], 持续 %2秒, 总次数 %3").arg(typeStr).arg(sec).arg(total));
    }
}
