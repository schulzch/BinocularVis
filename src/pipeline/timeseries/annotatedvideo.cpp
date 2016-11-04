#include "annotatedvideo.h"
#include <QtCore>

bool readBTD(QVector<AnnotatedVideo::Object>& objects, QString& videoFilenName, const QString& fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Can not open file" << fileName;
        return false;
    }

    QDataStream stream(&file);

    // Read BTD magic.
    quint8 magic[3];
    stream.readRawData(reinterpret_cast<char*>(magic), sizeof(magic));
    if (magic[0] != (quint8)'B' || magic[1] != (quint8)'T'
        || magic[2] != (quint8)'D') {
        qWarning() << "Invalid BTD file!";
        return false;
    }

    // Read BTD version.
    quint8 version;
    stream >> version;
    if (version != 1) {
        qWarning() << "Invalid BTD version!";
        return false;
    }

    // Read header.
    quint32 nextId;
    stream >> videoFilenName;
    stream >> nextId;

    // Read categories.
    quint32 categoryCount;
    stream >> categoryCount;
    for (quint32 i = 0; i < categoryCount; ++i) {
        QString categoryName;
        stream >> categoryName;
        // Read object.
        quint32 objectCount;
        stream >> objectCount;
        for (quint32 j = 0; j < objectCount; ++j) {
            AnnotatedVideo::Object object;
            object.categoryName = categoryName;
            stream >> object.id;
            // Read bounding boxes.
            quint32 bboxCount;
            stream >> bboxCount;
            for (quint32 k = 0; k < bboxCount; ++k) {
                AnnotatedVideo::BBox bbox;
                stream >> reinterpret_cast<quint8&>(bbox.type);
                stream >> reinterpret_cast<quint32&>(bbox.frameIndex);
                stream >> bbox.rect;
                object.bboxes.insert(bbox.frameIndex, bbox);
            }
            objects.append(object);
        }
    }

    return true;
}

AnnotatedVideo::AnnotatedVideo(const QFileInfo& fileInfo)
    : Video(QFileInfo())
    , _fileInfoAnnotation(fileInfo)
{
    QString videoFileName;
    if (readBTD(_objects, videoFileName, fileInfo.absoluteFilePath())) {
        QString basePath = fileInfo.path();
        open(QFileInfo(basePath % "/" % videoFileName));
    }
    else {
        open(fileInfo.absoluteFilePath());
    }
}

const QFileInfo& AnnotatedVideo::filePathAnnotation() const
{
    return _fileInfoAnnotation;
}

const QVector<AnnotatedVideo::Object>& AnnotatedVideo::objects() const
{
    return _objects;
}
