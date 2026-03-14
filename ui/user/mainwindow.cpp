#include "mainwindow.h"
#include "rm_client.h"
#include "video/video_decoder.h"
#include "log_bridge.h"
#include "web_bridge.h"
#include <QPainter>
#include <QWebEngineView>
#include <QWebChannel>
#include <QFile>
#include <QDir>
#include <QUrl>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("RoboMaster 2026 自定义客户端 (User)");
    setMinimumSize(800, 600);
    // Ensure background is black
    QPalette pal = palette();
    pal.setColor(QPalette::Window, Qt::black);
    setPalette(pal);
    setAutoFillBackground(true);

    // Initialize backend components
    m_rmClient = new RMClient(this);
    m_videoDecoder = new VideoDecoder(this);
    m_logBridge = new LogBridge(this);
    m_webBridge = new WebBridge(this);

    // Connect Video Decoder
    connect(m_videoDecoder, &VideoDecoder::frameReady, this, &MainWindow::updateVideoFrame);

    // Connect Logs
    connect(m_rmClient, &RMClient::sigPacketLog, m_logBridge, &LogBridge::onPacketLog);
    connect(m_rmClient, &RMClient::sigConnectionStateChanged, m_logBridge, &LogBridge::onConnectionStateChanged);
    connect(m_videoDecoder, &VideoDecoder::logMessage, m_logBridge, &LogBridge::onVideoLog);

    // Connect Robot Status to WebBridge
    connect(m_rmClient, &RMClient::sigRobotDynamic, this, [this](u32 hp, f32 heat, f32, u32, u32, u32, u32, u32, u32 ammo, bool, u32, bool, bool) {
         m_webBridge->updateRobotStatus(hp, m_webBridge->maxHp(), ammo, heat, m_webBridge->robotId());
    });

    connect(m_rmClient, &RMClient::sigRobotStatic, this, [this](u32, u32, u32, u32 id, u32, u32, u32, u32, u32 maxHp, u32, u32, u32, u32, f32) {
          m_webBridge->updateRobotStatus(m_webBridge->robotHp(), maxHp, m_webBridge->robotAmmo(), m_webBridge->robotHeat(), id);
    });

    connect(m_rmClient, &RMClient::sigRobotPosition, this, [this](f32 x, f32 y, f32, f32 yaw) {
        m_webBridge->updateRobotPosition(x, y, yaw);
    });

    connect(m_rmClient, &RMClient::sigGlobalUnitStatus, this, [this](u32 baseHp, u32, u32, u32 outpostHp, u32, const QList<u32> &robotHp, const QList<i32> &robotBullets, u32, u32) {
        m_webBridge->updateGlobalUnitStatus(baseHp, outpostHp, robotHp, robotBullets);
    });

    connect(m_rmClient, &RMClient::sigGameStatus, this, [this](u32 round, u32 totalRounds, u32 redScore, u32 blueScore, u32, i32 time, i32, bool) {
        m_webBridge->updateGameStatus(round, totalRounds, redScore, blueScore, time);
    });

    // Setup WebEngine Overlay
    m_webView = new QWebEngineView(this);
    // Make WebEngine transparent
    m_webView->page()->setBackgroundColor(Qt::transparent);
    m_webView->setStyleSheet("background: transparent;");
    m_webView->setAttribute(Qt::WA_TranslucentBackground);
    
    // Setup WebChannel
    m_webChannel = new QWebChannel(m_webView->page());
    m_webChannel->registerObject("webBridge", m_webBridge);
    m_webView->page()->setWebChannel(m_webChannel);

    // Load HTML
    m_webView->load(QUrl("qrc:/assets/overlay.html"));
    m_webView->show();
    m_webView->raise(); // Ensure it's on top

    // Start components
    m_rmClient->connectToHost();
    m_videoDecoder->start();
}

MainWindow::~MainWindow()
{
    if (m_videoDecoder) {
        m_videoDecoder->stop();
        m_videoDecoder->wait();
    }
}

void MainWindow::updateVideoFrame(const QImage &image)
{
    QMutexLocker locker(&m_frameMutex);
    m_currentFrame = image;
    update();
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.fillRect(rect(), Qt::black);

    QMutexLocker locker(&m_frameMutex);
    if (!m_currentFrame.isNull()) {
        // Draw image scaled to fit the window, keeping aspect ratio
        QImage scaled = m_currentFrame.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        int x = (width() - scaled.width()) / 2;
        int y = (height() - scaled.height()) / 2;
        painter.drawImage(x, y, scaled);
    }
    
    // Note: HUD is handled by m_webView
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    if (m_webView) {
        m_webView->resize(this->size());
    }
}
