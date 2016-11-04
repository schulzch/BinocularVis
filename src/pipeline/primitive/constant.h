#ifndef CONSTANT_H
#define CONSTANT_H

#include <QVector3D>
#include "../function.h"
#include "vector.h"

/// This class implements a Function to compute the Constant.
class Constant : public Function {
public:
    Constant(const QString& constName, const QVariant& value, const QString& timeName);
    ~Constant() = default;

protected:
    const QList<const Value*> values() override;

    void call() override;

private:
    QString _timeName;
    QVariant _value;
    VariantVector _vector;
    QList<const Value*> _values;
};

#endif
