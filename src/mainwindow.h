#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QUndoStack>
#include "pipeline/function.h"
#include "timeline/timelinescene.h"
#include "ui_mainwindow.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget* parent = 0);

    QString c(const QString& what, double from, double to, bool drop = true);
    QString c(const QString& what);
    void initTobiiPipeline();
    void initSMIPipeline();

    void openFiles(const QStringList& fileNames);

public slots:
    void open();

    void undo();
    void redo();

    void play();
    void pause();

    void about();

    void toggleInfoSci();

protected:
    virtual void showEvent(QShowEvent* event);
    virtual void closeEvent(QCloseEvent* event);

private:
    Ui::MainWindow _ui;
    QString _recentDir;
    TimelineScene _scene;

    QUndoStack _stack;
    QSharedPointer<Root> _root;
    QMap<QString, SharedFunction> _fn;
    int _lineSkip;

    bool _clamp;
};

#endif
