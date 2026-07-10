#pragma once
#include "anvil/collections/small_vector.h"
#include "anvil/core/context.h"
#include "anvil/core/value.h"
#include "anvil/ir/inst.h"
#include "anvil/streams/raw_ostream.h"
#include <cassert>

namespace anvil {

class BinaryOperator : public Inst {
    BinaryOperator (OpCode opcode, Value *lhs, Value *rhs, std::string_view name = "")
        : Inst (opcode, SmallVector<Value *, 2>{ lhs, rhs }, name) {}

public:
    RawOstream &
    Print (RawOstream &os) override {
        os << '/' << _name << " = " << OpCodeToStr (_opcode) << '.';
        _operands[0]->GetType ()->Print (os);
        os << ' ';
        _operands[0]->PrintAsOperand (os);
        os << ' ';
        _operands[1]->PrintAsOperand (os);
        return os;
    }

    RawOstream &
    PrintAsOperand (RawOstream &os) override {
        os << '/' << _name;
        return os;
    }

    static BinaryOperator *
    Create (
        OpCode opcode, Value *lhs, Value *rhs, Context &ctx, std::string_view name = "") {
        assert (
            lhs->GetType () == rhs->GetType ()
            && "Binary operator must have the same operand types");
        assert (lhs->GetType () != nullptr && "Operand has null type");

        auto &alloc = ctx.Allocator ();
        auto *mem   = alloc.Alloc<BinaryOperator> ();
        return ::new (mem) BinaryOperator (opcode, lhs, rhs, name);
    }

    static bool
    ClassOf (Value *val) {
        if (IsA<BinaryOperator> (val)) {
            return ClassOf (Cast<BinaryOperator> (val));
        }
        return false;
    }

    static bool
    ClassOf (const Value *val) {
        if (IsA<BinaryOperator> (val)) {
            return ClassOf (Cast<BinaryOperator> (val));
        }
        return false;
    }

    static bool
    ClassOf (BinaryOperator *val) {
        return val->Opcode () == Inst::Add;
    }

    static bool
    ClassOf (const BinaryOperator *val) {
        return val->Opcode () == Inst::Add;
    }
};

}
