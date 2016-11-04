#include "mainwindow.h"
#include <QApplication>
#include <QtCore>

void loadStyle()
{
    QFile styleFile(":qdarkstyle/style.qss");
    if (!styleFile.open(QFile::ReadOnly | QFile::Text)) {
        qWarning() << "Unable to load style sheet";
    }
    static_cast<QApplication*>(QApplication::instance())->setStyleSheet(styleFile.readAll());
}

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName("BinocularVis");
    QApplication::setApplicationVersion("0.1");

    QCommandLineParser cmd;
    cmd.addHelpOption();
    cmd.addVersionOption();
    cmd.addPositionalArgument("files", QApplication::translate("main", "Files to open."), "[files]");

    cmd.process(app);
    const QStringList args = cmd.positionalArguments();

    loadStyle();
    MainWindow window;
    window.show();

    if (args.length() > 0) {
        window.openFiles(args);
    }

    return app.exec();
}
