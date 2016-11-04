#ifndef VECTOR_H
#define VECTOR_H

#include <QVector>
#include <QVariant>
#include "../value.h"

template <typename T>
class Vector : public QVector<T>, public Value {
public:
    Vector(const QString& name)
        : QVector<T>()
        , Value(name)
    {
    }
};

typedef Vector<QVariant> VariantVector;

#endif
