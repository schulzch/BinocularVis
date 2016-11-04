#ifndef OPENCOMMAND_H
#define OPENCOMMAND_H

#include <QUndoCommand>
#include <QFileInfo>
#include "pipeline/function.h"
#include "pipeline/primitive/loadcsv.h"
#include "pipeline/timeseries/loadvideo.h"

class OpenCommand : public QUndoCommand {
public:
    OpenCommand(const QFileInfo& fileInfo, int lineSkip, const QList<SharedFunction>& parents);

    virtual void redo();
    virtual void undo();

private:
    QSharedPointer<LoadCSV> _csv;
    QSharedPointer<LoadVideo> _video;
    QFileInfo _fileInfo;
    int _lineSkip;
    QList<SharedFunction> _parents;
};

#endif
