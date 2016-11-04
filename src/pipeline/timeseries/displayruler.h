#ifndef DISPLAYRULER_H
#define DISPLAYRULER_H

#include "../function.h"
#include "../../timeline/timelinescene.h"
#include "../primitive/variant.h"
#include "../primitive/vector.h"
#include "video.h"

/// Updates a scene to display data.
class DisplayRuler : public Function {
public:
    DisplayRuler(TimelineScene* scene, const QString &label, const QString& timeName, const QString &timeScaledName);
    ~DisplayRuler() = default;

protected:
    const QList<const Value*> values() override;

    void call() override;

private:
    TimelineScene* _scene;
    QScopedPointer<TimelineItem> _item;
    QString _timeName;
    QString _timeScaledName;
};

#endif
