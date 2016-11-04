#include "timelineitem.h"
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPainter>

TimelineItem::TimelineItem(QGraphicsItem* parent)
    : QGraphicsItem(parent)
    , _bgColor("#eeeeee")
{
}

QRectF TimelineItem::boundingRect() const
{
    return QRectF(QPointF(0, 0), _size);
}

void TimelineItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    painter->setPen(Qt::transparent);
    painter->setBrush(_bgColor);
    painter->drawRect(boundingRect());
}

qreal TimelineItem::height() const
{
    return _size.height();
}

void TimelineItem::setHeight(qreal height)
{
    _size.setHeight(height);
    prepareGeometryChange();
}

QPalette TimelineItem::palette() const
{
    return scene()->views().first()->palette();
}

void TimelineItem::setWidth(qreal width)
{
    _size.setWidth(width);
    prepareGeometryChange();
}
