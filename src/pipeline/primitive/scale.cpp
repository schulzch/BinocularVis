#include "scale.h"

Scale::Scale(double scale, const QString& scaleName, const QString& name)
    : _name(name)
    , _scale(scale)
    , _vector(scaleName)
{
    _values.append(&_vector);
}

const QList<const Value*> Scale::values()
{
    return _values;
}

void Scale::call()
{
    const VariantVector* otherVector = input<VariantVector>(_name);
    if (!otherVector) {
        return;
    }

    // Pre-allocate and scale.
    _vector.resize(otherVector->length());
    for (int i = 0; i < otherVector->length(); ++i) {
        if ((*otherVector)[i].isValid()) {
            _vector[i] = QVariant((*otherVector)[i].toDouble() * _scale);
        }
        else {
            _vector[i] = QVariant();
        }
    }
}
