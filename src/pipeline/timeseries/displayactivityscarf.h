#ifndef DISPLAYACTIVITYSCARF_H
#define DISPLAYACTIVITYSCARF_H

#include "../function.h"
#include "../../timeline/timelinescene.h"
#include "../primitive/variant.h"
#include "../primitive/vector.h"
#include "video.h"

/// Updates a scene to display data.
class DisplayActivityScarf : public Function {
public:
    DisplayActivityScarf(TimelineScene* scene, const QString& label, int height, const QString& timeName, const QString& eventName);
    ~DisplayActivityScarf() = default;

protected:
    const QList<const Value*> values() override;

    void call() override;

private:
    QString _timeName;
    QString _eventName;
    TimelineScene* _scene;
    QScopedPointer<TimelineItem> _item;
};

#endif
