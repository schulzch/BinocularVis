#ifndef DISPLAYSTS_H
#define DISPLAYSTS_H

#include "../function.h"
#include "../../timeline/timelinescene.h"
#include "../primitive/variant.h"
#include "../primitive/vector.h"
#include "video.h"

struct STSNames {
    QString timeName;
    QString topName;
    QString bottomName;
    QString combinedName;
};

/// Updates a scene to display data.
class DisplaySTS : public Function {
public:
    DisplaySTS(TimelineScene* scene, const QString &label, int height, QColor color, const STSNames& names);
    ~DisplaySTS() = default;

protected:
    const QList<const Value*> values() override;

    void call() override;

private:
    TimelineScene* _scene;
    QScopedPointer<TimelineItem> _item;
    STSNames _names;
};

#endif
