#ifndef VARIANT_H
#define VARIANT_H

#include <QVariant>
#include "../value.h"

class Variant : public QVariant, public Value {
public:
    Variant(const QString& name)
        : QVariant()
        , Value(name)
    {
    }
};

#endif
