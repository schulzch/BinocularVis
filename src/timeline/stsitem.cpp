#include "stsitem.h"
#include <QtWidgets>

void saneMinMax(const QVector<QVariant>& vec, QVariant& aMin, QVariant& aMax)
{
    double min = std::numeric_limits<double>::max();
    double max = std::numeric_limits<double>::min();
    for (const auto& v : vec) {
        if (v.isValid()) {
            min = qMin(min, v.toDouble());
            max = qMax(max, v.toDouble());
        }
    }
    aMin.setValue(min);
    aMax.setValue(max);
}

STSItem::STSItem(QGraphicsItem* parent)
    : TimelineItem(parent)
    , _color(Qt::red)
    , _pen(_color)
{
    _pen.setCapStyle(Qt::FlatCap);
    _pen.setJoinStyle(Qt::MiterJoin);
    _pen.setWidthF(0.5);
}

void STSItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    TimelineItem::paint(painter, option, widget);

    QColor areaColor(_color.red(), _color.green(), _color.blue(), 96);
    painter->setPen(Qt::transparent);
    painter->setBrush(areaColor);
    for (const auto& polygon : _topPolygons) {
        painter->drawPolygon(polygon);
    }
    for (const auto& polygon : _bottomPolygons) {
        painter->drawPolygon(polygon);
    }
    painter->setPen(_pen);
    for (const auto& polygon : _combinedTopPolygons) {
        painter->drawPolyline(polygon);
    }
    for (const auto& polygon : _combinedBottomPolygons) {
        painter->drawPolyline(polygon);
    }
}

void STSItem::setPoints(
    const QVector<QVariant>& time,
    const QVector<QVariant>* top,
    const QVector<QVariant>* bottom,
    const QVector<QVariant>* combined)
{
    const auto itemRect = boundingRect();
    const auto plotHeight = [&]() {
        if (top && bottom) {
            return (itemRect.height() - 2) / 2.0;
        }
        else {
            return (itemRect.height() - 2);
        }
    }();
    const auto plotFn = [&](PolygonsVector& polygons, bool flipped, const QVector<QVariant>& values, QVariant valueMin, QVariant valueMax) {
        polygons.clear();
        bool nextPolygon = false;
        int polygonIndex = 0;
        QVector<QPointF> polygon;
        polygon.resize(time.length() * 2);
        double lastTime = time.first().toDouble();
        for (int i = 0; i < time.length(); ++i) {
            if (!values[i].isValid() || !time[i].isValid() || lastTime > time[i].toDouble()) {
                // Start skipping values.
                nextPolygon = true;
                continue;
            }
            lastTime = time[i].toDouble();
            if (polygonIndex > 0 && nextPolygon) {
                // Append last polygon and prepare a new one.
                polygon.resize(polygonIndex);
                polygons.append(polygon);
                polygon.resize(time.length() * 2);
                polygonIndex = 0;
                nextPolygon = false;
            }
            double y;
            double scaledValue = ((values[i].toDouble() - valueMin.toDouble()) / (valueMax.toDouble() - valueMin.toDouble())) * plotHeight;
            if (!flipped) {
                if (scaledValue > 0) {
                    y = plotHeight - scaledValue + 0.5;
                }
                else {
                    y = plotHeight + 0.5;
                }
            }
            else {
                if (scaledValue > 0) {
                    y = plotHeight + scaledValue + 1.5;
                }
                else {
                    y = plotHeight + 1.5;
                }
            }
            // Add interval points.
            polygon[polygonIndex].setX(time[i].toDouble());
            polygon[polygonIndex].setY(y);
            if (i + 1 < time.length() && time[i + 1].isValid() && time[i].toDouble() <= time[i + 1].toDouble()) {
                polygon[polygonIndex + 1].setX(time[i + 1].toDouble());
                polygon[polygonIndex + 1].setY(y);
            }
            else {
                polygon[polygonIndex + 1].setX(time[i].toDouble());
                polygon[polygonIndex + 1].setY(y);
            }
            polygonIndex += 2;
        }
        // Append last polygon.
        if (polygonIndex > 0) {
            polygon.resize(polygonIndex);
            polygons.append(polygon);
        }
    };
    const auto closeFn = [&](PolygonsVector& polygons, bool flipped) {
        float offset = 0;
        if (!flipped) {
            offset = 0.5;
        }
        else {
            offset = 1.5;
        }
        for (auto& polygon : polygons) {
            polygon.prepend(QPointF(polygon.first().x(), plotHeight + offset));
            polygon.append(QPointF(polygon.last().x(), plotHeight + offset));
        }
    };

    if (time.isEmpty()) {
        _topPolygons.clear();
        _combinedTopPolygons.clear();
        _bottomPolygons.clear();
        _combinedBottomPolygons.clear();
        setWidth(0);
        update();
        return;
    }

    // Generate polygons.
    if (top && top->length() == time.length()) {
        QVariant min;
        QVariant max;
        saneMinMax(*top, min, max);
        plotFn(_topPolygons, false, *top, min, max);
        closeFn(_topPolygons, false);
        if (combined && combined->length() == time.length()) {
            plotFn(_combinedTopPolygons, false, *combined, min, max);
        }
    }
    if (bottom && bottom->length() == time.length()) {
        QVariant min;
        QVariant max;
        saneMinMax(*bottom, min, max);
        plotFn(_bottomPolygons, true, *bottom, min, max);
        closeFn(_bottomPolygons, true);
        if (combined && combined->length() == time.length()) {
            plotFn(_combinedBottomPolygons, true, *combined, min, max);
        }
    }

    // Issue repaint.
    auto minmaxTime = std::minmax_element(time.begin(), time.end());
    setWidth((*minmaxTime.second).toDouble());
    update();
}

void STSItem::setColor(const QColor& color)
{
    _color = color.toRgb();
    _pen.setColor(_color);
}
