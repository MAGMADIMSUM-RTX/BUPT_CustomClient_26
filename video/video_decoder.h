#ifndef VIDEO_DECODER_H
#define VIDEO_DECODER_H

#include <QThread>
#include <QImage>
#include <QUdpSocket>
#include <QMutex>
#include <atomic>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#include <libavformat/avformat.h>
}

class VideoDecoder : public QThread
{
    Q_OBJECT

public:
    explicit VideoDecoder(QObject *parent = nullptr);
    ~VideoDecoder() override;

    void stop();

signals:
    void frameReady(const QImage &image);
    void logMessage(const QString &msg);

protected:
    void run() override;

private:
    bool initDecoder();
    void cleanup();
    void processPacket(const QByteArray &data);

    std::atomic<bool> m_running;
    quint16 m_port;

    // FFmpeg components
    const AVCodec *m_codec = nullptr;
    AVCodecContext *m_codecCtx = nullptr;
    AVCodecParserContext *m_parser = nullptr;
    AVFrame *m_frame = nullptr;
    AVFrame *m_rgbFrame = nullptr;
    AVPacket *m_packet = nullptr;
    SwsContext *m_swsCtx = nullptr;

    uint8_t *m_rgbBuffer = nullptr;
    int m_lastWidth = 0;
    int m_lastHeight = 0;

    // Reassembly
    QByteArray m_reassemblyBuffer;
    quint16 m_currentFrameSeq = 0;
    quint32 m_expectedSize = 0;
    int m_packetCount = 0;
    qint64 m_lastFrameTime = 0;
};

#endif // VIDEO_DECODER_H
