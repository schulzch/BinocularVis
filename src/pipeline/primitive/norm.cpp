#include "norm.h"

Norm::Norm(const QString& normName, const QList<QString>& names)
    : _names(names)
    , _vector(normName)
{
    _values.append(&_vector);
}

const QList<const Value*> Norm::values()
{
    return _values;
}

void Norm::call()
{
    QList<const VariantVector*> otherVectors = inputs<VariantVector>(_names);
    if (otherVectors.length() != _names.length() || otherVectors.isEmpty()) {
        return;
    }

    // Determine size for pre-allocation.
    int size = 0;
    for (const auto* otherVector : otherVectors) {
        size = qMax(size, otherVector->length());
    }

    // Pre-allocate and compute the norm.
    _vector.resize(size);
    for (int i = 0; i < size; ++i) {
        double sum = 0;
        bool valid = true;
        for (const auto* otherVector : otherVectors) {
            double value = (*otherVector)[i].toDouble();
            sum += value * value;
            valid &= (*otherVector)[i].isValid();
        }
        if (valid) {
            _vector[i] = QVariant(sqrtf(sum));
        }
        else {
            _vector[i] = QVariant();
        }
    }
}
