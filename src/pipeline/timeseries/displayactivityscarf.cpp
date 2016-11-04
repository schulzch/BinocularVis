#include "displayactivityscarf.h"
#include "timeline/scarfitem.h"
#include "utils/colorbrewer.h"

DisplayActivityScarf::DisplayActivityScarf(TimelineScene* scene, const QString& label, int height, const QString& timeName, const QString& eventName)
    : _scene(scene)
    , _timeName(timeName)
    , _eventName(eventName)
{
    auto* scarf = new ScarfItem();
    scarf->setHeight(height);
    _scene->appendItem(scarf, 5);
    _item.reset(scarf);
    _scene->appendLabel(scarf, label);
}

const QList<const Value*> DisplayActivityScarf::values()
{
    return QList<const Value*>();
}

void DisplayActivityScarf::call()
{
    const VariantVector* times = input<VariantVector>(_timeName);
    const VariantVector* events = input<VariantVector>(_eventName);
    double stimulusStart = 0;
    double stimulusEnd;
    int stimulusIndex = 0;

    if (!times || !events) {
        return;
    }

    QVector<QVariant> starts;
    QVector<QVariant> widths;
    QVector<QVariant> colors;

    QList<QColor> colorsDark2(brew<QColor>("Dark2", 8));
    QMap<int, QColor> colorMap;
    auto color = [&](int hash) {
        if (!colorMap.contains(hash)) {
            colorMap[hash] = colorsDark2.takeFirst();
            if (colorsDark2.isEmpty()) {
                colorsDark2 = brew<QColor>("Dark2", 8);
            }
        }
        return colorMap[hash];
    };

    for (int i = 0; i < times->length(); ++i) {
        QVariant event = (*events)[i];
        QString eventString = event.toString();
        double time = (*times)[i].toDouble();
        if (eventString == "ImageStart") {
            stimulusStart = time;
        }
        else if (eventString == "ImageEnd") {
            stimulusEnd = time;
            starts.append(stimulusStart);
            widths.append(stimulusEnd - stimulusStart);
            colors.append(color(stimulusIndex++));
        }
        else if (event.isValid()) {
            starts.append(stimulusStart);
            widths.append(1);
            colors.append(color(qHash(eventString)));
        }
    }

    ScarfItem* scarf = dynamic_cast<ScarfItem*>(_item.data());
    scarf->setBars(starts, widths, colors);
}
