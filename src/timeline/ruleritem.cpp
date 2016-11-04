#include "ruleritem.h"
#include <QtWidgets>

double lerp(float a, float b, float t)
{
    return a + t * (b - a);
}

RulerItem::RulerItem(QGraphicsItem* parent)
    : TimelineItem(parent)
    , _tickStep(10)
{
    setFlag(QGraphicsItem::ItemUsesExtendedStyleOption, true);
    _pen.setWidthF(0.5);
}

void RulerItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    const qreal TextPadding = 3;
    const QRect exposedRect = option->exposedRect.toRect();
    const qreal lod = option->levelOfDetailFromTransform(painter->worldTransform());

    // Lazy update labels.
    if (_tickLabels.isEmpty()) {
        updateTickLabels();
    }

    // Compute first and last tick adjacent to exposed rectangle.
    int xStart = exposedRect.left() - exposedRect.left() % _tickStep;
    int xEnd = exposedRect.right() + _tickStep;

    painter->setPen(_pen);
    _pen.setColor(palette().color(QPalette::Active, QPalette::Text));

    // Draw lines and labels.
    QVector<QLineF> lines;
    for (int x = xStart; x < xEnd; x += _tickStep) {
        if (lod > 0.5) {
            QString label = _tickLabels.value(x / _tickStep, "?");
            painter->drawText(x + TextPadding, 0, exposedRect.width(), exposedRect.height(),
                Qt::AlignLeft | Qt::AlignVCenter,
                label);
        }
        lines.append(QLineF(x, exposedRect.top() + exposedRect.height() / 2, x, exposedRect.bottom()));
    }
    lines.append(QLineF(exposedRect.left(), exposedRect.bottom(), exposedRect.right(), exposedRect.bottom()));
    painter->drawLines(lines);
}

int RulerItem::tickStep() const
{
    return _tickStep;
}

void RulerItem::setTickStep(int tickStep)
{
    _tickStep = tickStep;
    _tickLabels.clear();
    update();
}

void RulerItem::updateTickLabels()
{
    int ii = 0;
    int width = boundingRect().toRect().width();
    for (int x = 0; x < width + _tickStep; x += _tickStep) {
        double lastTimeScaled = 0;
        double lastTime = 0;
        for (int i = ii; i < _timeScaled.length(); ++i) {
            double time = _time[i].toDouble();
            if (_timeScaled[i] >= x) {
                double timeScaled = _timeScaled[i].toDouble();
                double t = (x - lastTimeScaled) / (timeScaled - lastTimeScaled);
                if (isnan(t)) {
                    t = 0;
                }
                double labelNumber = lerp(lastTime, time, t);
                _tickLabels.append(QString::number(labelNumber, 'f', 3));
                ii = i;
                break;
            }
            lastTimeScaled = _timeScaled[i].toDouble();
            lastTime = time;
        }
    }
}

void RulerItem::setTime(const QVector<QVariant>& time, const QVector<QVariant>& timeScaled)
{
    _time = time;
    _timeScaled = timeScaled;
    auto minmax = std::minmax_element(timeScaled.begin(), timeScaled.end());
    double width = (*minmax.second).toDouble() - (*minmax.first).toDouble();
    setWidth(width);
}
