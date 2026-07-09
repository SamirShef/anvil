#pragma once
#include "anvil/core/value.h"
#include "anvil/ir/inst.h"
#include "anvil/streams/raw_ostream.h"
#include "anvil/support/casting.h"

namespace anvil {

class RetInst : public Inst {
    friend class IRBuilder;
    Value *_val;

    explicit RetInst (Value *val) : _val (val), Inst (Inst::OpCode::Ret, { val }) {}

public:
    RawOstream &
    Print (RawOstream &os) override {
        os << "ret ";
        return _val->PrintAsOperand (os);
    }

    RawOstream &
    PrintAsOperand (RawOstream &os) override {
        return os;
    }

    static bool
    ClassOf (Value *val) {
        if (IsA<Inst> (val)) {
            return ClassOf (Cast<Inst> (val));
        }
        return false;
    }

    static bool
    ClassOf (const Value *val) {
        if (IsA<Inst> (val)) {
            return ClassOf (Cast<Inst> (val));
        }
        return false;
    }

    static bool
    ClassOf (Inst *val) {
        return val->Opcode () == Inst::Add;
    }

    static bool
    ClassOf (const Inst *val) {
        return val->Opcode () == Inst::Add;
    }

    Value *
    Val () const {
        return _val;
    }
};

}
