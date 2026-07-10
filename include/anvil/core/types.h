#pragma once
#include "anvil/streams/raw_ostream.h"
#include <cstdint>

namespace anvil {

class Context;

class Type {
public:
    enum Kind : uint8_t { Integer, Function };

protected:
    Kind _kind;

    explicit Type (Kind kind) : _kind (kind) {}

public:
    Type (const Type &) = default;
    Type (Type &&)      = delete;
    Type &
    operator= (const Type &) = default;
    Type &
    operator= (Type &&) = delete;
    virtual ~Type ()    = default;

    virtual RawOstream &
    Print (RawOstream &os) = 0;

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
    RawOstream &
    Print (RawOstream &os) override {
        os << 'i' << _width;
        return os;
    }

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
