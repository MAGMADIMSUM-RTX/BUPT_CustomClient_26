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

#include "ui/dashboard_widget.h"
#include "ui/robot_status_widget.h"
#include "ui/map_widget.h"
#include "ui/command_console_widget.h"
#include "ui/connection_status_widget.h"
#include "log_bridge.h"

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
    QTabWidget *m_tabWidget;
    
    DashboardWidget *m_dashboardWidget;
    VideoWidget *m_videoWidget;
    RobotStatusWidget *m_robotStatusWidget;
    MapWidget *m_mapWidget;
    CommandConsoleWidget *m_commandConsoleWidget;
    ConnectionStatusWidget *m_connectionStatusWidget;
    LogBridge *m_logBridge;

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
    void updateKeyMask(int key, bool pressed);
    void syncControlData();
};

#endif // MAINWINDOW_H
