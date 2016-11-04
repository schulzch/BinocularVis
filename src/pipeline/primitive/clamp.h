#ifndef CLAMP_H
#define CLAMP_H

#include <QVector3D>
#include "../function.h"
#include "../primitive/vector.h"

class Clamp : public Function {
public:
    Clamp(const QString& clampedName, const QString& name, double from, double to, bool drop);

protected:
    const QList<const Value*> values() override;

    void call() override;

private:
    QString _name;
    double _from;
    double _to;
    bool _drop;
    VariantVector _vector;
    QList<const Value*> _values;
};

#endif
