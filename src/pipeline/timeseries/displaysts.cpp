#include "displaysts.h"
#include "timeline/stcitem.h"
#include "timeline/stsitem.h"

DisplaySTS::DisplaySTS(TimelineScene* scene, const QString& label, int height, QColor color, const STSNames& names)
    : _scene(scene)
    , _names(names)
{
    auto* sts = new STSItem();
    sts->setHeight(height);
    sts->setColor(color);
    _scene->appendItem(sts, 5);
    _item.reset(sts);
    _scene->appendLabel(sts, label);
}

const QList<const Value*> DisplaySTS::values()
{
    return QList<const Value*>();
}

void DisplaySTS::call()
{
    const VariantVector* time = input<VariantVector>(_names.timeName);
    const VariantVector* top = input<VariantVector>(_names.topName);
    const VariantVector* bottom = input<VariantVector>(_names.bottomName);
    const VariantVector* combined = input<VariantVector>(_names.combinedName);
    if (!time) {
        return;
    }
    STSItem* sts = dynamic_cast<STSItem*>(_item.data());

    sts->setPoints(*time, top, bottom, combined);
}
