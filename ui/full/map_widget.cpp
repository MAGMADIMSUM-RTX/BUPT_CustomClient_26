#include "map_widget.h"

MapWidget::MapWidget(QWidget *parent) : QWidget(parent)
{
    setupUi();
}

void MapWidget::setupUi()
{
    QHBoxLayout *layout = new QHBoxLayout(this);

    // Radar Info
    QGroupBox *grpRadar = new QGroupBox("雷达与定位");
    QVBoxLayout *radLayout = new QVBoxLayout(grpRadar);
    treeRadar = new QTreeWidget();
    treeRadar->setHeaderLabels({"目标ID", "位置(x,y)", "角度", "高亮"});
    radLayout->addWidget(treeRadar);
    layout->addWidget(grpRadar);

    // Path Plan
    QGroupBox *grpPath = new QGroupBox("路径规划");
    QVBoxLayout *pathLayout = new QVBoxLayout(grpPath);
    lblPathPlan = new QLabel("暂无路径数据");
    txtPathDetail = new QTextEdit();
    txtPathDetail->setReadOnly(true);
    txtPathDetail->setMaximumHeight(100);
    pathLayout->addWidget(lblPathPlan);
    pathLayout->addWidget(txtPathDetail);
    layout->addWidget(grpPath);
}

void MapWidget::updateRadarInfo(int targetId, float x, float y, float angle, bool highlight)
{
    if(treeRadar) {
        // Simple implementation: Add new item. In real app, might update existing ID.
        // Assuming simplistic approach as per original code style request.
        new QTreeWidgetItem(treeRadar, QStringList() << QString::number(targetId) << QString("(%1, %2)").arg(x).arg(y) << QString::number(angle) << (highlight ? "是" : "否"));
    }
}

void MapWidget::updateRobotPathPlan(int intention, int sx, int sy, const QList<int> &offX, const QList<int> &offY, int sender)
{
    if(lblPathPlan) {
        QString intentionStr;
        switch(intention) {
            case 1: intentionStr = "攻击"; break;
            case 2: intentionStr = "防守"; break;
            case 3: intentionStr = "移动"; break;
            default: intentionStr = QString::number(intention); break;
        }
        lblPathPlan->setText(QString("意图: %1, 发送者: %2, 起点: (%3, %4)").arg(intentionStr).arg(sender).arg(sx).arg(sy));
    }
    if(txtPathDetail) {
        QStringList points;
        for(int i=0; i<offX.size() && i<offY.size(); ++i) {
            points << QString("(%1,%2)").arg(offX[i]).arg(offY[i]);
        }
        txtPathDetail->setText("偏移量: " + points.join(", "));
    }
}
