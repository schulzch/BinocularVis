#ifndef ALIGN_H
#define ALIGN_H

#include <QVector3D>
#include "../function.h"
#include "../primitive/vector.h"

class Align : public Function {
public:
    Align(const QString& alignedName, const QString& name);

protected:
    const QList<const Value*> values() override;

    void call() override;

private:
    QString _name;
    VariantVector _vector;
    QList<const Value*> _values;
};

#endif
