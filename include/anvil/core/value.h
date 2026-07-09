#pragma once
#include "anvil/streams/raw_ostream.h"
#include <cassert>
#include <cstdint>
#include <string_view>

namespace anvil {

class Type;
class Use;

class Value {
protected:
    enum class Kind : uint8_t { Constant, Inst, BasicBlock, Argument, Function } _kind;

    Type            *_type;
    std::string_view _name;
    Use             *_firstUse{};

    Value (Kind kind, Type *type, std::string_view name)
        : _kind (kind), _type (type), _name (name) {}

public:
    Value (const Value &) = default;
    Value (Value &&)      = default;
    Value &
    operator= (const Value &) = default;
    Value &
    operator= (Value &&) = default;

    virtual ~Value () {
        assert (_firstUse == nullptr && "Value already uses");
    }

    virtual RawOstream &
    Print (RawOstream &os) {
        os << '/' << _name;
        return os;
    }

    virtual RawOstream &
    PrintAsOperand (RawOstream &os) {
        os << '/' << _name;
        return os;
    }

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

    void
    SetName (std::string_view name) {
        _name = name;
    }

    Use *
    FirstUse () const {
        return _firstUse;
    }

    bool
    HasUsers () const {
        return _firstUse != nullptr;
    }

    void
    AddUse (Use *u);

    void
    RemoveUse (Use *u);
};

}
