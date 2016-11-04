#ifndef LABELITEM_H
#define LABELITEM_H

#include "timelineitem.h"
#include <QPen>

class LabelItem : public TimelineItem {
public:
    LabelItem(const QString &label, QGraphicsItem* parent = 0);

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);


    const QString& label() const;

private:
    QString _label;
};

#endif
