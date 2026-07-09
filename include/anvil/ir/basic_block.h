#pragma once
#include "anvil/core/value.h"
#include "anvil/ir/inst.h"
#include "anvil/streams/raw_ostream.h"
#include <cassert>

namespace anvil {

class BasicBlock : public Value {
    friend class Function;
    Inst       *_start{};
    Inst       *_end{};
    BasicBlock *_prev{};
    BasicBlock *_next{};

public:
    explicit BasicBlock (std::string_view name = "")
        : Value (Value::Kind::BasicBlock, nullptr, name) {}

    RawOstream &
    Print (RawOstream &os) override {
        os << "block " << _name << ":\n";
        auto *cur = _start;
        while (cur != nullptr && cur <= _end) {
            os << "  ";
            cur->Print (os) << '\n';
            cur = cur->Next ();
        }
        return os;
    }

    RawOstream &
    PrintAsOperand (RawOstream &os) override {
        os << '/' << _name;
        return os;
    }

    void
    AddInst (Inst *inst, Inst *insertPoint = nullptr) {
        assert (inst->_parent == nullptr && "Instruction already has parent BasicBlock");
        inst->_parent = this;
        if (insertPoint == nullptr) {
            AddInstAtEnd (inst);
        } else {
            assert (
                insertPoint->_parent == this
                && "Parent of insert point is other BasicBlock");
            inst->_prev = insertPoint;
            inst->_next = insertPoint->_next;
            if (insertPoint->_next != nullptr) {
                insertPoint->_next->_prev = inst;
            }
            insertPoint->_next = inst;
        }
    }

    void
    AddInstAtEnd (Inst *inst) {
        if (_end == nullptr) {
            _start = _end = inst;
            inst->_prev   = nullptr;
            inst->_next   = nullptr;
        } else {
            inst->_prev = _end;
            inst->_next = nullptr;
            _end->_next = inst;
            _end        = inst;
        }
    }

    Inst *
    Start () const {
        return _start;
    }

    Inst *
    End () const {
        return _end;
    }

    bool
    IsEmpty () const {
        return _start == nullptr;
    }

    BasicBlock *
    Next () const {
        return _next;
    }

    bool
    HasTerminator () const {
        return _end != nullptr && _end->Opcode () == Inst::OpCode::Ret;
    }

    Inst *
    Terminator () const {
        return HasTerminator () ? _end : nullptr;
    }

    static bool
    ClassOf (Value *val) {
        return val->GetKind () == Value::Kind::BasicBlock;
    }

    static bool
    ClassOf (const Value *val) {
        return val->GetKind () == Value::Kind::BasicBlock;
    }
};

}
