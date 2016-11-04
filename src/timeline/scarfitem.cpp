#include "scarfitem.h"
#include <QtWidgets>

ScarfItem::ScarfItem(QGraphicsItem* parent)
    : TimelineItem(parent)
{
    _pen.setCapStyle(Qt::SquareCap);
    _pen.setJoinStyle(Qt::MiterJoin);
    _pen.setWidthF(0.0);
    _pen.setCosmetic(false);
}

void ScarfItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    TimelineItem::paint(painter, option, widget);

    for (const auto& color : _drawOrder) {
        _pen.setColor(color);
        painter->setPen(_pen);
        painter->setBrush(color);
        painter->drawRects(_rects[color]);
    }
}

void ScarfItem::setBars(const QVector<QVariant>& starts, const QVector<QVariant>& widths, const QVector<QVariant>& colors)
{
    const auto itemRect = boundingRect();

    // Construct rect and color map.
    QHash<QColor, int> zIndex;
    double maxValue = 0;
    _rects.clear();
    for (int i = 0; i < starts.length(); ++i) {
        double start = starts[i].toDouble();
        double width = widths[i].toDouble();
        QColor color = colors[i].value<QColor>();
        QRectF rect(start, 0.5, width, itemRect.height() - 1.0);
        for (const auto& otherColor : _rects.keys()) {
            if (color == otherColor) {
                for (auto& otherRect : _rects[otherColor]) {
                    if (rect.intersects(otherRect)) {
                        color = color.darker();
                    }
                }
            }
        }
        if (!zIndex.contains(color)) {
            zIndex[color] = 0;
        }
        for (const auto& otherColor : _rects.keys()) {
            if (color != otherColor) {
                for (auto& otherRect : _rects[otherColor]) {
                    if (rect.intersects(otherRect)) {
                        // Split rect and adjust drawing order.
                        if (rect.width() <= otherRect.width()) {
                            qreal offset = otherRect.height() * 0.2;
                            rect.setTop(otherRect.top() + offset);
                            rect.setBottom(otherRect.bottom() - offset);
                            if (zIndex[color] <= zIndex[otherColor]) {
                                zIndex[color] = zIndex[otherColor] + 1;
                            }
                        }
                        else {
                            qreal offset = rect.height() * 0.2;
                            otherRect.setTop(rect.top() + offset);
                            otherRect.setBottom(rect.bottom() - offset);
                            if (zIndex[otherColor] <= zIndex[color]) {
                                zIndex[otherColor] = zIndex[color] + 1;
                            }
                        }
                    }
                }
            }
        }
        _rects[color].append(rect);
        maxValue = qMax(maxValue, start + width);
    }

    // Sort colored rectangles by Z-Index.
    _drawOrder = _rects.keys();
    std::sort(_drawOrder.begin(), _drawOrder.end(), [&](const QColor& a, const QColor& b) {
        return zIndex[a] < zIndex[b];
    });

    // Issue repaint.
    setWidth(maxValue);
    update();
}
