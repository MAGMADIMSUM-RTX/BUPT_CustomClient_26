#include "web_bridge.h"
#include "rm_client.h"

WebBridge::WebBridge(RMClient *client, QObject *parent) : QObject(parent), m_client(client)
{
}

void WebBridge::updateRobotStatus(int hp, int maxHp, int ammo, float heat, int id)
{
    if (m_robotHp != hp) {
        m_robotHp = hp;
        emit robotHpChanged(hp);
    }
    if (m_maxHp != maxHp) {
        m_maxHp = maxHp;
        emit maxHpChanged(maxHp);
    }
    if (m_robotAmmo != ammo) {
        m_robotAmmo = ammo;
        emit robotAmmoChanged(ammo);
    }
    if (!qFuzzyCompare(m_robotHeat, heat)) {
        m_robotHeat = heat;
        emit robotHeatChanged(heat);
    }
    if (m_robotId != id && id != 0) {
        m_robotId = id;
        emit robotIdChanged(id);
    }
}

void WebBridge::updateRobotPosition(float x, float y, float yaw)
{
    if (!qFuzzyCompare(m_robotX, x)) {
        m_robotX = x;
        emit robotXChanged(x);
    }
    if (!qFuzzyCompare(m_robotY, y)) {
        m_robotY = y;
        emit robotYChanged(y);
    }
    if (!qFuzzyCompare(m_robotYaw, yaw)) {
        m_robotYaw = yaw;
        emit robotYawChanged(yaw);
    }
}

void WebBridge::updateGlobalUnitStatus(unsigned int baseHp, unsigned int outpostHp, const QList<unsigned int> &robotHp, const QList<int> &robotAmmo)
{
    if (m_allyBaseHp != static_cast<int>(baseHp)) {
        m_allyBaseHp = static_cast<int>(baseHp);
        emit allyBaseHpChanged(m_allyBaseHp);
    }
    if (m_allyOutpostHp != static_cast<int>(outpostHp)) {
        m_allyOutpostHp = static_cast<int>(outpostHp);
        emit allyOutpostHpChanged(m_allyOutpostHp);
    }

    QVariantList newList;
    for (unsigned int hp : robotHp) {
        newList.append(hp);
    }
    if (m_allRobotHp != newList) {
        m_allRobotHp = newList;
        emit allRobotHpChanged(newList);
    }

    QVariantList newAmmoList;
    for (int ammo : robotAmmo) {
        newAmmoList.append(ammo);
    }
    if (m_allRobotAmmo != newAmmoList) {
        m_allRobotAmmo = newAmmoList;
        emit allRobotAmmoChanged(newAmmoList);
    }
}

void WebBridge::updateGameStatus(int round, int totalRounds, int redScore, int blueScore, int time)
{
    if (m_currentRound != round) {
        m_currentRound = round;
        emit currentRoundChanged(round);
    }
    if (m_totalRounds != totalRounds) {
        m_totalRounds = totalRounds;
        emit totalRoundsChanged(totalRounds);
    }
    if (m_redScore != redScore) {
        m_redScore = redScore;
        emit redScoreChanged(redScore);
    }
    if (m_blueScore != blueScore) {
        m_blueScore = blueScore;
        emit blueScoreChanged(blueScore);
    }
    if (m_gameTime != time) {
        m_gameTime = time;
        emit gameTimeChanged(time);
    }
}

void WebBridge::sendAirSupport(int commandId)
{
    m_client->sendAirSupport(commandId);
}

void WebBridge::sendCommonCommand(int type, int param)
{
    m_client->sendCommonCommand(type, param);
}
