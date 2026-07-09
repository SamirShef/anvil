#pragma once
#include "anvil/core/types.h"
#include "anvil/core/value.h"
#include "anvil/streams/raw_ostream.h"

namespace anvil {

class Function;

class Argument : public Value {
    Function *_parent;
    unsigned  _argNo;

public:
    Argument (Type *type, Function *parent, unsigned argNo, std::string_view name = "")
        : _parent (parent), _argNo (argNo), Value (Value::Kind::Argument, type, name) {}

    RawOstream &
    Print (RawOstream &os) override {
        _type->Print (os);
        return os << " /" << _name;
    }

    RawOstream &
    PrintAsOperand (RawOstream &os) override {
        return os << '/' << _name;
    }

    Function *
    Parent () const {
        return _parent;
    }

    unsigned
    ArgNo () const {
        return _argNo;
    }

    static bool
    ClassOf (Value *val) {
        return val->GetKind () == Value::Kind::Argument;
    }

    static bool
    ClassOf (const Value *val) {
        return val->GetKind () == Value::Kind::Argument;
    }
};

}
