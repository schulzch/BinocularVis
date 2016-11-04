#ifndef ANNOTATEDVIDEO_H
#define ANNOTATEDVIDEO_H

#include "video.h"
#include <QFileInfo>
#include <QVector>
#include <QRectF>
#include <QHash>

/// Represents an optionally annotated stimulus.
class AnnotatedVideo : public Video {
public:
    /// Represents a bounding box with its properties like type, framenumber and geometry.
    struct BBox {
        /// Type of a bounding box
        ///
        /// The type specifies whether the box really exists, got interpolated or is
        /// null.
        enum Type {
            /// Invalid bounding box (not existing).
            NULLTYPE = 0,
            /// Normal bounding box only present for one frame.
            SINGLE = 1,
            /// Like a keyframe; Not existing bounding boxes before this box will be interpolated.
            KEYBOX = 2,
            /// Interpolated bounding box.
            VIRTUAL = 3
        };
        /// Type of the bounding box
        Type type;
        /// Number of the frame the bounding box appears
        int frameIndex;
        /// Geometry of the bounding box
        QRect rect;
        /// Default constructor.
        BBox()
            : type(NULLTYPE)
        {
        }
    };

    /// Represents an object composed of bounding boxes.
    struct Object {
        /// ID of the object.
        int id;
        /// Category name.
        QString categoryName;
        /// Hashmap of bounding boxes (key: frame number).
        QHash<int, BBox> bboxes;
    };

    AnnotatedVideo(const QFileInfo& fileInfo);

    const QFileInfo& filePathAnnotation() const;
    const QVector<Object>& objects() const;

protected:
    QFileInfo _fileInfoAnnotation;
    QVector<Object> _objects;
};

#endif
