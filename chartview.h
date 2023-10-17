#ifndef CHARTVIEW_H
#define CHARTVIEW_H
#include <QChartView>
#include <QObject>

class ChartView : public QChartView
{
    Q_OBJECT
public:
    ChartView(QWidget* parent = nullptr);
protected:
    void mouseReleaseEvent(QMouseEvent* e);
    void mousePressEvent(QMouseEvent* e);
    void mouseMoveEvent(QMouseEvent* e);
private:
    QPoint begin;
    QPoint end;
signals:
    void mouseMovePoint(const QPoint& point);
};

#endif // CHARTVIEW_H
