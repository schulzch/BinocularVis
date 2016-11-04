#ifndef VALUE_H
#define VALUE_H

#include <QString>

/// This abstract class implements a value (pipes and filters pattern).
class Value {
public:
    virtual ~Value() = default;

    inline const QString& name() const
    {
        return _name;
    }

protected:
    Value(const QString& name)
        : _name(name)
    {
    }

private:
    QString _name;
};

#endif
