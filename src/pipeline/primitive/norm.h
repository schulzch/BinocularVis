#ifndef NORM_H
#define NORM_H

#include <QVector3D>
#include "../function.h"
#include "vector.h"

/// This class implements a Function to compute the norm.
class Norm : public Function {
public:
    Norm(const QString &normName, const QList<QString>& names);
    ~Norm() = default;

protected:
    const QList<const Value*> values() override;

    void call() override;

private:
    QList<QString> _names;
    VariantVector _vector;
    QList<const Value*> _values;
};

#endif
