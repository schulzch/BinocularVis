#ifndef SCALE_H
#define SCALE_H

#include "../function.h"
#include "vector.h"

/// This class implements a Function to scale a value.
class Scale : public Function {
public:
    Scale(double scale, const QString& scaleName, const QString& name);
    ~Scale() = default;

protected:
    const QList<const Value*> values() override;

    void call() override;

private:
    QString _name;
    double _scale;
    VariantVector _vector;
    QList<const Value*> _values;
};

#endif
