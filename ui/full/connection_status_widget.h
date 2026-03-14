#ifndef CONNECTION_STATUS_WIDGET_H
#define CONNECTION_STATUS_WIDGET_H

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>

class ConnectionStatusWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ConnectionStatusWidget(QWidget *parent = nullptr);

public slots:
    void onConnectionStateChanged(int state);
    void updateActivity(bool isSend);

private:
    void setupUi();

    QLabel *lblConnStatus;
    QLabel *lblLastSendTime;
    QLabel *lblLastRecvTime;
    QLabel *lblFeedback;
};

#endif // CONNECTION_STATUS_WIDGET_H
