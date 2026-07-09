#pragma once
#include <cstdint>
#include <string_view>

namespace anvil {

class Type;

class Value {
protected:
    enum Kind : uint8_t { Constant, Inst } _kind;

    Type            *_type;
    std::string_view _name;

    Value (Kind kind, Type *type, std::string_view name)
        : _kind (kind), _type (type), _name (name) {}

public:
    Kind
    GetKind () const {
        return _kind;
    }

    Type *
    GetType () const {
        return _type;
    }

    std::string_view
    GetName () const {
        return _name;
    }
};

}
