#include "video_widget.h"
#include <QPainter>

VideoWidget::VideoWidget(QWidget *parent)
    : QWidget(parent)
{
    // Set a black background default
    setAttribute(Qt::WA_OpaquePaintEvent);
}

void VideoWidget::setImage(const QImage &image)
{
    QMutexLocker locker(&m_mutex);
    m_image = image;
    update(); // Trigger repaint
}

void VideoWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.fillRect(rect(), Qt::black);

    QMutexLocker locker(&m_mutex);
    if (!m_image.isNull()) {
        // Draw image scaled to fit the widget, keeping aspect ratio
        QImage scaled = m_image.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        int x = (width() - scaled.width()) / 2;
        int y = (height() - scaled.height()) / 2;
        painter.drawImage(x, y, scaled);
    }
}
