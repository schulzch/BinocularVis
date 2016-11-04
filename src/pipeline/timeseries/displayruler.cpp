#include "displayruler.h"
#include "timeline/ruleritem.h"

DisplayRuler::DisplayRuler(TimelineScene* scene, const QString& label, const QString& timeName, const QString& timeScaledName)
    : _scene(scene)
    , _timeName(timeName)
    , _timeScaledName(timeScaledName)
{
    auto* ruler = new RulerItem();
    ruler->setHeight(15);
    ruler->setTickStep(50);
    _scene->appendItem(ruler, 0);
    _item.reset(ruler);
    _scene->appendLabel(ruler, label);
}

const QList<const Value*> DisplayRuler::values()
{
    return QList<const Value*>();
}

void DisplayRuler::call()
{
    const VariantVector* time = input<VariantVector>(_timeName);
    const VariantVector* timeScaled = input<VariantVector>(_timeScaledName);
    if (!time || !timeScaled) {
        return;
    }

    RulerItem* ruler = dynamic_cast<RulerItem*>(_item.data());
    ruler->setTime(*time, *timeScaled);
}
