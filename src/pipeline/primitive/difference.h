#ifndef DIFFERENCE_H
#define DIFFERENCE_H

#include <QVector3D>
#include "../function.h"
#include "vector.h"

/// This class implements a Function to compute the difference.
class Difference : public Function {
public:
    Difference(const QString& diffName, const QString& name);
    ~Difference() = default;

protected:
    const QList<const Value*> values() override;

    void call() override;

private:
    QString _name;
    VariantVector _vector;
    QList<const Value*> _values;
};

#endif
