#ifndef COMMAND_CONSOLE_WIDGET_H
#define COMMAND_CONSOLE_WIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QGroupBox>
#include <QCheckBox>
#include <QFormLayout>
#include <QGridLayout>

class CommandConsoleWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CommandConsoleWidget(QWidget *parent = nullptr);

signals:
    void sigSendMapClick(uint32_t sendAll, const QByteArray &robotId, uint32_t mode, uint32_t enemyId, uint32_t ascii, uint32_t type, uint32_t sx, uint32_t sy, float mx, float my);
    void sigSendAssembly(uint32_t operation, uint32_t difficulty);
    void sigSendPerfSel(uint32_t shooter, uint32_t chassis);
    void sigSendHeroDeploy(uint32_t mode);
    void sigSendRuneActivate(uint32_t activate);
    void sigSendDart(uint32_t targetId, bool open);
    void sigSendGuardCtrl(uint32_t commandId);
    void sigSendAirSupport(uint32_t commandId);
    // Remote Control
    void sigSendRemoteControl(int mx, int my, int mz, bool left, bool right, uint32_t keys, bool mid, const QByteArray &data);

private slots:
    void onBtnMapClick();
    void onBtnAssembly();
    void onBtnPerf();
    void onBtnHero();
    void onBtnRune();
    void onBtnDart();
    void onBtnGuard();
    void onBtnAir();
    void onBtnRemoteControl();

private:
    void setupUi();

    // Remote Control
    QSpinBox *spnRcMx, *spnRcMy, *spnRcMz;
    QCheckBox *chkRcLeft, *chkRcRight, *chkRcMid;
    QLineEdit *edtRcKeys;
    QLineEdit *edtRcData;
    QPushButton *btnSendRemoteControl;

    // Map Click
    QLineEdit *edtMapRobotId;
    QComboBox *cmbMapSendAll; // Added
    QComboBox *cmbMapMode;
    QSpinBox *spnMapEnemyId; // Added
    QSpinBox *spnMapAscii; // Added
    QComboBox *cmbMapType; // Added
    QSpinBox *spnMapSx, *spnMapSy; // Screen X/Y
    QDoubleSpinBox *spnMapMx, *spnMapMy; // Map X/Y
    QPushButton *btnSendMapClick;

    // Assembly
    QComboBox *cmbAssemblyOp;
    QSpinBox *spnAssemblyDiff;
    QPushButton *btnSendAssembly;

    // Perf Selection
    QComboBox *cmbPerfShooter;
    QComboBox *cmbPerfChassis;
    QPushButton *btnSendPerf;

    // Hero Deploy
    QComboBox *cmbHeroMode;
    QPushButton *btnSendHero;

    // Rune
    QComboBox *cmbRuneAct;
    QPushButton *btnSendRune;

    // Dart
    QSpinBox *spnDartTarget;
    QComboBox *cmbDartOpen;
    QPushButton *btnSendDart;

    // Guard & Air
    QComboBox *cmbGuardCmd;
    QPushButton *btnSendGuard;
    QComboBox *cmbAirCmd;
    QPushButton *btnSendAir;
};

#endif // COMMAND_CONSOLE_WIDGET_H
