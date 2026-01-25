#include "video_decoder.h"
#include <QDebug>
#include <QDateTime>

VideoDecoder::VideoDecoder(QObject *parent)
    : QThread(parent)
    , m_running(false)
    , m_port(3334)
{
}

VideoDecoder::~VideoDecoder()
{
    stop();
    wait();
    cleanup();
}

void VideoDecoder::stop()
{
    m_running = false;
}

bool VideoDecoder::initDecoder()
{
    // Find H.265 decoder
    m_codec = avcodec_find_decoder(AV_CODEC_ID_HEVC);
    if (!m_codec) {
        emit logMessage("Error: H.265 decoder not found");
        return false;
    }

    m_codecCtx = avcodec_alloc_context3(m_codec);
    if (!m_codecCtx) {
        emit logMessage("Error: Could not allocate video codec context");
        return false;
    }

    // Enable low delay
    m_codecCtx->flags |= AV_CODEC_FLAG_LOW_DELAY;
    
    // Thread count for software decoding
    m_codecCtx->thread_count = 2;
    m_codecCtx->thread_type = FF_THREAD_SLICE;

    if (avcodec_open2(m_codecCtx, m_codec, nullptr) < 0) {
        emit logMessage("Error: Could not open codec");
        return false;
    }

    // Initialize parser
    m_parser = av_parser_init(m_codec->id);
    if (!m_parser) {
        emit logMessage("Error: Could not initialize parser");
        return false;
    }

    m_packet = av_packet_alloc();
    m_frame = av_frame_alloc();
    m_rgbFrame = av_frame_alloc();

    if (!m_packet || !m_frame || !m_rgbFrame) {
        emit logMessage("Error: Could not allocate frames/packet");
        return false;
    }

    emit logMessage("Video decoder initialized successfully");
    return true;
}

void VideoDecoder::cleanup()
{
    if (m_codecCtx) {
        avcodec_free_context(&m_codecCtx);
        m_codecCtx = nullptr;
    }
    if (m_parser) {
        av_parser_close(m_parser);
        m_parser = nullptr;
    }
    if (m_frame) {
        av_frame_free(&m_frame);
        m_frame = nullptr;
    }
    if (m_rgbFrame) {
        av_frame_free(&m_rgbFrame);
        m_rgbFrame = nullptr;
    }
    if (m_packet) {
        av_packet_free(&m_packet);
        m_packet = nullptr;
    }
    if (m_swsCtx) {
        sws_freeContext(m_swsCtx);
        m_swsCtx = nullptr;
    }
    if (m_rgbBuffer) {
        av_free(m_rgbBuffer);
        m_rgbBuffer = nullptr;
    }
}

void VideoDecoder::run()
{
    if (!initDecoder()) {
        return;
    }

    QUdpSocket socket;
    if (!socket.bind(QHostAddress::AnyIPv4, m_port)) {
        emit logMessage(QString("Error: Could not bind to port %1").arg(m_port));
        return;
    }

    emit logMessage(QString("Listening for video on port %1").arg(m_port));

    m_running = true;
    m_reassemblyBuffer.clear();
    m_currentFrameSeq = 0;
    m_expectedSize = 0;
    m_packetCount = 0;
    m_lastFrameTime = 0;

    // Buffer to hold datagram content
    QByteArray datagram;

    while (m_running) {
        if (socket.waitForReadyRead(100)) {
            while (socket.hasPendingDatagrams()) {
                datagram.resize(socket.pendingDatagramSize());
                socket.readDatagram(datagram.data(), datagram.size());

                // 1. Strip Custom Header (8 bytes)
                if (datagram.size() < 8) continue;

                // Parse Header - BIG ENDIAN
                const uint8_t *header = reinterpret_cast<const uint8_t*>(datagram.constData());
                uint16_t frameSeq = (header[0] << 8) | header[1];
                // uint16_t fragSeq = (header[2] << 8) | header[3]; // Not strictly used for logic if assuming ordered UDP
                uint32_t totalSize = (header[4] << 24) | (header[5] << 16) | (header[6] << 8) | header[7];

                // 2. New Frame Detection
                if (frameSeq != m_currentFrameSeq) {
                    // Check if previous frame was incomplete
                    if (!m_reassemblyBuffer.isEmpty()) {
                        // Optional: Log packet loss
                        // emit logMessage(QString("Warning: Frame %1 incomplete, dropping.").arg(m_currentFrameSeq));
                    }

                    m_currentFrameSeq = frameSeq;
                    m_reassemblyBuffer.clear();
                    m_expectedSize = totalSize;
                    m_packetCount = 0;
                    
                    // Sanity check on size
                    if (m_expectedSize > 10 * 1024 * 1024) { // Limit to 10MB
                         m_expectedSize = 0;
                    } else if (m_expectedSize > 0) {
                        m_reassemblyBuffer.reserve(m_expectedSize);
                    }
                }

                // 3. Append Payload (Strip header)
                if (m_expectedSize > 0) {
                    m_reassemblyBuffer.append(datagram.mid(8));
                    m_packetCount++;

                    // 4. Check Completion
                    if (m_reassemblyBuffer.size() >= (int)m_expectedSize) {
                        qint64 now = QDateTime::currentMSecsSinceEpoch();
                        int interval = (m_lastFrameTime == 0) ? 0 : (now - m_lastFrameTime);
                        m_lastFrameTime = now;

                        emit logMessage(QString("Received Frame #%1, Size: %2 bytes, Packets: %3, Interval: %4 ms")
                                        .arg(m_currentFrameSeq)
                                        .arg(m_expectedSize)
                                        .arg(m_packetCount)
                                        .arg(interval));

                        processPacket(m_reassemblyBuffer);
                        m_reassemblyBuffer.clear();
                        m_expectedSize = 0;
                        m_packetCount = 0;
                    }
                }
            }
        }
    }
    
    emit logMessage("Video decoder thread stopped");
}

void VideoDecoder::processPacket(const QByteArray &data)
{
    if (!m_codecCtx || !m_parser) return;

    const uint8_t *cur_ptr = reinterpret_cast<const uint8_t*>(data.constData());
    int cur_size = data.size();

    while (cur_size > 0) {
        uint8_t *out_data = nullptr;
        int out_size = 0;

        int len = av_parser_parse2(m_parser, m_codecCtx,
                                   &out_data, &out_size,
                                   cur_ptr, cur_size,
                                   AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);

        cur_ptr += len;
        cur_size -= len;

        if (out_size == 0) {
            continue;
        }

        // We have a parsed packet. 
        // Note: out_data points to the parser's internal buffer.
        // We must copy it to AVPacket.
        if (av_new_packet(m_packet, out_size) < 0) {
            emit logMessage("Error: Could not allocate packet data");
            continue;
        }
        memcpy(m_packet->data, out_data, out_size);
        
        // Send to decoder
        int ret = avcodec_send_packet(m_codecCtx, m_packet);
        av_packet_unref(m_packet); // Unref immediately after sending

        if (ret < 0 && ret != AVERROR(EAGAIN)) {
            char errbuf[AV_ERROR_MAX_STRING_SIZE];
            av_strerror(ret, errbuf, sizeof(errbuf));
            emit logMessage(QString("Decoder Error (Send): %1").arg(errbuf));
            continue;
        }

        // Receive frames
        while (true) {
            int res = avcodec_receive_frame(m_codecCtx, m_frame);
            if (res == AVERROR(EAGAIN) || res == AVERROR_EOF) {
                break;
            } else if (res < 0) {
                char errbuf[AV_ERROR_MAX_STRING_SIZE];
                av_strerror(res, errbuf, sizeof(errbuf));
                emit logMessage(QString("Decoder Error (Receive): %1").arg(errbuf));
                break;
            }

            // Convert to RGB
            if (m_frame->width != m_lastWidth || m_frame->height != m_lastHeight) {
                // Clean up old scaler/buffers
                if (m_swsCtx) {
                    sws_freeContext(m_swsCtx);
                    m_swsCtx = nullptr;
                }
                if (m_rgbBuffer) {
                    av_free(m_rgbBuffer);
                    m_rgbBuffer = nullptr;
                }

                m_lastWidth = m_frame->width;
                m_lastHeight = m_frame->height;

                // User requested Format_RGB888 -> AV_PIX_FMT_RGB24
                m_swsCtx = sws_getContext(
                    m_lastWidth, m_lastHeight, m_codecCtx->pix_fmt,
                    m_lastWidth, m_lastHeight, AV_PIX_FMT_RGB24,
                    SWS_BILINEAR, nullptr, nullptr, nullptr
                );

                int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24, m_lastWidth, m_lastHeight, 1);
                m_rgbBuffer = (uint8_t *)av_malloc(numBytes * sizeof(uint8_t));

                av_image_fill_arrays(m_rgbFrame->data, m_rgbFrame->linesize, m_rgbBuffer,
                                     AV_PIX_FMT_RGB24, m_lastWidth, m_lastHeight, 1);
            }

            if (m_swsCtx) {
                sws_scale(m_swsCtx, (uint8_t const * const *)m_frame->data,
                          m_frame->linesize, 0, m_lastHeight,
                          m_rgbFrame->data, m_rgbFrame->linesize);

                QImage img(m_rgbFrame->data[0], m_lastWidth, m_lastHeight, m_rgbFrame->linesize[0], QImage::Format_RGB888);
                emit frameReady(img.copy());
            }
        }
    }
}
