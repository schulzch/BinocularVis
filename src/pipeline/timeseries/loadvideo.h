#ifndef LOADVIDEO_H
#define LOADVIDEO_H

#include "annotatedvideo.h"
#include "../function.h"

/// Loads an (annotated) video.
class LoadVideo : public Function {
public:
    LoadVideo(const QFileInfo& fileInfo);
    ~LoadVideo() = default;

protected:
    const QList<const Value*> values() override;

    void call() override;

private:
    AnnotatedVideo _video;
    QList<const Value*> _values;
};

#endif
