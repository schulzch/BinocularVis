#ifndef SCARFITEM_H
#define SCARFITEM_H

#include "timelineitem.h"
#include <QPen>
#include <QHash>

/// Implement hashing for QColor, required by QHash
inline uint qHash(const QColor& key)
{
    return key.rgba();
}

/// This class provides a 2D scarf-plot that you can add to QGraphicsScene.
class ScarfItem : public TimelineItem {
public:
    ScarfItem(QGraphicsItem* parent = 0);

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);

    void setBars(const QVector<QVariant>& starts, const QVector<QVariant>& widths, const QVector<QVariant>& colors);

private:
    QHash<QColor, QVector<QRectF> > _rects;
    QList<QColor> _drawOrder;
    QPen _pen;
};

#endif
