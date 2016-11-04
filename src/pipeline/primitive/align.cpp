#include "align.h"

Align::Align(const QString& alignedName, const QString& name)
    : _name(name)
    , _vector(alignedName)
{
    _values.append(&_vector);
}

const QList<const Value*> Align::values()
{
    return _values;
}

void Align::call()
{
    const VariantVector* otherVector = input<VariantVector>(_name);
    if (!otherVector || otherVector->isEmpty()) {
        return;
    }

    // Pre-allocate and align.
    double start;
    bool startFound = false;
    _vector.resize(otherVector->length());
    for (int i = 0; i < otherVector->length(); ++i) {
        QVariant other = (*otherVector)[i];
        if (other.isValid()) {
            if (!startFound) {
                start = other.toDouble();
                startFound = true;
            }
            _vector[i] = QVariant(other.toDouble() - start);
        }
        else {
            _vector[i] = QVariant();
        }
    }
}
