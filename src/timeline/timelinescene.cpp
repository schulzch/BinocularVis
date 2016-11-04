#include "timelinescene.h"
#include "labelitem.h"
#include <QtWidgets>

TimelineScene::TimelineScene(QObject* parent)
    : QGraphicsScene(parent)
{
}

void TimelineScene::appendItem(QGraphicsItem* item, qreal padding)
{
    const QRectF itemsRect = itemsBoundingRect();
    item->setPos(0, itemsRect.bottom() + padding);
    addItem(item);
}

void TimelineScene::appendLabel(QGraphicsItem* item, const QString& text)
{
    LabelItem* label = new LabelItem(text);
    label->setHeight(item->boundingRect().height());
    label->setPos(-label->boundingRect().width(), item->y());
    addItem(label);
}
