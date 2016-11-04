#ifndef DISPLAYSTC_H
#define DISPLAYSTC_H

#include "../function.h"
#include "../../timeline/timelinescene.h"
#include "../primitive/variant.h"
#include "../primitive/vector.h"
#include "video.h"

struct STCNames {
    QString timeName;
    QString positionXName;
    QString positionYName;
    QString sizeXName;
    QString sizeYName;
    QString videoName;
};

/// Updates a scene to display data.
class DisplaySTC : public Function {
public:
    DisplaySTC(TimelineScene* scene, const QString &label, int height, const STCNames& names);
    ~DisplaySTC() = default;

protected:
    const QList<const Value*> values() override;

    void call() override;

private:
    TimelineScene* _scene;
    QScopedPointer<TimelineItem> _item;
    STCNames _names;
};

#endif
