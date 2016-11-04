#include "timelineview.h"
#include "timelineitem.h"
#include <QtWidgets>
#include <QtCore>

class TimelineGLWidget : public QOpenGLWidget {
public:
    TimelineGLWidget(QWidget* parent = nullptr)
        : QOpenGLWidget(parent)
    {
        QSurfaceFormat format;
#ifdef B_OPENGL4
        format.setVersion(4, 5);
        format.setProfile(QSurfaceFormat::CompatibilityProfile);
#else
        format.setVersion(2, 1);
#endif
        format.setSamples(4);
        setFormat(format);
    }

    ~TimelineGLWidget()
    {
        teardownGL();
    }

protected:
    void initializeGL() override
    {
        qDebug() << "initializeGL";
        QOpenGLWidget::initializeGL();
        //TODO: propagate to timeline items.
    }

    void teardownGL()
    {
        //TODO: propagate to timeline items.
    }

    void resizeGL(int w, int h) override
    {
        qDebug() << "resizeGL";
        QOpenGLWidget::resizeGL(w, h);
    }
};

TimelineView::TimelineView(QWidget* parent)
    : QGraphicsView(parent)
    , _initialized(false)
{
    // Use OpenGL.
    setViewport(new TimelineGLWidget());
    // Enable anti-aliasing.
    setRenderHints(QPainter::Antialiasing | QPainter::HighQualityAntialiasing);
#ifdef QT_DEBUG
    // Force repainting (circumvent optimization)
    QTimer* timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(15);
#endif
}

void TimelineView::paintEvent(QPaintEvent* event)
{
#ifdef QT_DEBUG
    static QElapsedTimer* fpsTimer = nullptr;
    static qint64 fpsTime = 0;
    if (!fpsTimer) {
        fpsTimer = new QElapsedTimer();
        fpsTimer->start();
    }
    QElapsedTimer frameTimer;
    frameTimer.start();
#endif
    QGraphicsView::paintEvent(event);
#ifdef QT_DEBUG
    if (fpsTimer->elapsed() > fpsTime) {
        qDebug() << "TimelineView FPS:" << qPrintable(QString::number(1000.0 / frameTimer.elapsed(), 'f', 0));
        fpsTime += 5000;
    }
#endif
}

void TimelineView::showEvent(QShowEvent* event)
{
    QBrush background = palette().brush(QPalette::Active, QPalette::Base);
    setBackgroundBrush(background);
    QGraphicsView::showEvent(event);
}

void TimelineView::wheelEvent(QWheelEvent* event)
{
    int wheelAngle = event->angleDelta().y() / 8;
    qreal zoomFactor = qPow(1.0025, wheelAngle);
    bool zoomed = false;
    if (event->modifiers().testFlag(Qt::ControlModifier)) {
        scale(1.0, zoomFactor);
        zoomed = true;
    }
    if (event->modifiers().testFlag(Qt::ShiftModifier)) {
        scale(zoomFactor, 1.0);
        zoomed = true;
    }
    if (!zoomed) {
        QGraphicsView::wheelEvent(event);
    }
}

void TimelineView::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Control || event->key() == Qt::Key_Shift) {
        setDragMode(ScrollHandDrag);
    }
    else if (event->key() == Qt::Key_0 && event->modifiers().testFlag(Qt::ControlModifier)) {
        setTransform(QTransform());
    }
    else if (event->key() == Qt::Key_Plus && event->modifiers().testFlag(Qt::ControlModifier)) {
        scale(1.1, 1.1);
    }
    else if (event->key() == Qt::Key_Minus && event->modifiers().testFlag(Qt::ControlModifier)) {
        scale(0.9, 0.9);
    }
    else {
        QGraphicsView::keyPressEvent(event);
    }
}

void TimelineView::keyReleaseEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Control || event->key() == Qt::Key_Shift) {
        setDragMode(NoDrag);
    }
    else {
        QGraphicsView::keyReleaseEvent(event);
    }
}

void TimelineView::mouseDoubleClickEvent(QMouseEvent* event)
{
    QGraphicsItem* item = itemAt(event->pos());
    if (item && event->modifiers().testFlag(Qt::ControlModifier) || event->modifiers().testFlag(Qt::ShiftModifier)) {
        fitInView(item);
    }
    else {
        QGraphicsView::mouseDoubleClickEvent(event);
    }
}
