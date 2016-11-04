#ifndef LOADCSV_H
#define LOADCSV_H

#include <QFileInfo>
#include "../function.h"
#include "variant.h"
#include "vector.h"

typedef QList<VariantVector> DimensionList;

/// This class implements a function to load CSV files.
class LoadCSV : public Function {
public:
    LoadCSV(const QFileInfo& fileInfo, int preambleLineSkip = 0);

protected:
    const QList<const Value*> values() override;

    void call() override;

private:
    Variant _preamble;
    DimensionList _dimensions;
    QList<const Value*> _values;
};

#endif
