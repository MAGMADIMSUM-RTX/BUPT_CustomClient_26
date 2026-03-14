#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QMutex>

class RMClient;
class VideoDecoder;
class LogBridge;
class QWebEngineView;
class QWebChannel;
class WebBridge;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void updateVideoFrame(const QImage &image);

private:
    RMClient *m_rmClient;
    VideoDecoder *m_videoDecoder;
    LogBridge *m_logBridge;

    QWebEngineView *m_webView;
    QWebChannel *m_webChannel;
    WebBridge *m_webBridge;

    QImage m_currentFrame;
    QMutex m_frameMutex;
};

#endif // MAINWINDOW_H
