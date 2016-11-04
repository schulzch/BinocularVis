#include "labelitem.h"
#include <QtWidgets>

LabelItem::LabelItem(const QString& label, QGraphicsItem* parent)
    : TimelineItem(parent)
    , _label(label)
{
    setWidth(70);
}

void LabelItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    const int TextPadding = 2;
    QRectF rect = boundingRect().adjusted(TextPadding, 0, -TextPadding, 0);
    painter->setPen(palette().color(QPalette::Active, QPalette::Text));
    painter->drawText(rect, Qt::AlignRight | Qt::AlignVCenter, _label);
}

const QString& LabelItem::label() const
{
    return _label;
}
