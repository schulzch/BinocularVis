#include "clamp.h"

Clamp::Clamp(const QString& clampedName, const QString& name, double from, double to, bool drop)
    : _name(name)
    , _vector(clampedName)
    , _from(from)
    , _to(to)
    , _drop(drop)
{
    _values.append(&_vector);
}

const QList<const Value*> Clamp::values()
{
    return _values;
}

void Clamp::call()
{
    const VariantVector* otherVector = input<VariantVector>(_name);
    if (!otherVector || otherVector->isEmpty()) {
        return;
    }

    // Pre-allocate and clamp.
    _vector.resize(otherVector->length());
    for (int i = 0; i < otherVector->length(); ++i) {
        QVariant other = (*otherVector)[i];
        const double otherValue = other.toDouble();
        if (other.isValid() && otherValue >= _from && otherValue <= _to) {
            _vector[i] = QVariant(otherValue);
        }
        else if (!_drop) {
            _vector[i] = qBound(_from, otherValue, _to);
        }
        else {
            _vector[i] = QVariant();
        }
    }
}
