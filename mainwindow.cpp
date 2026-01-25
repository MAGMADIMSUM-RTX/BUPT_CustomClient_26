#include "mainwindow.h"
#include <QVBoxLayout>
#include <QDebug>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    m_rmClient = new RMClient(this);
    m_videoDecoder = new VideoDecoder(this);
    m_logBridge = new LogBridge(this);

    setupUi();
    
    // Connect Signals to DashboardWidget
    connect(m_rmClient, &RMClient::sigGameStatus, m_dashboardWidget, &DashboardWidget::updateGameStatus);
    connect(m_rmClient, &RMClient::sigGlobalUnitStatus, m_dashboardWidget, &DashboardWidget::updateGlobalUnit);
    connect(m_rmClient, &RMClient::sigGlobalLogistics, m_dashboardWidget, &DashboardWidget::updateGlobalLogistics);
    connect(m_rmClient, &RMClient::sigGlobalSpecialMech, m_dashboardWidget, &DashboardWidget::updateGlobalSpecialMech);
    connect(m_rmClient, &RMClient::sigTechCoreState, m_dashboardWidget, &DashboardWidget::updateTechCore);
    connect(m_rmClient, &RMClient::sigDeployModeSync, m_dashboardWidget, &DashboardWidget::updateDeployMode);
    connect(m_rmClient, &RMClient::sigRuneStatus, m_dashboardWidget, &DashboardWidget::updateRuneStatus);
    connect(m_rmClient, &RMClient::sigSentinelStatus, m_dashboardWidget, &DashboardWidget::updateSentinelStatus);
    connect(m_rmClient, &RMClient::sigAirSupportSync, m_dashboardWidget, &DashboardWidget::updateAirSupport);
    connect(m_rmClient, &RMClient::sigDartTargetSync, m_dashboardWidget, &DashboardWidget::updateDartTarget);
    connect(m_rmClient, &RMClient::sigPerfSelSync, m_dashboardWidget, &DashboardWidget::updatePerfSelSync);
    connect(m_rmClient, &RMClient::sigEvent, m_dashboardWidget, &DashboardWidget::updateEvent);
    connect(m_rmClient, &RMClient::sigCustomData, m_dashboardWidget, &DashboardWidget::updateCustomData);

    // Connect Signals to RobotStatusWidget
    connect(m_rmClient, &RMClient::sigRobotDynamic, m_robotStatusWidget, &RobotStatusWidget::updateRobotDynamic);
    connect(m_rmClient, &RMClient::sigRobotPosition, m_robotStatusWidget, &RobotStatusWidget::updateRobotPosition);
    connect(m_rmClient, &RMClient::sigRobotStatic, m_robotStatusWidget, &RobotStatusWidget::updateRobotStatic);
    connect(m_rmClient, &RMClient::sigRobotInjury, m_robotStatusWidget, &RobotStatusWidget::updateRobotInjury);
    connect(m_rmClient, &RMClient::sigRobotRespawn, m_robotStatusWidget, &RobotStatusWidget::updateRobotRespawn);
    connect(m_rmClient, &RMClient::sigPenalty, m_robotStatusWidget, &RobotStatusWidget::updatePenalty);
    connect(m_rmClient, &RMClient::sigRobotModule, m_robotStatusWidget, &RobotStatusWidget::updateRobotModule);
    connect(m_rmClient, &RMClient::sigBuff, m_robotStatusWidget, &RobotStatusWidget::updateBuff);

    // Connect Signals to MapWidget
    connect(m_rmClient, &RMClient::sigRobotPathPlan, m_mapWidget, &MapWidget::updateRobotPathPlan);
    connect(m_rmClient, &RMClient::sigRadarInfo, m_mapWidget, &MapWidget::updateRadarInfo);

    // Connect Signals to LogBridge (Logic)
    connect(m_rmClient, &RMClient::sigPacketLog, m_logBridge, &LogBridge::onPacketLog);
    connect(m_rmClient, &RMClient::sigConnectionStateChanged, m_logBridge, &LogBridge::onConnectionStateChanged);
    connect(m_videoDecoder, &VideoDecoder::logMessage, m_logBridge, &LogBridge::onVideoLog);

    // Connect Signals to ConnectionStatusWidget (UI)
    connect(m_rmClient, &RMClient::sigConnectionStateChanged, m_connectionStatusWidget, &ConnectionStatusWidget::onConnectionStateChanged);
    connect(m_rmClient, &RMClient::sigPacketLog, this, [this](LogLevel, bool isSend, const QString&, const QString&) {
        if (m_connectionStatusWidget) {
            m_connectionStatusWidget->updateActivity(isSend);
        }
    });
    // Connect Video Signals
    connect(m_videoDecoder, &VideoDecoder::frameReady, this, &MainWindow::updateVideoFrame);

    // Connect CommandConsoleWidget Signals to RMClient
    connect(m_commandConsoleWidget, &CommandConsoleWidget::sigSendMapClick, m_rmClient, &RMClient::sendMapClickInfo);
    connect(m_commandConsoleWidget, &CommandConsoleWidget::sigSendAssembly, m_rmClient, &RMClient::sendAssemblyCommand);
    connect(m_commandConsoleWidget, &CommandConsoleWidget::sigSendPerfSel, m_rmClient, &RMClient::sendRobotPerformanceSelection);
    connect(m_commandConsoleWidget, &CommandConsoleWidget::sigSendHeroDeploy, m_rmClient, &RMClient::sendHeroDeployMode);
    connect(m_commandConsoleWidget, &CommandConsoleWidget::sigSendRuneActivate, m_rmClient, &RMClient::sendRuneActivate);
    connect(m_commandConsoleWidget, &CommandConsoleWidget::sigSendDart, m_rmClient, &RMClient::sendDartCommand);
    connect(m_commandConsoleWidget, &CommandConsoleWidget::sigSendGuardCtrl, m_rmClient, &RMClient::sendGuardCtrl);
    connect(m_commandConsoleWidget, &CommandConsoleWidget::sigSendAirSupport, m_rmClient, &RMClient::sendAirSupport);
    connect(m_commandConsoleWidget, &CommandConsoleWidget::sigSendRemoteControl, m_rmClient, &RMClient::sendRemoteControl);

    // Note: m_rmClient signals for command results (e.g. sigGuardCtrlResult) are not currently handled by any widget in this refactor.
    // Original code had empty slots for these. If needed, we can connect them to LogPanel or similar.
    // For now, I will leave them unconnected as they were empty in original code (except for debug prints which are now gone, or rather, I didn't reimplement them in widgets).
    // Actually, looking at original MainWindow, updateGuardCtrlResult etc were empty.
    
    m_rmClient->connectToHost();
    m_videoDecoder->start();
    
    setMinimumSize(1024, 768);
    setWindowTitle("RoboMaster 2026 自定义客户端 (BUPT)");
}

MainWindow::~MainWindow() {
    if (m_videoDecoder) {
        m_videoDecoder->stop();
        m_videoDecoder->wait();
    }
}

void MainWindow::setupUi() {
    QWidget *central = new QWidget(this);
    setCentralWidget(central);
    QVBoxLayout *mainLayout = new QVBoxLayout(central);
    
    // Top: Connection Status Widget
    m_connectionStatusWidget = new ConnectionStatusWidget(this);
    mainLayout->addWidget(m_connectionStatusWidget);

    // Top: Tab Widget
    m_tabWidget = new QTabWidget(this);
    mainLayout->addWidget(m_tabWidget, 1); // Stretch factor 1

    // 1. Dashboard Tab
    m_dashboardWidget = new DashboardWidget(this);
    m_tabWidget->addTab(m_dashboardWidget, "全局面板 (Global)");

    // 2. Video Tab
    QWidget *videoTab = new QWidget();
    QVBoxLayout *videoLayout = new QVBoxLayout(videoTab);
    m_videoWidget = new VideoWidget(videoTab);
    m_videoWidget->setMinimumSize(640, 360);
    m_videoWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    videoLayout->addWidget(m_videoWidget);
    m_tabWidget->addTab(videoTab, "视频流 (Video Feed)");

    // 3. Robot Status Tab
    m_robotStatusWidget = new RobotStatusWidget(this);
    m_tabWidget->addTab(m_robotStatusWidget, "机器人状态 (My Robot)");

    // 4. Map Tab
    m_mapWidget = new MapWidget(this);
    m_tabWidget->addTab(m_mapWidget, "地图与感知 (Map)");

    // 5. Command Tab
    m_commandConsoleWidget = new CommandConsoleWidget(this);
    m_tabWidget->addTab(m_commandConsoleWidget, "控制台 (Command)");
}

void MainWindow::updateVideoFrame(const QImage &image) {
    if (m_videoWidget) {
        m_videoWidget->setImage(image);
    }
}

// --- Input Handling ---

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (event->isAutoRepeat()) return;
    updateKeyMask(event->key(), true);
}

void MainWindow::keyReleaseEvent(QKeyEvent *event) {
    if (event->isAutoRepeat()) return;
    updateKeyMask(event->key(), false);
}

void MainWindow::mouseMoveEvent(QMouseEvent *event) {
    m_mouseX = event->position().toPoint().x();
    m_mouseY = event->position().toPoint().y();
}

void MainWindow::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) m_mouseLeft = true;
    if (event->button() == Qt::RightButton) m_mouseRight = true;
    if (event->button() == Qt::MiddleButton) m_mouseMid = true;
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) m_mouseLeft = false;
    if (event->button() == Qt::RightButton) m_mouseRight = false;
    if (event->button() == Qt::MiddleButton) m_mouseMid = false;
}

void MainWindow::wheelEvent(QWheelEvent *event) {
    m_mouseZ += event->angleDelta().y();
}

void MainWindow::updateKeyMask(int key, bool pressed) {
    // TODO: Implement key mapping to RoboMaster protocol
}

void MainWindow::syncControlData() {
    // TODO: Send control data
    // Example:
    // m_rmClient->updateControlData(m_mouseX, m_mouseY, m_mouseZ, m_mouseLeft, m_mouseRight, m_keyMask, m_mouseMid, m_data);
    // m_rmClient->sendRemoteControl();
}
