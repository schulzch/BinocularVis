#include "displaystc.h"
#include "timeline/stcitem.h"
#include "timeline/stsitem.h"

DisplaySTC::DisplaySTC(TimelineScene* scene, const QString& label, int height, const STCNames& names)
    : _scene(scene)
    , _names(names)
{
    auto* stc = new STCItem();
    stc->setHeight(height);
    _scene->appendItem(stc, 5);
    _item.reset(stc);
    _scene->appendLabel(stc, label);
}

const QList<const Value*> DisplaySTC::values()
{
    return QList<const Value*>();
}

void DisplaySTC::call()
{
    const VariantVector* time = input<VariantVector>(_names.timeName);
    const VariantVector* positionX = input<VariantVector>(_names.positionXName);
    const VariantVector* positionY = input<VariantVector>(_names.positionYName);
    const VariantVector* sizeX = input<VariantVector>(_names.sizeXName);
    const VariantVector* sizeY = input<VariantVector>(_names.sizeYName);
    const Video* video = input<Video>(_names.videoName);
    if (!positionX || !positionY || !sizeX || !sizeY) {
        return;
    }

    STCItem* stc = dynamic_cast<STCItem*>(_item.data());
    stc->setPoints(*time, *positionX, *positionY, *sizeX, *sizeY);
    if (video) {
        float frameTime = 50.0f; //XXX: delegate to timelinescene
        QImage frameImage = video->frameImage(frameTime * video->fps());
        stc->setImage(frameImage, frameTime);
    }
}
