#include "command_console_widget.h"
#include <QDebug>

CommandConsoleWidget::CommandConsoleWidget(QWidget *parent) : QWidget(parent)
{
    setupUi();
}

void CommandConsoleWidget::setupUi()
{
    QGridLayout *layout = new QGridLayout(this);

    // Map Interaction
    QGroupBox *grpMap = new QGroupBox("地图交互");
    QFormLayout *mapLayout = new QFormLayout(grpMap);
    
    cmbMapSendAll = new QComboBox();
    cmbMapSendAll->addItems({"指定客户端", "除哨兵", "包含哨兵"});
    
    edtMapRobotId = new QLineEdit("0");
    edtMapRobotId->setPlaceholderText("目标ID列表 (Space separated)");
    
    cmbMapMode = new QComboBox();
    cmbMapMode->addItems({"无", "攻击", "防御", "警戒", "自定义"}); 
    
    spnMapEnemyId = new QSpinBox();
    spnMapEnemyId->setRange(0, 200);
    
    spnMapAscii = new QSpinBox();
    spnMapAscii->setRange(0, 255);
    
    cmbMapType = new QComboBox();
    cmbMapType->addItems({"无", "地图", "对方机器人"});
    
    spnMapSx = new QSpinBox(); spnMapSx->setRange(0, 10000); spnMapSx->setPrefix("Screen X:");
    spnMapSy = new QSpinBox(); spnMapSy->setRange(0, 10000); spnMapSy->setPrefix("Screen Y:");
    spnMapMx = new QDoubleSpinBox(); spnMapMx->setRange(0, 30.0); spnMapMx->setPrefix("Map X:");
    spnMapMy = new QDoubleSpinBox(); spnMapMy->setRange(0, 20.0); spnMapMy->setPrefix("Map Y:");
    
    btnSendMapClick = new QPushButton("发送点击指令");
    connect(btnSendMapClick, &QPushButton::clicked, this, &CommandConsoleWidget::onBtnMapClick);
    
    mapLayout->addRow("范围:", cmbMapSendAll);
    mapLayout->addRow("目标机器人:", edtMapRobotId);
    mapLayout->addRow("模式:", cmbMapMode);
    mapLayout->addRow("敌方ID:", spnMapEnemyId);
    mapLayout->addRow("ASCII:", spnMapAscii);
    mapLayout->addRow("标记类型:", cmbMapType);
    mapLayout->addRow(spnMapSx, spnMapSy);
    mapLayout->addRow(spnMapMx, spnMapMy);
    mapLayout->addRow(btnSendMapClick);
    layout->addWidget(grpMap, 0, 0);

    // Assembly
    QGroupBox *grpAss = new QGroupBox("工程装配指令");
    QFormLayout *assLayout = new QFormLayout(grpAss);
    cmbAssemblyOp = new QComboBox();
    cmbAssemblyOp->addItems({"确认装配", "取消装配"});
    spnAssemblyDiff = new QSpinBox();
    btnSendAssembly = new QPushButton("发送装配指令");
    connect(btnSendAssembly, &QPushButton::clicked, this, &CommandConsoleWidget::onBtnAssembly);

    assLayout->addRow("操作:", cmbAssemblyOp);
    assLayout->addRow("难度:", spnAssemblyDiff);
    assLayout->addRow(btnSendAssembly);
    layout->addWidget(grpAss, 0, 1);

    // Perf Selection
    QGroupBox *grpPerf = new QGroupBox("性能体系选择");
    QFormLayout *perfLayout = new QFormLayout(grpPerf);
    cmbPerfShooter = new QComboBox(); cmbPerfShooter->addItems({"冷却优先", "爆发优先", "英雄近战优先", "英雄远程优先"});
    cmbPerfChassis = new QComboBox(); cmbPerfChassis->addItems({"血量优先", "功率优先", "英雄近战优先", "英雄远程优先"});
    btnSendPerf = new QPushButton("发送性能配置");
    connect(btnSendPerf, &QPushButton::clicked, this, &CommandConsoleWidget::onBtnPerf);

    perfLayout->addRow("发射机构:", cmbPerfShooter);
    perfLayout->addRow("底盘:", cmbPerfChassis);
    perfLayout->addRow(btnSendPerf);
    layout->addWidget(grpPerf, 1, 0);

    // Hero
    QGroupBox *grpHero = new QGroupBox("英雄部署模式");
    QFormLayout *heroLayout = new QFormLayout(grpHero);
    cmbHeroMode = new QComboBox(); cmbHeroMode->addItems({"退出", "进入"});
    btnSendHero = new QPushButton("发送英雄指令");
    connect(btnSendHero, &QPushButton::clicked, this, &CommandConsoleWidget::onBtnHero);

    heroLayout->addRow("模式:", cmbHeroMode);
    heroLayout->addRow(btnSendHero);
    layout->addWidget(grpHero, 1, 1);

    // Rune
    QGroupBox *grpRune = new QGroupBox("能量机关激活");
    QFormLayout *runeLayout = new QFormLayout(grpRune);
    cmbRuneAct = new QComboBox(); cmbRuneAct->addItems({"开启"});
    btnSendRune = new QPushButton("发送激活指令");
    connect(btnSendRune, &QPushButton::clicked, this, &CommandConsoleWidget::onBtnRune);

    runeLayout->addRow(btnSendRune);
    layout->addWidget(grpRune, 2, 0);

    // Dart
    QGroupBox *grpDart = new QGroupBox("飞镖控制");
    QFormLayout *dartLayout = new QFormLayout(grpDart);
    spnDartTarget = new QSpinBox();
    cmbDartOpen = new QComboBox(); cmbDartOpen->addItems({"开启", "关闭"});
    btnSendDart = new QPushButton("发送飞镖指令");
    connect(btnSendDart, &QPushButton::clicked, this, &CommandConsoleWidget::onBtnDart);

    dartLayout->addRow("目标ID:", spnDartTarget);
    dartLayout->addRow("闸门:", cmbDartOpen);
    dartLayout->addRow(btnSendDart);
    layout->addWidget(grpDart, 2, 1);
    
    // Guard & Air Support & Custom
    QGroupBox *grpExtra = new QGroupBox("其他指令");
    QFormLayout *extraLayout = new QFormLayout(grpExtra);
    
    cmbGuardCmd = new QComboBox(); 
    cmbGuardCmd->addItems({
        "0: 无效", 
        "1: 补血点补弹", 
        "2: 补给站实体补弹", 
        "3: 远程补弹", 
        "4: 远程回血", 
        "5: 确认复活", 
        "6: 确认花费金币复活", 
        "7: 地图标点", 
        "8: 切换为进攻姿态", 
        "9: 切换为防御姿态", 
        "10: 切换为移动姿态"
    });
    
    btnSendGuard = new QPushButton("哨兵指令");
    connect(btnSendGuard, &QPushButton::clicked, this, &CommandConsoleWidget::onBtnGuard);
    
    cmbAirCmd = new QComboBox(); 
    cmbAirCmd->addItems({
        "1: 免费呼叫空中支援", 
        "2: 花费金币呼叫空中支援", 
        "3: 中断空中支援"
    });
    
    btnSendAir = new QPushButton("空中支援");
    connect(btnSendAir, &QPushButton::clicked, this, &CommandConsoleWidget::onBtnAir);
    
    extraLayout->addRow("哨兵:", cmbGuardCmd);
    extraLayout->addRow(btnSendGuard);
    extraLayout->addRow("空援:", cmbAirCmd);
    extraLayout->addRow(btnSendAir);
    
    layout->addWidget(grpExtra, 0, 2, 3, 1);

    // Remote Control
    QGroupBox *grpRc = new QGroupBox("遥控");
    QGridLayout *rcLayout = new QGridLayout(grpRc);
    
    spnRcMx = new QSpinBox(); spnRcMx->setRange(-10000, 10000); spnRcMx->setPrefix("X:");
    spnRcMy = new QSpinBox(); spnRcMy->setRange(-10000, 10000); spnRcMy->setPrefix("Y:");
    spnRcMz = new QSpinBox(); spnRcMz->setRange(-10000, 10000); spnRcMz->setPrefix("Z:");
    
    chkRcLeft = new QCheckBox("左");
    chkRcRight = new QCheckBox("右");
    chkRcMid = new QCheckBox("中");
    
    edtRcKeys = new QLineEdit("0"); edtRcKeys->setPlaceholderText("按键 (整数)");
    edtRcData = new QLineEdit(); edtRcData->setPlaceholderText("数据 (Hex/Str)");
    
    btnSendRemoteControl = new QPushButton("发送遥控指令");
    connect(btnSendRemoteControl, &QPushButton::clicked, this, &CommandConsoleWidget::onBtnRemoteControl);
    
    rcLayout->addWidget(spnRcMx, 0, 0);
    rcLayout->addWidget(spnRcMy, 0, 1);
    rcLayout->addWidget(spnRcMz, 0, 2);
    rcLayout->addWidget(chkRcLeft, 0, 3);
    rcLayout->addWidget(chkRcRight, 0, 4);
    rcLayout->addWidget(chkRcMid, 0, 5);
    rcLayout->addWidget(edtRcKeys, 1, 0, 1, 2);
    rcLayout->addWidget(edtRcData, 1, 2, 1, 2);
    rcLayout->addWidget(btnSendRemoteControl, 1, 4, 1, 2);
    
    layout->addWidget(grpRc, 3, 0, 1, 3);
}

void CommandConsoleWidget::onBtnMapClick()
{
    // Parse Robot IDs (Space separated integers)
    // Protocol requires fixed 7 bytes for robot_id list
    QString idText = edtMapRobotId->text();
    QByteArray robotIds;
    QStringList parts = idText.split(' ', Qt::SkipEmptyParts);
    for (const QString &part : parts) {
        robotIds.append((char)part.toUInt());
    }
    while (robotIds.size() < 7) {
        robotIds.append((char)0);
    }
    if (robotIds.size() > 7) {
        robotIds = robotIds.left(7);
    }

    emit sigSendMapClick(
        cmbMapSendAll->currentIndex(), 
        robotIds, 
        cmbMapMode->currentIndex(), 
        spnMapEnemyId->value(), 
        spnMapAscii->value(), 
        cmbMapType->currentIndex(), 
        spnMapSx->value(), 
        spnMapSy->value(), 
        (float)spnMapMx->value(), 
        (float)spnMapMy->value()
    );
}

void CommandConsoleWidget::onBtnAssembly()
{
    emit sigSendAssembly(cmbAssemblyOp->currentIndex(), spnAssemblyDiff->value());
}

void CommandConsoleWidget::onBtnPerf()
{
    // Protobuf enum starts at 1, ComboBox index starts at 0
    emit sigSendPerfSel(cmbPerfShooter->currentIndex() + 1, cmbPerfChassis->currentIndex() + 1);
}

void CommandConsoleWidget::onBtnHero()
{
    emit sigSendHeroDeploy(cmbHeroMode->currentIndex());
}

void CommandConsoleWidget::onBtnRune()
{
    emit sigSendRuneActivate(cmbRuneAct->currentIndex());
}

void CommandConsoleWidget::onBtnDart()
{
    emit sigSendDart(spnDartTarget->value(), cmbDartOpen->currentIndex() == 0);
}

void CommandConsoleWidget::onBtnGuard()
{
    emit sigSendGuardCtrl(cmbGuardCmd->currentIndex());
}

void CommandConsoleWidget::onBtnAir()
{
    emit sigSendAirSupport(cmbAirCmd->currentIndex() + 1);
}

void CommandConsoleWidget::onBtnRemoteControl()
{
    uint32_t keys = edtRcKeys->text().toUInt();
    emit sigSendRemoteControl(
        spnRcMx->value(),
        spnRcMy->value(),
        spnRcMz->value(),
        chkRcLeft->isChecked(),
        chkRcRight->isChecked(),
        keys,
        chkRcMid->isChecked(),
        edtRcData->text().toUtf8()
    );
}
