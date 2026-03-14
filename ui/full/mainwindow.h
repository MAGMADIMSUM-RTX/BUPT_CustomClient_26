#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>

#include "rm_client.h"
#include "video/video_decoder.h"
#include "video/video_widget.h"

#include "dashboard_widget.h"
#include "robot_status_widget.h"
#include "map_widget.h"
#include "command_console_widget.h"
#include "connection_status_widget.h"
#include "../../log_bridge.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    // Input Handling
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private slots:
    // --- Video Slots ---
    void updateVideoFrame(const QImage &image);

private:
    RMClient *m_rmClient;
    VideoDecoder *m_videoDecoder;

    // --- UI Components ---
    QTabWidget *m_tabWidget = nullptr;
    
    DashboardWidget *m_dashboardWidget = nullptr;
    VideoWidget *m_videoWidget = nullptr;
    RobotStatusWidget *m_robotStatusWidget = nullptr;
    MapWidget *m_mapWidget = nullptr;
    CommandConsoleWidget *m_commandConsoleWidget = nullptr;
    ConnectionStatusWidget *m_connectionStatusWidget = nullptr;
    LogBridge *m_logBridge = nullptr;

    // Input State
    uint32_t m_keyMask = 0;
    int m_mouseX = 0;
    int m_mouseY = 0;
    int m_mouseZ = 0;
    bool m_mouseLeft = false;
    bool m_mouseRight = false;
    bool m_mouseMid = false;
    QByteArray m_data;

    void setupUi();
    // void setupDebugUi();
    // void setupReleaseUi();
    // void bindSignals();

    void updateKeyMask(int key, bool pressed);
    void syncControlData();
};

#endif // MAINWINDOW_H
