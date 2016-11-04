#include "displayscarf.h"
#include "timeline/scarfitem.h"
#include "utils/colorbrewer.h"

DisplayScarf::DisplayScarf(TimelineScene* scene, const QString& label, int height, const ScarfNames& names, const QMap<int, QColor>& colors)
    : _scene(scene)
    , _names(names)
    , _colors(colors)
    , _height(height)
{
    auto* scarfTop = new ScarfItem();
    if (!names.bottomName.isEmpty()) {
        scarfTop->setHeight(_height / 2);
    }
    else {
        scarfTop->setHeight(_height);
    }
    _scene->addItem(scarfTop);
    _itemTop.reset(scarfTop);
    if (names.topName.isEmpty()) {
        scarfTop->setVisible(false);
    }

    auto* scarfBottom = new ScarfItem();
    if (!names.topName.isEmpty()) {
        scarfBottom->setHeight(_height / 2);
        scarfBottom->setPos(0, _height / 2);
    }
    else {
        scarfBottom->setHeight(_height);
    }
    _scene->addItem(scarfBottom);
    _itemBottom.reset(scarfBottom);
    if (names.bottomName.isEmpty()) {
        scarfBottom->setVisible(false);
    }

    QGraphicsItemGroup* group = new QGraphicsItemGroup();
    group->setHandlesChildEvents(false);
    group->addToGroup(scarfTop);
    group->addToGroup(scarfBottom);
    _scene->appendItem(group, 5);
    _group.reset(group);

    _scene->appendLabel(group, label);
}

const QList<const Value*> DisplayScarf::values()
{
    return QList<const Value*>();
}

void DisplayScarf::call()
{
    const VariantVector* time = input<VariantVector>(_names.timeName);
    const VariantVector* top = input<VariantVector>(_names.topName);
    const VariantVector* bottom = input<VariantVector>(_names.bottomName);
    if (!time || (!top && !bottom)) {
        return;
    }

    auto rle = [&](const QVector<QVariant>& list, QVector<QVariant>& starts,
        QVector<QVariant>& widths, QVector<QVariant>& colors) {
        double start = (*time)[0].toDouble();
        QVariant lastItem = list[0];
        for (int i = 0; i < list.length(); ++i) {
            QVariant item = list[i];
            if (item != lastItem) {
                double end = (*time)[i].toDouble();
                if (lastItem.isValid()) {
                    starts.append(start);
                    widths.append(end - start);
                    colors.append(_colors.value(lastItem.toInt(), Qt::black));
                }
                lastItem = item;
                start = end;
            }
        }
    };

    ScarfItem* scarfTop = dynamic_cast<ScarfItem*>(_itemTop.data());
    ScarfItem* scarfBottom = dynamic_cast<ScarfItem*>(_itemBottom.data());
    if (top) {
        QVector<QVariant> startsTop;
        QVector<QVariant> widthsTop;
        QVector<QVariant> colorsTop;
        rle(*top, startsTop, widthsTop, colorsTop);
        scarfTop->setBars(startsTop, widthsTop, colorsTop);
    }
    if (bottom) {
        QVector<QVariant> startsBottom;
        QVector<QVariant> widthsBottom;
        QVector<QVariant> colorsBottom;
        rle(*bottom, startsBottom, widthsBottom, colorsBottom);
        scarfBottom->setBars(startsBottom, widthsBottom, colorsBottom);
    }
}
