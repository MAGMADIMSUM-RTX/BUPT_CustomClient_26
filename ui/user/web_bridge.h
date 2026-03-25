#ifndef WEB_BRIDGE_H
#define WEB_BRIDGE_H

#include <QObject>

class RMClient;

class WebBridge : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int robotHp READ robotHp NOTIFY robotHpChanged)
    Q_PROPERTY(int maxHp READ maxHp NOTIFY maxHpChanged)
    Q_PROPERTY(int robotId READ robotId NOTIFY robotIdChanged)
    Q_PROPERTY(int robotAmmo READ robotAmmo NOTIFY robotAmmoChanged)
    Q_PROPERTY(float robotHeat READ robotHeat NOTIFY robotHeatChanged)
    Q_PROPERTY(float robotX READ robotX NOTIFY robotXChanged)
    Q_PROPERTY(float robotY READ robotY NOTIFY robotYChanged)
    Q_PROPERTY(float robotYaw READ robotYaw NOTIFY robotYawChanged)
    Q_PROPERTY(QVariantList allRobotHp READ allRobotHp NOTIFY allRobotHpChanged)
    Q_PROPERTY(QVariantList allRobotAmmo READ allRobotAmmo NOTIFY allRobotAmmoChanged)
    Q_PROPERTY(int allyBaseHp READ allyBaseHp NOTIFY allyBaseHpChanged)
    Q_PROPERTY(int allyOutpostHp READ allyOutpostHp NOTIFY allyOutpostHpChanged)
    Q_PROPERTY(int gameTime READ gameTime NOTIFY gameTimeChanged)
    Q_PROPERTY(int currentRound READ currentRound NOTIFY currentRoundChanged)
    Q_PROPERTY(int totalRounds READ totalRounds NOTIFY totalRoundsChanged)
    Q_PROPERTY(int redScore READ redScore NOTIFY redScoreChanged)
    Q_PROPERTY(int blueScore READ blueScore NOTIFY blueScoreChanged)

public:
    explicit WebBridge(RMClient *client, QObject *parent = nullptr);

    int robotHp() const { return m_robotHp; }
    int maxHp() const { return m_maxHp; }
    int robotId() const { return m_robotId; }
    int robotAmmo() const { return m_robotAmmo; }
    float robotHeat() const { return m_robotHeat; }
    float robotX() const { return m_robotX; }
    float robotY() const { return m_robotY; }
    float robotYaw() const { return m_robotYaw; }
    QVariantList allRobotHp() const { return m_allRobotHp; }
    QVariantList allRobotAmmo() const { return m_allRobotAmmo; }
    int gameTime() const { return m_gameTime; }
    int currentRound() const { return m_currentRound; }
    int totalRounds() const { return m_totalRounds; }
    int redScore() const { return m_redScore; }
    int blueScore() const { return m_blueScore; }
    int allyBaseHp() const { return m_allyBaseHp; }
    int allyOutpostHp() const { return m_allyOutpostHp; }

public slots:
    void updateRobotStatus(int hp, int maxHp, int ammo, float heat, int id);
    void updateRobotPosition(float x, float y, float yaw);
    void updateGlobalUnitStatus(unsigned int baseHp, unsigned int outpostHp, const QList<unsigned int> &robotHp, const QList<int> &robotAmmo);
    void updateGameStatus(int round, int totalRounds, int redScore, int blueScore, int time);

    // 2.2.33 AirSupportStatusSync
    Q_INVOKABLE void sendAirSupport(int commandId);

    Q_INVOKABLE void sendCommonCommand(int type, int param);

signals:
    // --- Decoupled Signals for UI Updates (Proxy from RMClient) ---
    void robotHpChanged(int hp);
    void maxHpChanged(int maxHp);
    void robotIdChanged(int id);
    void robotAmmoChanged(int ammo);
    void robotHeatChanged(float heat);
    void robotXChanged(float x);
    void robotYChanged(float y);
    void robotYawChanged(float yaw);
    void allRobotHpChanged(QVariantList allRobotHp);
    void allRobotAmmoChanged(QVariantList allRobotAmmo);
    void allyBaseHpChanged(int hp);
    void allyOutpostHpChanged(int hp);
    void gameTimeChanged(int time);
    void currentRoundChanged(int round);
    void totalRoundsChanged(int totalRounds);
    void redScoreChanged(int score);
    void blueScoreChanged(int score);

private:
    int m_robotHp = 0;
    int m_maxHp = 0;
    int m_robotId = 0;
    int m_robotAmmo = 0;
    float m_robotHeat = 0.0f;
    float m_robotX = 0.0f;
    float m_robotY = 0.0f;
    float m_robotYaw = 0.0f;
    QVariantList m_allRobotHp;
    QVariantList m_allRobotAmmo;
    int m_allyBaseHp = 0;
    int m_allyOutpostHp = 0;
    int m_gameTime = 0;
    int m_currentRound = 0;
    int m_totalRounds = 0;
    int m_redScore = 0;
    int m_blueScore = 0;
    RMClient *m_client;
};

#endif // WEB_BRIDGE_H
