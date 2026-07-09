#pragma once
#include "anvil/core/value.h"
#include <cstdint>

namespace anvil {

class Constant : public Value {
    friend class Context;

protected:
    enum Kind : uint8_t { Integer } _kind;

    explicit Constant (Kind kind, Type *type)
        : _kind (kind), Value (Value::Constant, type, "") {}

public:
    Kind
    GetConstantKind () const {
        return _kind;
    }

    static bool
    ClassOf (Value *val) {
        return val->GetKind () == Value::Constant;
    }

    static bool
    ClassOf (const Value *val) {
        return val->GetKind () == Value::Constant;
    }
};

}
