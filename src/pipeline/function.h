#ifndef FUNCTION_H
#define FUNCTION_H

#include <QList>
#include <QSharedPointer>
#include "value.h"

class Function;

typedef QSharedPointer<Function> SharedFunction;

/// This abstract class implements a function (pipes and filters pattern).
class Function {
public:
    static void connect(SharedFunction source, SharedFunction target)
    {
		Q_ASSERT(!source.isNull() && "Invalid function");
		Q_ASSERT(!target.isNull() && "Invalid function");
        source->_targets.append(target);
        target->_sources.append(source);
        target->invalidate();
    }

    static void disconnect(SharedFunction source, SharedFunction target)
    {
		Q_ASSERT(!source.isNull() && "Invalid function");
		Q_ASSERT(!target.isNull() && "Invalid function");
		target->_sources.removeAll(source);
        source->_targets.removeAll(target);
    }

    Function()
        : _valid(false)
    {
    }

    virtual ~Function() = default;

    void invalidate()
    {
        _valid = false;
        for (const auto& target : _targets) {
            target->invalidate();
        }
    }

    void validate()
    {
        if (_valid) {
            return;
        }
        for (const auto& source : _sources) {
            source->validate();
        }
        call();
        _valid = true;
    }

    bool isValid() const
    {
        return _valid;
    }

    const QList<SharedFunction>& sources()
    {
        return _sources;
    }

    const QList<SharedFunction>& targets()
    {
        return _targets;
    }

    template <typename T>
    const T* input(const QString& name = "*")
    {
        for (const auto& source : sources()) {
            for (const auto* value : source->values()) {
                const T* tValue = dynamic_cast<const T*>(value);
                if (tValue && (name == "*" || tValue->name() == name)) {
                    return tValue;
                }
            }
        }
        return nullptr;
    }

    template <typename T>
    QList<const T*> inputs()
    {
        QList<const T*> result;
        for (const auto& source : sources()) {
            for (const auto* value : source->values()) {
                const T* tValue = dynamic_cast<const T*>(value);
                if (tValue) {
                    result.append(tValue);
                }
            }
        }
        return result;
    }

    template <typename T>
    QList<const T*> inputs(const QList<QString>& names)
    {
        QList<const T*> result;
        for (const auto& source : sources()) {
            for (const auto* value : source->values()) {
                const T* tValue = dynamic_cast<const T*>(value);
                if (tValue && names.contains(tValue->name())) {
                    result.append(tValue);
                }
            }
        }
        return result;
    }

protected:
    virtual const QList<const Value*> values() = 0;

    virtual void call() = 0;

private:
    QList<SharedFunction> _sources;
    QList<SharedFunction> _targets;
    bool _valid;
};

/// This class implements a function which all other ones in a pipeline
/// depend on.
class Root : public Function {
public:
    Root() = default;
    ~Root() = default;

protected:
    inline const QList<const Value*> values()
    {
        return QList<const Value*>();
    }

    inline void call() override
    {
    }
};

#endif
