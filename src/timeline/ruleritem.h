#ifndef RULERITEM_H
#define RULERITEM_H

#include "timelineitem.h"
#include <QPen>

class RulerItem : public TimelineItem {
public:
    RulerItem(QGraphicsItem* parent = 0);

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);

    int tickStep() const;
    void setTickStep(int tickStep);

    void setTime(const QVector<QVariant>& time, const QVector<QVariant>& timeScaled);

private:
    void updateTickLabels();

    QVector<QVariant> _time;
    QVector<QVariant> _timeScaled;
    QVector<QString> _tickLabels;
    int _tickStep;
    QPen _pen;
};

#endif
