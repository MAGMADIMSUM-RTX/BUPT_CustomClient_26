#ifndef DASHBOARD_WIDGET_H
#define DASHBOARD_WIDGET_H

#include <QWidget>
#include <QLabel>
#include <QTreeWidget>
#include <QTableWidget>
#include <QGroupBox>
#include <QGridLayout>
#include "../types.h"

class DashboardWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DashboardWidget(QWidget *parent = nullptr);

public slots:
    void updateGameStatus(u32 round, u32 totalRounds, u32 redScore, u32 blueScore, u32 stage, i32 time, i32 elapsed, bool paused);
    void updateGlobalUnit(u32 baseHp, u32 baseStatus, u32 baseShield, u32 outpostHp, u32 outpostStatus, const QList<u32> &robotHp, const QList<i32> &robotBullets, u32 dmgRed, u32 dmgBlue);
    void updateGlobalLogistics(u32 economy, u64 totalEconomy, u32 techLevel, u32 encryptLevel);
    void updateGlobalSpecialMech(const QList<u32> &ids, const QList<i32> &times);
    void updateTechCore(u32 maxDiff, u32 status);
    void updateDeployMode(u32 status);
    void updateRuneStatus(u32 status, u32 arms, u32 avgRings);
    void updateSentinelStatus(u32 posture, bool weakened);
    void updateAirSupport(u32 status, u32 time, u32 cost);
    void updateDartTarget(u32 targetId, bool open);
    void updatePerfSelSync(u32 shooter, u32 chassis);
    void updateEvent(i32 eventId, const QString &param);
    void updateCustomData(const QByteArray &data);

private:
    void setupUi();

    QLabel *lblGameRound;
    QLabel *lblGameScore;
    QLabel *lblGameStage;
    QLabel *lblGameTime;
    QLabel *lblGamePaused;
    
    QLabel *lblBaseHp;
    QLabel *lblOutpostHp;
    QLabel *lblTotalDmgRed;
    QLabel *lblTotalDmgBlue;
    
    QLabel *lblEconomy;
    QLabel *lblTechLevel;
    QLabel *lblEncryptLevel;
    
    // Sync Status
    QLabel *lblTechCore;
    QLabel *lblDeployMode;
    QLabel *lblRuneStatus;
    QLabel *lblSentinelStatus;
    QLabel *lblAirSupport;
    QLabel *lblDartTarget;
    QLabel *lblPerfSel;
    QLabel *lblCustomData;
    
    QTableWidget *tblRobotHp;
    QTreeWidget *treeEvents;
    QTreeWidget *treeSpecialMech;
};

#endif // DASHBOARD_WIDGET_H
