#pragma once
#include <cstdint>

namespace anvil {

class Context;

class Type {
protected:
    enum Kind : uint8_t { Integer } _kind;

    explicit Type (Kind kind) : _kind (kind) {}

public:
    Kind
    GetKind () const {
        return _kind;
    }
};

class IntegerType : public Type {
    friend class Context;
    unsigned _width;

    explicit IntegerType (unsigned width) : _width (width), Type (Type::Integer) {}

public:
    unsigned
    Width () const {
        return _width;
    }

    static bool
    ClassOf (Type *ty) {
        return ty->GetKind () == Type::Integer;
    }

    static bool
    ClassOf (const Type *ty) {
        return ty->GetKind () == Type::Integer;
    }
};

}
