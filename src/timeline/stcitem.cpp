#include "stcitem.h"
#include <QtWidgets>
#include <QtGui>

void findMinMax(QVector3D& min, QVector3D& max, const QVector<QVector3D>& points)
{
    const float RealMin = std::numeric_limits<float>::min();
    const float RealMax = std::numeric_limits<float>::max();
    min = QVector3D(RealMax, RealMax, RealMax);
    max = QVector3D(RealMin, RealMin, RealMin);
    for (const auto& point : points) {
        min.setX(qMin(min.x(), point.x()));
        min.setY(qMin(min.y(), point.y()));
        min.setZ(qMin(min.z(), point.z()));
        max.setX(qMax(max.x(), point.x()));
        max.setY(qMax(max.y(), point.y()));
        max.setZ(qMax(max.z(), point.z()));
    }
}

QString loadShader(const QString& name)
{
    QFile shaderFile(":timeline/" + name);
    if (!shaderFile.open(QFile::ReadOnly | QFile::Text)) {
        qWarning() << "Unable to load shader" << name;
    }
    QString source = shaderFile.readAll();
    return source;
}

STCItem::STCItem(QGraphicsItem* parent)
    : TimelineItem(parent)
    , _initialized(false)
    , _validBuffers(false)
    , _validTextures(false)
    , _rotation(0)
    , _imageTime(0)
    , _imageTexture(QOpenGLTexture::Target2D)
    , _volumeMode(2)
    , _positionBuffer(QOpenGLBuffer::VertexBuffer)
    , _colorBuffer(QOpenGLBuffer::VertexBuffer)
    , _sizeBuffer(QOpenGLBuffer::VertexBuffer)
{
}

void STCItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    TimelineItem::paint(painter, option, widget);

#ifdef B_OPENGL4
    if (painter->paintEngine()->type() != QPaintEngine::OpenGL
        && painter->paintEngine()->type() != QPaintEngine::OpenGL2) {
        qWarning("Item requires an OpenGL paint engine");
        return;
    }

    painter->beginNativePainting();

    if (!_initialized) {
        initializeOpenGLFunctions();
        initializePipelines();
        _initialized = true;
    }

    if (!_validBuffers) {
        validateBuffers();
        _validBuffers = true;
    }

    if (!_validTextures) {
        validateTextures();
        _validTextures = true;
    }

    QSizeF deviceSize(painter->device()->width(), painter->device()->height());
    render(deviceSize, painter->deviceTransform());

    //XXX: ugly workaround to prevent bad destruction of texture
    _imageTexture.destroy();
    _validTextures = false;

    painter->endNativePainting();
#else
    // Draw cross to indicate missing OpenGL 4.x support.
    QRectF bounds(boundingRect());
    painter->setPen(Qt::red);
    painter->drawLine(bounds.topLeft(), bounds.bottomRight());
    painter->drawLine(bounds.bottomLeft(), bounds.topRight());
#endif
}

void STCItem::setPoints(
    const QVector<QVariant>& time,
    const QVector<QVariant>& positionX,
    const QVector<QVariant>& positionY,
    const QVector<QVariant>& sizeX,
    const QVector<QVariant>& sizeY)
{
    int pointIndex = 0;
    _points.resize(time.length());
    _sizes.resize(time.length());
    for (int i = 0; i < time.length(); ++i) {
        if (!time[i].isValid()
            || !positionX[i].isValid() || !positionY[i].isValid()
            || !sizeX[i].isValid() || !sizeY[i].isValid()) {
            continue;
        }
        float timeValue = time[i].toFloat();
        _points[pointIndex].setX(positionX[i].toFloat());
        _points[pointIndex].setY(positionY[i].toFloat());
        _points[pointIndex].setZ(timeValue);
        _sizes[pointIndex].setX(sizeX[i].toFloat() * 2.0f);
        _sizes[pointIndex].setY(sizeY[i].toFloat() * 2.0f);
        if (i + 1 != time.length()) {
            float nextTimeValue = time[i + 1].toFloat();
            float delta = nextTimeValue - timeValue;
            _sizes[pointIndex].setZ(delta * 2.0f);
        }
        else {
            _sizes[pointIndex].setZ(0);
        }
        pointIndex++;
    }
    _points.resize(pointIndex);
    _sizes.resize(pointIndex);

    auto minmax = std::minmax_element(time.begin(), time.end());
    setWidth((*minmax.second).toDouble());

    _validBuffers = false;
    update();
}

void STCItem::setImage(const QImage& image, float time)
{
    _image = image;
    _imageTime = time;
    _validTextures = false;
    update();
}

void STCItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    QPointF deltaPos = event->scenePos() - event->lastScenePos();
    QPointF deltaAngle = deltaPos * 0.5f;
    _rotation += deltaAngle.y();
    update();

    TimelineItem::mouseMoveEvent(event);
}

void STCItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    TimelineItem::mousePressEvent(event);
    if (event->modifiers() == Qt::NoModifier) {
        if (event->button() == Qt::RightButton) {
            _volumeMode = (_volumeMode + 1) % 4;
        }
        event->accept();
    }
}

void STCItem::initializePipelines()
{
    bool build;

    build = _basicProgram.create();
    build &= _basicProgram.addShaderFromSourceCode(QOpenGLShader::Vertex, loadShader("stcitem.basic.vert"));
    build &= _basicProgram.addShaderFromSourceCode(QOpenGLShader::Fragment, loadShader("stcitem.basic.frag"));
    build &= _basicProgram.link();
    if (!build) {
        qCritical() << "Building basic program failed";
        QApplication::exit(-1);
    }

    build = _skinProgram.create();
    build &= _skinProgram.addShaderFromSourceCode(QOpenGLShader::Vertex, loadShader("stcitem.skin.vert"));
    build &= _skinProgram.addShaderFromSourceCode(QOpenGLShader::Fragment, loadShader("stcitem.skin.frag"));
    build &= _skinProgram.link();
    if (!build) {
        qCritical() << "Building skin program failed";
        QApplication::exit(-1);
    }

    build = _volumeProgram.create();
    build &= _volumeProgram.addShaderFromSourceCode(QOpenGLShader::Vertex, loadShader("stcitem.volume.vert"));
    build &= _volumeProgram.addShaderFromSourceCode(QOpenGLShader::Geometry, loadShader("stcitem.volume.geom"));
    build &= _volumeProgram.addShaderFromSourceCode(QOpenGLShader::Fragment, loadShader("stcitem.volume.frag"));
    build &= _volumeProgram.link();
    if (!build) {
        qCritical() << "Building volume program failed";
        QApplication::exit(-1);
    }

    _vertexCount = 0;

    _positionBuffer.create();
    _positionBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);

    _colorBuffer.create();
    _colorBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);

    _sizeBuffer.create();
    _sizeBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
}

void STCItem::validateBuffers()
{
    _vertexCount = _points.length();

    QVector3D _pointsMin;
    QVector3D _pointsMax;
    findMinMax(_pointsMin, _pointsMax, _points);

    _pointsSize = QVector3D(_pointsMax.x() - _pointsMin.x(),
        _pointsMax.y() - _pointsMin.y(),
        _pointsMax.z() - _pointsMin.z());

    GLfloat* positions = new GLfloat[_vertexCount * 3];
    GLfloat* colors = new GLfloat[_vertexCount * 3];
    GLfloat* sizes = new GLfloat[_vertexCount * 3];

    for (GLuint i = 0; i < _vertexCount; ++i) {
        const int j = i * 3;
        positions[j + 0] = _points[i].x();
        positions[j + 1] = _points[i].y();
        positions[j + 2] = _points[i].z();
        colors[j + 0] = 1; //i % 3 == 0 ? 1.0f : 0.0f;
        colors[j + 1] = 0; //i % 3 == 1 ? 1.0f : 0.0f;
        colors[j + 2] = 0; //i % 3 == 2 ? 1.0f : 0.0f;
        sizes[j + 0] = _sizes[i].x();
        sizes[j + 1] = _sizes[i].y();
        sizes[j + 2] = _sizes[i].z();
    }

    _positionBuffer.bind();
    _positionBuffer.allocate(positions, _vertexCount * 3 * sizeof(GLfloat));
    _positionBuffer.release();

    _colorBuffer.bind();
    _colorBuffer.allocate(colors, _vertexCount * 3 * sizeof(GLfloat));
    _colorBuffer.release();

    _sizeBuffer.bind();
    _sizeBuffer.allocate(sizes, _vertexCount * 3 * sizeof(GLfloat));
    _sizeBuffer.release();

    delete[] positions;
    delete[] colors;
    delete[] sizes;
}

void STCItem::validateTextures()
{
    _imageTexture.setData(_image);
}

void STCItem::render(const QSizeF& deviceSize, const QTransform& deviceTransform)
{
    // Map from logical Qt coordinates to OpenGL device coordiantes.
    auto glMapRect = [&](const QRectF& rect) {
        const QRectF mappedRect = deviceTransform.mapRect(rect);
        return QVector4D(mappedRect.left(),
            deviceSize.height() - mappedRect.bottom(),
            mappedRect.width(),
            mappedRect.height());
    };

    // Setup viewport and scissor rectangle.
    const QVector4D deviceRect(0, 0, deviceSize.width(), deviceSize.height());
    const QVector4D qtRect = glMapRect(boundingRect());
    glViewport(deviceRect.x(), deviceRect.y(), deviceRect.z(), deviceRect.w());
    glScissor(qtRect.x(), qtRect.y(), qtRect.z(), qtRect.w());
    glEnable(GL_SCISSOR_TEST);

    // Transformation usually works like this:
    //   Window = ViewportGL * Projection * View * Model
    // Because of limitations of glViewport() the following workaround is applied:
    //   Window = ViewportGL * ViewportGL^-1 * ViewportQt * Projection * View * Model
    QMatrix4x4 viewportGL;
    viewportGL.viewport(deviceRect.x(), deviceRect.y(), deviceRect.z(), deviceRect.w());
    QMatrix4x4 viewportQt;
    viewportQt.viewport(qtRect.x(), qtRect.y(), qtRect.z(), qtRect.w());
    QMatrix4x4 viewportWorkaround = viewportGL.inverted() * viewportQt;

    // Create orthographic and aspect-ratio preserving projection.
    const qreal aspect = qtRect.z() / qtRect.w();
    QMatrix4x4 projection;
    if (aspect >= 1) {
        projection.ortho(-1.0f * aspect, 1.0f * aspect, -1.0f, 1.0f, -10000.0f, 10000.0f);
    }
    else {
        projection.ortho(-1.0f, 1.0f, -1.0f / aspect, 1.0f / aspect, -10000.0f, 10000.0f);
    }

    // Create isometric view with z axis aligned horizontally.
    QMatrix4x4 view;
    view.rotate(30.0f, 0.0f, 0.0f, 1.0f);
    view.rotate(35.264f, 1.0f, 0.0f, 0.0f);
    view.rotate(45.0f, 0.0f, 1.0f, 0.0f);

    // Transform bounding cube from world to view space.
    const float maxXY = qMax(_pointsSize.x(), _pointsSize.y());
    const QVector3D halfPointsSize(maxXY / 2, maxXY / 2, _pointsSize.z() / 2); // XXX: bugged?
    const QVector3D halfPointsSize2(_pointsSize.x() / 2, _pointsSize.y() / 2, _pointsSize.z() / 2);
    const QVector4D a = view * -halfPointsSize;
    const QVector4D b = view * halfPointsSize;
    const QVector3D viewPointsSize(
        qMax(a.x(), b.x()) - qMin(a.x(), b.x()),
        qMax(a.y(), b.y()) - qMin(a.y(), b.y()),
        qMax(a.z(), b.z()) - qMin(a.z(), b.z()));

    // Scale and translate model to fit camera.
    const float scaleXY = 2.0f / viewPointsSize.y();
    const float scaleZ = (2.0f * aspect) / viewPointsSize.x();
    QMatrix4x4 model;
    model.rotate(_rotation, 0, 0, 1);
    model.scale(scaleXY, scaleXY, scaleZ); // * 0.7);
    model.translate(-halfPointsSize2); // XXX: translation is bugged?

    // Combine to model-view-projection matrix.
    QMatrix4x4 mvp = viewportWorkaround * projection * view * model;

    // Compute translation from image time.
    QMatrix4x4 imageTranslation;
    imageTranslation.translate(0, 0, _imageTime);

    // Scale from unit space to points space.
    QMatrix4x4 pointsScale;
    pointsScale.scale(_pointsSize);

    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    // Render image.
    if (!_image.isNull()) {
        renderImage(mvp * imageTranslation * pointsScale);
    }
    // Render cube.
    renderCube(mvp * pointsScale);
    glDisable(GL_DEPTH_TEST);

    // Render points.
    QVector3D cameraRay = (mvp.inverted() * QVector3D(0, 0, 1));
    renderPoints(mvp, deviceRect, cameraRay);
}

void STCItem::renderImage(const QMatrix4x4& modelViewProjection)
{
    const GLfloat positions[] = {
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
    };

    const GLfloat uvs[] = {
        0.0f, 1.0f,
        1.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,
    };

    _skinProgram.bind();

    glActiveTexture(GL_TEXTURE0);
    _imageTexture.bind();

    _skinProgram.setUniformValue(0, modelViewProjection);
    _skinProgram.setUniformValue(1, 0);

    _skinProgram.enableAttributeArray(0);
    _skinProgram.setAttributeArray(0, GL_FLOAT, positions, 3);

    _skinProgram.enableAttributeArray(1);
    _skinProgram.setAttributeArray(1, GL_FLOAT, uvs, 2);

    glEnable(GL_TEXTURE_2D);
    glDrawArrays(GL_TRIANGLES, 0, sizeof(positions) / sizeof(GLfloat) / 3);
    glDisable(GL_TEXTURE_2D);

    _skinProgram.disableAttributeArray(0);
    _skinProgram.disableAttributeArray(1);
    _skinProgram.disableAttributeArray(2);

    _imageTexture.release();

    _skinProgram.release();
}

void STCItem::renderCube(const QMatrix4x4& modelViewProjection)
{
    const GLfloat positions[] = {
        // front
        0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        // back
        0.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        // top
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 1.0f,
        // bottom
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 1.0f,
        // center
        0.5f, 0.5f, 0.0f,
        0.5f, 0.5f, 1.0f
    };

    const GLfloat colors[] = {
        // front
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        // back
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        // top
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        // bottom
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        // center
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f
    };

    _basicProgram.bind();

    _basicProgram.setUniformValue(0, modelViewProjection);

    _basicProgram.enableAttributeArray(0);
    _basicProgram.setAttributeArray(0, GL_FLOAT, positions, 3);

    _basicProgram.enableAttributeArray(1);
    _basicProgram.setAttributeArray(1, GL_FLOAT, colors, 3);

    glDrawArrays(GL_LINES, 0, sizeof(positions) / sizeof(GLfloat) / 3);

    _basicProgram.disableAttributeArray(0);
    _basicProgram.disableAttributeArray(1);

    _basicProgram.release();
}

void STCItem::renderPoints(const QMatrix4x4& modelViewProjection, const QVector4D& viewport, const QVector3D& cameraRay)
{
    _volumeProgram.bind();

    _volumeProgram.setUniformValue(0, modelViewProjection);
    _volumeProgram.setUniformValue(1, modelViewProjection.inverted());
    _volumeProgram.setUniformValue(2, viewport);
    _volumeProgram.setUniformValue(3, cameraRay);
    _volumeProgram.setUniformValue(4, _volumeMode);

    _positionBuffer.bind();
    _volumeProgram.enableAttributeArray(0);
    _volumeProgram.setAttributeBuffer(0, GL_FLOAT, 0, 3);

    _colorBuffer.bind();
    _volumeProgram.enableAttributeArray(1);
    _volumeProgram.setAttributeBuffer(1, GL_FLOAT, 0, 3);

    _sizeBuffer.bind();
    _volumeProgram.enableAttributeArray(2);
    _volumeProgram.setAttributeBuffer(2, GL_FLOAT, 0, 3);

    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDrawArrays(GL_POINTS, 0, _vertexCount);
    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);

    _volumeProgram.disableAttributeArray(2);
    _sizeBuffer.release();

    _volumeProgram.disableAttributeArray(1);
    _colorBuffer.release();

    _volumeProgram.disableAttributeArray(0);
    _positionBuffer.release();

    _volumeProgram.release();
}
