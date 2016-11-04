#ifndef STCITEM_H
#define STCITEM_H

#include "timelineitem.h"
#include <opencv2/opencv.hpp>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLBuffer>

/// Space-Time-Cube Item.
class STCItem : public TimelineItem, private QOpenGLFunctions {
public:
    STCItem(QGraphicsItem* parent = 0);

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);

    void setPoints(
        const QVector<QVariant>& time,
        const QVector<QVariant>& positionX,
        const QVector<QVariant>& positionY,
        const QVector<QVariant>& sizeX,
        const QVector<QVariant>& sizeY);

    void setImage(const QImage& image, float time);

protected:
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);

private:
    void initializePipelines();
    void validateBuffers();
    void validateTextures();
    void render(const QSizeF& deviceSize, const QTransform& deviceTransform);
    void renderImage(const QMatrix4x4& modelViewProjection);
    void renderCube(const QMatrix4x4& modelViewProjection);
    void renderPoints(const QMatrix4x4& modelViewProjection, const QVector4D& viewport, const QVector3D& cameraRay);

    QVector<QVector3D> _points;
    QVector<QVector3D> _sizes;
    QVector3D _pointsSize;
    bool _initialized;
    bool _validBuffers;
    bool _validTextures;

    float _rotation;

    QOpenGLShaderProgram _basicProgram;

    QOpenGLShaderProgram _skinProgram;
    QImage _image;
    float _imageTime;
    QOpenGLTexture _imageTexture;

    int _volumeMode;

    QOpenGLShaderProgram _volumeProgram;
    QOpenGLBuffer _positionBuffer;
    QOpenGLBuffer _colorBuffer;
    QOpenGLBuffer _sizeBuffer;
    GLuint _vertexCount;
};

#endif
