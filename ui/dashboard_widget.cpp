#include "dashboard_widget.h"
#include <QVBoxLayout>
#include <QHeaderView>
#include <QDateTime>

DashboardWidget::DashboardWidget(QWidget *parent) : QWidget(parent)
{
    setupUi();
}

void DashboardWidget::setupUi()
{
    QGridLayout *layout = new QGridLayout(this);

    // Dashboard Group
    QGroupBox *grpDash = new QGroupBox("比赛概况");
    QGridLayout *dashLayout = new QGridLayout(grpDash);
    
    lblGameRound = new QLabel("局次: 0/0");
    lblGameScore = new QLabel("比分: 红 0 : 蓝 0");
    lblGameStage = new QLabel("阶段: 准备 (0s)");
    lblGamePaused = new QLabel("暂停: 否");
    
    lblBaseHp = new QLabel("基地血量: 0 / 0");
    lblOutpostHp = new QLabel("前哨站血量: 0");
    lblTotalDmgRed = new QLabel("红方伤害: 0");
    lblTotalDmgBlue = new QLabel("蓝方伤害: 0");
    
    lblEconomy = new QLabel("经济: 0 (总计: 0)");
    lblTechLevel = new QLabel("科技等级: 0");
    lblEncryptLevel = new QLabel("加密等级: 0");

    dashLayout->addWidget(lblGameRound, 0, 0);
    dashLayout->addWidget(lblGameScore, 0, 1);
    dashLayout->addWidget(lblGameStage, 0, 2);
    dashLayout->addWidget(lblGamePaused, 0, 3);
    
    dashLayout->addWidget(lblBaseHp, 1, 0);
    dashLayout->addWidget(lblOutpostHp, 1, 1);
    dashLayout->addWidget(lblTotalDmgRed, 1, 2);
    dashLayout->addWidget(lblTotalDmgBlue, 1, 3);
    
    dashLayout->addWidget(lblEconomy, 2, 0);
    dashLayout->addWidget(lblTechLevel, 2, 1);
    dashLayout->addWidget(lblEncryptLevel, 2, 2);

    layout->addWidget(grpDash, 0, 0, 1, 2);

    // Special Mech
    QGroupBox *grpMech = new QGroupBox("全局特殊机制");
    QGridLayout *mechLayout = new QGridLayout(grpMech);
    treeSpecialMech = new QTreeWidget();
    treeSpecialMech->setHeaderLabels({"机制ID", "时间(秒)"});

    lblTechCore = new QLabel("科技核心: N/A");
    lblDeployMode = new QLabel("部署模式: N/A");
    lblRuneStatus = new QLabel("能量机关: N/A");
    lblSentinelStatus = new QLabel("哨兵: N/A");
    lblAirSupport = new QLabel("空中支援: N/A");
    lblDartTarget = new QLabel("飞镖目标: N/A");
    lblCustomData = new QLabel("自定义数据: N/A");

    mechLayout->addWidget(treeSpecialMech, 0, 0, 4, 1);
    mechLayout->addWidget(lblTechCore, 0, 1);
    mechLayout->addWidget(lblDeployMode, 0, 2);
    mechLayout->addWidget(lblRuneStatus, 1, 1);
    mechLayout->addWidget(lblSentinelStatus, 1, 2);
    mechLayout->addWidget(lblAirSupport, 2, 1);
    mechLayout->addWidget(lblDartTarget, 2, 2);
    mechLayout->addWidget(lblCustomData, 3, 1, 1, 2);

    layout->addWidget(grpMech, 1, 0);

    // Event Log
    QGroupBox *grpEvents = new QGroupBox("赛场事件");
    QVBoxLayout *evtLayout = new QVBoxLayout(grpEvents);
    treeEvents = new QTreeWidget();
    treeEvents->setHeaderLabels({"时间", "事件"});
    evtLayout->addWidget(treeEvents);
    layout->addWidget(grpEvents, 2, 0);

    // Robot HP Table
    QGroupBox *grpHp = new QGroupBox("全场机器人血量");
    QVBoxLayout *hpLayout = new QVBoxLayout(grpHp);
    tblRobotHp = new QTableWidget(0, 3);
    tblRobotHp->setHorizontalHeaderLabels({"ID", "血量", "弹量"});
    hpLayout->addWidget(tblRobotHp);
    layout->addWidget(grpHp, 1, 1, 2, 1); // Span 2 rows
}

void DashboardWidget::updateGameStatus(u32 round, u32 totalRounds, u32 redScore, u32 blueScore, u32 stage, i32 time, i32 elapsed, bool paused)
{
    if(lblGameRound) lblGameRound->setText(QString("局次: %1/%2").arg(round).arg(totalRounds));
    if(lblGameScore) lblGameScore->setText(QString("比分: 红 %1 : 蓝 %2").arg(redScore).arg(blueScore));
    
    QString stageStr;
    switch(stage) {
        case 0: stageStr = "未开始比赛"; break;
        case 1: stageStr = "准备阶段"; break;
        case 2: stageStr = "自检阶段"; break;
        case 3: stageStr = "倒计时"; break;
        case 4: stageStr = "比赛中"; break;
        case 5: stageStr = "比赛结算"; break;
        default: stageStr = "未知"; break;
    }
    if(lblGameStage) lblGameStage->setText(QString("阶段: %1 (%2s)").arg(stageStr).arg(time));
    if(lblGamePaused) lblGamePaused->setText(QString("暂停: %1").arg(paused ? "是" : "否"));
}

void DashboardWidget::updateGlobalUnit(u32 baseHp, u32 baseStatus, u32 baseShield, u32 outpostHp, u32 outpostStatus, const QList<u32> &robotHp, const QList<i32> &robotBullets, u32 dmgRed, u32 dmgBlue)
{
    if(lblBaseHp) lblBaseHp->setText(QString("基地血量: %1 (盾: %2)").arg(baseHp).arg(baseShield));
    if(lblOutpostHp) lblOutpostHp->setText(QString("前哨站血量: %1").arg(outpostHp));
    if(lblTotalDmgRed) lblTotalDmgRed->setText(QString("红方伤害: %1").arg(dmgRed));
    if(lblTotalDmgBlue) lblTotalDmgBlue->setText(QString("蓝方伤害: %1").arg(dmgBlue));
    
    // Update Table
    if(tblRobotHp) {
        tblRobotHp->setRowCount(robotHp.size());
        for(int i=0; i<robotHp.size(); i++) {
            tblRobotHp->setItem(i, 0, new QTableWidgetItem(QString::number(i)));
            tblRobotHp->setItem(i, 1, new QTableWidgetItem(QString::number(robotHp[i])));
            if(i < robotBullets.size()) {
                tblRobotHp->setItem(i, 2, new QTableWidgetItem(QString::number(robotBullets[i])));
            }
        }
    }
}

void DashboardWidget::updateGlobalLogistics(u32 economy, u64 totalEconomy, u32 techLevel, u32 encryptLevel)
{
    if(lblEconomy) lblEconomy->setText(QString("经济: %1 (总计: %2)").arg(economy).arg(totalEconomy));
    if(lblTechLevel) lblTechLevel->setText(QString("科技等级: %1").arg(techLevel));
    if(lblEncryptLevel) lblEncryptLevel->setText(QString("加密等级: %1").arg(encryptLevel));
}

void DashboardWidget::updateGlobalSpecialMech(const QList<u32> &ids, const QList<i32> &times)
{
    if(treeSpecialMech) {
        treeSpecialMech->clear();
        for(int i=0; i<ids.size() && i<times.size(); i++) {
            QTreeWidgetItem *item = new QTreeWidgetItem(treeSpecialMech);
            item->setText(0, QString::number(ids[i]));
            item->setText(1, QString::number(times[i]));
        }
    }
}

void DashboardWidget::updateTechCore(u32 maxDiff, u32 status)
{
    if(lblTechCore) lblTechCore->setText(QString("科技核心: 状态%1 (最高难度%2)").arg(status).arg(maxDiff));
}

void DashboardWidget::updateDeployMode(u32 status)
{
    if(lblDeployMode) lblDeployMode->setText(QString("部署模式: %1").arg(status == 1 ? "已部署" : "未部署"));
}

void DashboardWidget::updateRuneStatus(u32 status, u32 arms, u32 avgRings)
{
    QString s = (status == 1 ? "未激活" : (status == 2 ? "激活中" : "已激活"));
    if(lblRuneStatus) lblRuneStatus->setText(QString("能量机关: %1 (臂:%2 环:%3)").arg(s).arg(arms).arg(avgRings));
}

void DashboardWidget::updateSentinelStatus(u32 posture, bool weakened)
{
    if(lblSentinelStatus) lblSentinelStatus->setText(QString("哨兵: 姿态%1 %2").arg(posture).arg(weakened ? "(虚弱)" : ""));
}

void DashboardWidget::updateAirSupport(u32 status, u32 time, u32 cost)
{
    if(lblAirSupport) lblAirSupport->setText(QString("空中支援: %1 (CD:%2s 消耗:%3)").arg(status).arg(time).arg(cost));
}

void DashboardWidget::updateDartTarget(u32 targetId, bool open)
{
    if(lblDartTarget) lblDartTarget->setText(QString("飞镖目标: %1 (闸门:%2)").arg(targetId).arg(open ? "开" : "关"));
}

void DashboardWidget::updatePerfSelSync(u32 shooter, u32 chassis)
{
    QString sStr, cStr;
    switch(shooter) {
        case 1: sStr = "冷却优先"; break;
        case 2: sStr = "爆发优先"; break;
        case 3: sStr = "英雄近战"; break;
        case 4: sStr = "英雄远程"; break;
        default: sStr = "未知(" + QString::number(shooter) + ")"; break;
    }
    switch(chassis) {
        case 1: cStr = "血量优先"; break;
        case 2: cStr = "功率优先"; break;
        case 3: cStr = "英雄近战"; break;
        case 4: cStr = "英雄远程"; break;
        default: cStr = "未知(" + QString::number(chassis) + ")"; break;
    }
    if(lblPerfSel) lblPerfSel->setText(QString("性能体系: 发射[%1] 底盘[%2]").arg(sStr).arg(cStr));
}

void DashboardWidget::updateEvent(i32 eventId, const QString &param)
{
    QString desc;
    switch(eventId) {
        case 1: desc = "击杀"; break;
        case 2: desc = "基地/前哨站摧毁"; break;
        case 3: desc = "能量机关次数变化"; break;
        case 4: desc = "能量单元可激活"; break;
        case 5: desc = "能量机关激活成功"; break;
        case 6: desc = "能量机关被激活"; break;
        case 7: desc = "己方英雄部署"; break;
        case 8: desc = "己方英雄狙击伤害"; break;
        case 9: desc = "对方英雄狙击伤害"; break;
        case 10: desc = "己方呼叫空援"; break;
        case 11: desc = "己方空援打断"; break;
        case 12: desc = "对方呼叫空援"; break;
        case 13: desc = "对方空援打断"; break;
        case 14: desc = "飞镖命中"; break;
        case 15: desc = "飞镖闸门开启"; break;
        case 16: desc = "基地遭攻击"; break;
        case 17: desc = "前哨站停转"; break;
        case 18: desc = "基地护甲展开"; break;
        default: desc = "未知事件"; break;
    }

    if(treeEvents) {
        QTreeWidgetItem *item = new QTreeWidgetItem(treeEvents);
        item->setText(0, QDateTime::currentDateTime().toString("HH:mm:ss"));
        item->setText(1, QString("[%1] %2 (Param: %3)").arg(eventId).arg(desc).arg(param));
        treeEvents->scrollToBottom();
    }
}

void DashboardWidget::updateCustomData(const QByteArray &data)
{
    if(lblCustomData) {
        lblCustomData->setText(QString("自定义数据: %1 (Size=%2)").arg(QString(data.toHex())).arg(data.size()));
    }
}
