#include "mainwindow.h"
#include "opencv2/core/core.hpp"
#include "commands/opencommand.h"
#include "pipeline/primitive/align.h"
#include "pipeline/primitive/norm.h"
#include "pipeline/primitive/scale.h"
#include "pipeline/primitive/difference.h"
#include "pipeline/primitive/constant.h"
#include "pipeline/primitive/clamp.h"
#include "pipeline/timeseries/displaystc.h"
#include "pipeline/timeseries/displaysts.h"
#include "pipeline/timeseries/displayruler.h"
#include "pipeline/timeseries/displayactivityscarf.h"
#include "pipeline/timeseries/displayscarf.h"
#include "utils/colorbrewer.h"
#include <QtCore>
#include <QtWidgets>

static const QString SettingsPath = "./settings.ini";

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , _recentDir(".")
{
    _ui.setupUi(this);
    _ui.quitAction->setShortcut(QKeySequence::Quit);
    _ui.centralWidget->setScene(&_scene);

    setWindowTitle(QApplication::applicationName());

    _root.reset(new Root());
    //XXX: hardcoded pipelines :(
    _clamp = true;
    initTobiiPipeline();
    //initSMIPipeline();

    for (const auto& cName : _fn.keys()) {
        if (cName.startsWith("clamp_")) {
            for (const auto& fName : _fn.keys()) {
                if (!fName.startsWith("clamp_")) {
                    Function::connect(_fn[cName], _fn[fName]);
                }
            }
        }
    }

    _root->validate();
}

QString MainWindow::c(const QString& what, double from, double to, bool drop)
{
    QString cName = "clamp_" + what;
    _fn[cName].reset(new Clamp(cName, what, from, to, drop));
    return _clamp ? cName : what;
}

QString MainWindow::c(const QString& what)
{
    QString cName = "clamp_" + what;
    return _clamp ? cName : what;
}

void MainWindow::initTobiiPipeline()
{
    //XXX: hard coded line skip
    _lineSkip = 22;

    QList<QColor> colors(brew<QColor>("Set1", 9));

    _fn["displayRuler"].reset(new DisplayRuler(&_scene, "[s]", "TimeS", "TimeScaled"));
    Function::connect(_fn["displayRuler"], _root);
    _fn["displayActivity"].reset(new DisplayActivityScarf(&_scene, "Activity", 30, "TimeScaled", "Event"));
    Function::connect(_fn["displayActivity"], _root);

    QMap<int, QColor> validityColors{ { 0, "#abdda4" }, { 1, "#fdae61" }, { 2, "#d7191c" }, { 3, "#fdae61" }, { 4, "#ffffbf" } };
    _fn["displayValidity"].reset(new DisplayScarf(&_scene, "Validity", 60, { "TimeScaled", "ValidityLeft", "ValidityRight" }, validityColors));
    Function::connect(_fn["displayValidity"], _root);

    _fn["displayDistance"].reset(new DisplaySTS(&_scene, "Camera-Eye\nDistance", 60, colors[1], { "TimeScaled", "CameraDistanceLeft", "CameraDistanceRight" }));
    Function::connect(_fn["displayDistance"], _root);
    _fn["normDistanceLeft"].reset(new Norm("CameraDistanceLeft", QList<QString>({ c("CamXLeft", 0, 1e9), c("CamYLeft", 0, 1e9), c("DistanceLeft", 0, 1e9) })));

    Function::connect(_fn["normDistanceLeft"], _fn["displayDistance"]);
    _fn["normDistanceRight"].reset(new Norm("CameraDistanceRight", QList<QString>({ c("CamXRight", 0, 1e9), c("CamYRight", 0, 1e9), c("DistanceRight", 0, 1e9) })));
    Function::connect(_fn["normDistanceRight"], _fn["displayDistance"]);

    _fn["displayPupil"].reset(new DisplaySTS(&_scene, "Pupil Sizes", 60, colors[1], { "TimeScaled", c("PupilLeft", 0, 1e9), c("PupilRight", 0, 1e9) }));
    Function::connect(_fn["displayPupil"], _root);

    _fn["displayGazeXY"].reset(new DisplaySTC(&_scene, "Gaze\n[px, px]", 120, { "TimeScaled", c("GazePointX"), c("GazePointY"), "gazeSize", "gazeSize", "*" }));
    Function::connect(_fn["displayGazeXY"], _root);

    _fn["displayVelocity"].reset(new DisplaySTS(&_scene, "Gaze\nVelocity\n[px/s]", 60, colors[3], { "TimeScaled", "velocityLeft", "velocityRight", "velocityCombined" }));
    Function::connect(_fn["displayVelocity"], _root);

    _fn["normVelocityLeft"].reset(new Norm("velocityLeft", QList<QString>({ "diffXLeft", "diffYLeft" })));
    Function::connect(_fn["normVelocityLeft"], _fn["displayVelocity"]);
    _fn["normVelocityRight"].reset(new Norm("velocityRight", QList<QString>({ "diffXRight", "diffYRight" })));
    Function::connect(_fn["normVelocityRight"], _fn["displayVelocity"]);
    _fn["normVelocity"].reset(new Norm("velocityCombined", QList<QString>({ "diffX", "diffY" })));
    Function::connect(_fn["normVelocity"], _fn["displayVelocity"]);

    _fn["diffVelocityXLeft"].reset(new Difference("diffXLeft", c("GazePointXLeft", 0, 1280, false)));
    Function::connect(_fn["diffVelocityXLeft"], _fn["normVelocityLeft"]);
    _fn["diffVelocityYLeft"].reset(new Difference("diffYLeft", c("GazePointYLeft", 0, 1024, false)));
    Function::connect(_fn["diffVelocityYLeft"], _fn["normVelocityLeft"]);
    _fn["diffVelocityXRight"].reset(new Difference("diffXRight", c("GazePointXRight", 0, 1280, false)));
    Function::connect(_fn["diffVelocityXRight"], _fn["normVelocityRight"]);
    _fn["diffVelocityYRight"].reset(new Difference("diffYRight", c("GazePointYRight", 0, 1024, false)));
    Function::connect(_fn["diffVelocityYRight"], _fn["normVelocityRight"]);
    _fn["diffVelocityX"].reset(new Difference("diffX", c("GazePointX", 1, 1280, true)));
    Function::connect(_fn["diffVelocityX"], _fn["normVelocity"]);
    _fn["diffVelocityY"].reset(new Difference("diffY", c("GazePointY", 1, 1024, true)));
    Function::connect(_fn["diffVelocityY"], _fn["normVelocity"]);

    _fn["displayGazeX"].reset(new DisplaySTS(&_scene, "GazeX\n[px]", 60, colors[2], { "TimeScaled", c("GazePointXLeft"), c("GazePointXRight"), c("GazePointX") }));
    Function::connect(_fn["displayGazeX"], _root);
    _fn["displayGazeY"].reset(new DisplaySTS(&_scene, "GazeY\n[px]", 60, colors[2], { "TimeScaled", c("GazePointYLeft"), c("GazePointYRight"), c("GazePointY") }));
    Function::connect(_fn["displayGazeY"], _root);

    _fn["constGazeSize"].reset(new Constant("gazeSize", 32, "TimeScaled"));
    Function::connect(_fn["constGazeSize"], _fn["displayGazeXY"]);

    _fn["scaleTime"].reset(new Scale(1.0 / (1E3 / 60.0), "TimeScaled", "Timestamp"));
    _fn["scaleTime2"].reset(new Scale(1.0 / 1E3, "TimeS", "Timestamp"));
    Function::connect(_fn["scaleTime"], _fn["displayRuler"]);
    Function::connect(_fn["scaleTime2"], _fn["displayRuler"]);
    Function::connect(_fn["scaleTime"], _fn["displayActivity"]);
    Function::connect(_fn["scaleTime"], _fn["displayValidity"]);
    Function::connect(_fn["scaleTime"], _fn["displayDistance"]);
    Function::connect(_fn["scaleTime"], _fn["displayPupil"]);
    Function::connect(_fn["scaleTime"], _fn["displayGazeX"]);
    Function::connect(_fn["scaleTime"], _fn["displayGazeY"]);
    Function::connect(_fn["scaleTime"], _fn["displayVelocity"]);
    Function::connect(_fn["scaleTime"], _fn["displayGazeXY"]);
    Function::connect(_fn["scaleTime"], _fn["constGazeSize"]);
}

void MainWindow::initSMIPipeline()
{
    //XXX: hard coded line skip
    _lineSkip = 37;

    QList<QColor> colors(brew<QColor>("Set1", 9));

    _fn["displayRuler"].reset(new DisplayRuler(&_scene, "[ms]", "TimeMS", "TimeScaled"));
    Function::connect(_fn["displayRuler"], _root);

    _fn["displayLatency"].reset(new DisplaySTS(&_scene, "Latency\n[µs]", 60, colors[3], { "TimeScaled", "Latency" }));
    Function::connect(_fn["displayLatency"], _root);

    _fn["displayPupil"].reset(new DisplaySTS(&_scene, "Pupil Sizes\n[px]", 60, colors[1], { "TimeScaled", "L Dia [px]", "R Dia [px]" }));
    Function::connect(_fn["displayPupil"], _root);

    _fn["displayGazeXY_L"].reset(new DisplaySTC(&_scene, "Gaze L\n[px, px]", 120, { "TimeScaled", "L POR X [px]", "L POR Y [px]", "gazeSize", "gazeSize", "*" }));
    Function::connect(_fn["displayGazeXY_L"], _root);

    _fn["displayGazeXY_R"].reset(new DisplaySTC(&_scene, "Gaze R\n[px, px]", 120, { "TimeScaled", "R POR X [px]", "R POR Y [px]", "gazeSize", "gazeSize", "*" }));
    Function::connect(_fn["displayGazeXY_R"], _root);

    _fn["displayVelocity"].reset(new DisplaySTS(&_scene, "Gaze\nVelocity\n[px/s]", 60, colors[3], { "TimeScaled", "velocityLeft", "velocityRight" }));
    Function::connect(_fn["displayVelocity"], _root);

    _fn["normVelocityLeft"].reset(new Norm("velocityLeft", QList<QString>({ "diffXLeft", "diffYLeft" })));
    Function::connect(_fn["normVelocityLeft"], _fn["displayVelocity"]);
    _fn["normVelocityRight"].reset(new Norm("velocityRight", QList<QString>({ "diffXRight", "diffYRight" })));
    Function::connect(_fn["normVelocityRight"], _fn["displayVelocity"]);

    _fn["diffVelocityXLeft"].reset(new Difference("diffXLeft", "L POR X [px]"));
    Function::connect(_fn["diffVelocityXLeft"], _fn["normVelocityLeft"]);
    _fn["diffVelocityYLeft"].reset(new Difference("diffYLeft", "L POR Y [px]"));
    Function::connect(_fn["diffVelocityYLeft"], _fn["normVelocityLeft"]);
    _fn["diffVelocityXRight"].reset(new Difference("diffXRight", "R POR X [px]"));
    Function::connect(_fn["diffVelocityXRight"], _fn["normVelocityRight"]);
    _fn["diffVelocityYRight"].reset(new Difference("diffYRight", "R POR Y [px]"));
    Function::connect(_fn["diffVelocityYRight"], _fn["normVelocityRight"]);

    _fn["displayGazeX"].reset(new DisplaySTS(&_scene, "GazeX\n[px]", 60, colors[2], { "TimeScaled", "L POR X [px]", "R POR X [px]" }));
    Function::connect(_fn["displayGazeX"], _root);
    _fn["displayGazeY"].reset(new DisplaySTS(&_scene, "GazeY\n[px]", 60, colors[2], { "TimeScaled", "L POR Y [px]", "R POR Y [px]" }));
    Function::connect(_fn["displayGazeY"], _root);

    _fn["constGazeSize"].reset(new Constant("gazeSize", 32, "TimeScaled")); //XXX: check this number!
    Function::connect(_fn["constGazeSize"], _fn["displayGazeXY_L"]);
    Function::connect(_fn["constGazeSize"], _fn["displayGazeXY_R"]);

    _fn["scaleTime"].reset(new Scale(1.0 / (1E6 / 250.0), "TimeScaled", "TimeA"));
    _fn["scaleTime2"].reset(new Scale(1.0 / 1E6, "TimeMS", "TimeA"));
    Function::connect(_fn["scaleTime"], _fn["displayRuler"]);
    Function::connect(_fn["scaleTime2"], _fn["displayRuler"]);
    Function::connect(_fn["scaleTime"], _fn["displayLatency"]);
    Function::connect(_fn["scaleTime"], _fn["displayPupil"]);
    Function::connect(_fn["scaleTime"], _fn["displayGazeX"]);
    Function::connect(_fn["scaleTime"], _fn["displayGazeY"]);
    Function::connect(_fn["scaleTime"], _fn["displayVelocity"]);
    Function::connect(_fn["scaleTime"], _fn["displayGazeXY_L"]);
    Function::connect(_fn["scaleTime"], _fn["displayGazeXY_R"]);
    Function::connect(_fn["scaleTime"], _fn["constGazeSize"]);

    _fn["alignTime"].reset(new Align("TimeA", "TimeC"));
    Function::connect(_fn["alignTime"], _fn["displayRuler"]);
    Function::connect(_fn["alignTime"], _fn["scaleTime"]);
    Function::connect(_fn["alignTime"], _fn["scaleTime2"]);

    _fn["clampTime"].reset(new Clamp("TimeC", "Time", 19749918884, 19994950676, true)); //XXX: magic values :/
    Function::connect(_fn["clampTime"], _fn["alignTime"]);
}

void MainWindow::openFiles(const QStringList& fileNames)
{
    _recentDir = QFileInfo(fileNames.first()).path();

    // Modify pipeline and validate.
    for (const auto& fileName : fileNames) {
        _stack.push(new OpenCommand(fileName, _lineSkip, _fn.values()));
    }
    _root->validate();
}

void MainWindow::open()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(
        this, "Open files", _recentDir,
        "Recordings (*.tsv *.txt);;Annotations (*.btd)");
    if (fileNames.isEmpty()) {
        return;
    }
    openFiles(fileNames);
}

void MainWindow::undo()
{
    _stack.undo();
}

void MainWindow::redo()
{
    _stack.redo();
}

void MainWindow::play()
{
    QMessageBox::warning(this, "NYI", "XXX: NYI");
}

void MainWindow::pause()
{
    QMessageBox::warning(this, "NYI", "XXX: NYI");
}

void MainWindow::about()
{
    QString text = "Copyright © 2015 by University of Stuttgart (VISUS).\n"
                   "All rights reserved.\n"
                   "\n"
                   "Based on OpenCV %2 and Qt %3.\n"
                   "\n"
                   "Build on %1.";

    QString build = QString(__DATE__) + " " + QString(__TIME__);
    QString cvVersion(CV_VERSION);
    QMessageBox::about(this, "About", text.arg(build, cvVersion, qVersion()));
}

void MainWindow::toggleInfoSci()
{
    if (QApplication::queryKeyboardModifiers().testFlag(Qt::ControlModifier)) {
        qDebug() << "HACK: magic zoom!";
        _ui.centralWidget->setTransform(QTransform());
        _ui.centralWidget->scale(4, 4);
    }
    if (_ui.infoSciAction->text() == "InfoVis") {
        _ui.infoSciAction->setText("SciVis");
        _ui.centralWidget->setStyleSheet("QGraphicsView { color : black; }");
        _ui.centralWidget->setBackgroundBrush(Qt::white);
    }
    else {
        _ui.infoSciAction->setText("InfoVis");
        _ui.centralWidget->setStyleSheet("QGraphicsView { color : white; }");
        _ui.centralWidget->setBackgroundBrush(Qt::black);
    }
}

void MainWindow::showEvent(QShowEvent* event)
{
    QMainWindow::showEvent(event);
    QSettings settings(SettingsPath, QSettings::IniFormat);
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());
    _recentDir = settings.value("recentDir").toString();
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    QSettings settings(SettingsPath, QSettings::IniFormat);
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
    settings.setValue("recentDir", _recentDir);
    QMainWindow::closeEvent(event);
}
