#include "loadvideo.h"

LoadVideo::LoadVideo(const QFileInfo& fileInfo)
    : _video(fileInfo)
{
    _values.append(&_video);
}

const QList<const Value*> LoadVideo::values()
{
    return _values;
}

void LoadVideo::call()
{
    // Do nothing, as all the work has been done by the constructor.
}
