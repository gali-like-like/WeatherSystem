#include "chartview.h"

ChartView::ChartView(QWidget* widget):QChartView(widget)
{
    this->setMouseTracking(true);
    this->setDragMode(QGraphicsView::RubberBandDrag);
    this->setRubberBand(QChartView::ClickThroughRubberBand);
    QChartView::RubberBands flags = this->rubberBand();
    flags |= QChartView::RectangleRubberBand;
    this->setRubberBand(flags);
}

void ChartView::mousePressEvent(QMouseEvent* e)
{
    if(e->button() == Qt::LeftButton)
    {
        begin = e->pos();
    }
    QChartView::mousePressEvent(e);
}

void ChartView::mouseMoveEvent(QMouseEvent* e)
{
    QPoint point = e->pos();
    emit mouseMovePoint(point);
    QChartView::mouseMoveEvent(e);
}

void ChartView::mouseReleaseEvent(QMouseEvent* e)
{
    if(e->button() == Qt::LeftButton)
    {
        end = e->pos();
        if((this->dragMode() == QGraphicsView::ScrollHandDrag) && (this->rubberBand() == QChartView::NoRubberBand))
        {
            chart()->scroll((begin-end).x(),(begin-end).y());
        }
        else if (this->dragMode() == QGraphicsView::RubberBandDrag)
        {
            QRect rect;
            rect.setTopLeft(begin);
            rect.setBottomLeft(end);
            this->chart()->zoomIn(rect);
        }
    }
    QChartView::mouseReleaseEvent(e);
}
