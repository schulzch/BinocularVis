#ifndef STSITEM_H
#define STSITEM_H

#include "timelineitem.h"
#include <QPen>

/// This class provides a 2D stereo-plot (space time surface) that you can add to QGraphicsScene.
class STSItem : public TimelineItem {
public:
    STSItem(QGraphicsItem* parent = 0);

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);

    void setPoints(
        const QVector<QVariant>& time,
        const QVector<QVariant>* top,
        const QVector<QVariant>* bottom,
        const QVector<QVariant>* combined);

    void setColor(const QColor& color);

private:
    typedef QVector<QVector<QPointF>> PolygonsVector;
    PolygonsVector _topPolygons;
    PolygonsVector _bottomPolygons;
    PolygonsVector _combinedTopPolygons;
    PolygonsVector _combinedBottomPolygons;
    QColor _color;
    QPen _pen;
};

#endif
