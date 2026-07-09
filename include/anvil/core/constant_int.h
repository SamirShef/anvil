#pragma once
#include "anvil/core/constant.h"
#include "anvil/support/casting.h"
#include <cstdint>

namespace anvil {

class ConstantInt : public Constant {
    friend class Context;

    uint64_t _val;

    ConstantInt (Type *type, uint64_t val)
        : _val (val), Constant (Constant::Integer, type) {}

public:
    uint64_t
    Val () const {
        return _val;
    }

    static bool
    CalssOf (Value *val) {
        if (val->GetKind () == Value::Constant) {
            return ConstantInt::ClassOf (Cast<Constant> (val));
        }
        return false;
    }

    static bool
    CalssOf (const Value *val) {
        if (val->GetKind () == Value::Constant) {
            return ConstantInt::ClassOf (Cast<Constant> (val));
        }
        return false;
    }

    static bool
    ClassOf (Constant *val) {
        return val->GetConstantKind () == Constant::Integer;
    }

    static bool
    ClassOf (const Constant *val) {
        return val->GetConstantKind () == Constant::Integer;
    }
};

}
