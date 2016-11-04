#ifndef DISPLAYSCARF_H
#define DISPLAYSCARF_H

#include "../function.h"
#include "../../timeline/timelinescene.h"
#include "../primitive/variant.h"
#include "../primitive/vector.h"
#include "video.h"

struct ScarfNames {
    QString timeName;
    QString topName;
    QString bottomName;
};

/// Updates a scene to display data.
class DisplayScarf : public Function {
public:
    DisplayScarf(TimelineScene* scene, const QString& label, int height, const ScarfNames& names, const QMap<int, QColor>& colors);
    ~DisplayScarf() = default;

protected:
    const QList<const Value*> values() override;

    void call() override;

private:
    TimelineScene* _scene;
    QScopedPointer<QGraphicsItemGroup> _group;
    QScopedPointer<TimelineItem> _itemTop;
    QScopedPointer<TimelineItem> _itemBottom;
    ScarfNames _names;
    QMap<int, QColor> _colors;
    int _height;
};

#endif
