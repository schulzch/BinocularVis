#include "opencommand.h"
#include <QtWidgets>

OpenCommand::OpenCommand(const QFileInfo& fileInfo, int lineSkip, const QList<SharedFunction>& parents)
    : _fileInfo(fileInfo)
    , _parents(parents)
    , _lineSkip(lineSkip)
{
    setText("Open " + _fileInfo.fileName());
}

void OpenCommand::redo()
{
    QString suffix = _fileInfo.completeSuffix();
    if (suffix.compare("btd", Qt::CaseInsensitive) == 0
        || suffix.compare("avi", Qt::CaseInsensitive) == 0) {
        _video.reset(new LoadVideo(_fileInfo));
        for (const auto& parent : _parents) {
            Function::connect(_video, parent);
        }
    }
    else if (suffix.compare("tsv", Qt::CaseInsensitive) == 0
        || suffix.compare("txt", Qt::CaseInsensitive) == 0) {
        _csv.reset(new LoadCSV(_fileInfo, _lineSkip));
        for (const auto& parent : _parents) {
            Function::connect(_csv, parent);
        }
    }
    else {
        qWarning() << "Unknown file type" << suffix;
    }
}

void OpenCommand::undo()
{
    if (_csv) {
        for (const auto& parent : _parents) {
            Function::disconnect(_csv, parent);
        }
        _csv.clear();
    }
    if (_video) {
        for (const auto& parent : _parents) {
            Function::disconnect(_video, parent);
        }
        _video.clear();
    }
}
