#ifndef TIMELINESCENE_H
#define TIMELINESCENE_H

#include <QGraphicsScene>
#include "timelineitem.h"

class TimelineScene : public QGraphicsScene {
    Q_OBJECT
public:
    TimelineScene(QObject* parent = 0);

    void appendItem(QGraphicsItem* item, qreal padding);
    void appendLabel(QGraphicsItem* item, const QString& text);
};

#endif
