#include "difference.h"

Difference::Difference(const QString& diffName, const QString& name)
    : _name(name)
    , _vector(diffName)
{
    _values.append(&_vector);
}

const QList<const Value*> Difference::values()
{
    return _values;
}

void Difference::call()
{
    const VariantVector* otherVector = input<VariantVector>(_name);
    if (!otherVector || otherVector->isEmpty()) {
        return;
    }

    // Pre-allocate and compute the difference.
    _vector.resize(otherVector->length());
    double lastValue = (*otherVector)[0].toDouble();
    _vector[0] = QVariant(0);
    for (int i = 1; i < otherVector->length(); ++i) {
        double value = (*otherVector)[i].toDouble();
        if ((*otherVector)[i].isValid()) {
            _vector[i] = QVariant(lastValue - value);
        }
        else {
            _vector[i] = QVariant();
        }
        lastValue = value;
    }
}
