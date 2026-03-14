#include "connection_status_widget.h"
#include <QDateTime>

ConnectionStatusWidget::ConnectionStatusWidget(QWidget *parent) : QWidget(parent)
{
    setupUi();
}

void ConnectionStatusWidget::setupUi()
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(5, 5, 5, 5); // Small margins
    
    lblConnStatus = new QLabel("Status: Disconnected");
    lblConnStatus->setStyleSheet("background-color: gray; color: white; padding: 2px 5px; border-radius: 3px;");
    
    lblLastSendTime = new QLabel("Sent: --:--:--");
    lblLastRecvTime = new QLabel("Recv: --:--:--");
    
    lblFeedback = new QLabel("Ready");
    lblFeedback->setStyleSheet("color: blue;");
    
    layout->addWidget(lblConnStatus);
    layout->addWidget(lblLastSendTime);
    layout->addWidget(lblLastRecvTime);
    layout->addWidget(lblFeedback);
    layout->addStretch();
}

void ConnectionStatusWidget::onConnectionStateChanged(int state)
{
    if(state == 0) {
        lblConnStatus->setText("Status: Disconnected");
        lblConnStatus->setStyleSheet("background-color: gray; color: white; padding: 2px 5px; border-radius: 3px;");
    } else if(state == 1) {
        lblConnStatus->setText("Status: Connecting...");
        lblConnStatus->setStyleSheet("background-color: orange; color: white; padding: 2px 5px; border-radius: 3px;");
    } else if(state == 2) {
        lblConnStatus->setText("Status: Connected");
        lblConnStatus->setStyleSheet("background-color: green; color: white; padding: 2px 5px; border-radius: 3px;");
    }
}

void ConnectionStatusWidget::updateActivity(bool isSend)
{
    QString timeStr = QDateTime::currentDateTime().toString("HH:mm:ss.zzz");
    if (isSend) {
        lblLastSendTime->setText("Sent: " + timeStr);
    } else {
        lblLastRecvTime->setText("Recv: " + timeStr);
    }
}
