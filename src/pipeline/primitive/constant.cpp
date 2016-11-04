#include "constant.h"

Constant::Constant(const QString& constName, const QVariant& value, const QString& timeName)
    : _timeName(timeName)
    , _value(value)
    , _vector(constName)
{
    _values.append(&_vector);
}

const QList<const Value*> Constant::values()
{
    return _values;
}

void Constant::call()
{
    const VariantVector* time = input<VariantVector>(_timeName);
    if (!time || time->isEmpty()) {
        return;
    }

    // Pre-allocate and assign.
    _vector.resize(time->length());
    for (int i = 0; i < time->length(); ++i) {
        _vector[i] = _value;
    }
}
