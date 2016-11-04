#ifndef TIMELINEVIEW_H
#define TIMELINEVIEW_H

#include <QGraphicsView>
#include <QOpenGLFunctions>

class TimelineView : public QGraphicsView, private QOpenGLFunctions {
    Q_OBJECT
public:
    TimelineView(QWidget* parent = 0);

protected:
    void paintEvent(QPaintEvent* event) override;
    void showEvent(QShowEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;

private:
    bool _initialized;
    bool _showLabels;
};

#endif
