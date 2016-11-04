#ifndef TIMELINEITEM_H
#define TIMELINEITEM_H

#include <QGraphicsItem>
#include <QPalette>

class TimelineItem : public QGraphicsItem {
public:
    TimelineItem(QGraphicsItem* parent = 0);

    QRectF boundingRect() const;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);

    qreal height() const;
    void setHeight(qreal height);

protected:
    QPalette palette() const;

    void setWidth(qreal width);

private:
    QColor _bgColor;
    QString _label;
    QSizeF _size;
};

#endif
