#ifndef MAP_WIDGET_H
#define MAP_WIDGET_H

#include <QWidget>
#include <QTreeWidget>
#include <QLabel>
#include <QTextEdit>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>

class MapWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MapWidget(QWidget *parent = nullptr);

public slots:
    void updateRadarInfo(int targetId, float x, float y, float angle, bool highlight);
    void updateRobotPathPlan(int intention, int sx, int sy, const QList<int> &offX, const QList<int> &offY, int sender);

private:
    void setupUi();

    QTreeWidget *treeRadar;
    QLabel *lblPathPlan;
    QTextEdit *txtPathDetail;
};

#endif // MAP_WIDGET_H
