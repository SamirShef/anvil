#pragma once
#include "anvil/collections/small_vector.h"
#include "anvil/core/use.h"
#include "anvil/core/value.h"
#include <cassert>

namespace anvil {

class BasicBlock;

class Inst : public Value {
    friend class BasicBlock;

public:
    enum OpCode : uint8_t { Add, Sub, Ret };

protected:
    OpCode                  _opcode;
    BasicBlock             *_parent{};
    Inst                   *_prev{};
    Inst                   *_next{};
    SmallVector<Value *, 2> _operands;
    SmallVector<Use *, 2>   _operandLinks;

    Inst (OpCode kind, SmallVector<Value *, 2> operands, std::string_view name = "")
        : _opcode (kind),
          _operands (std::move (operands)),
          Value (Value::Kind::Inst, operands[0]->GetType (), name) {
        for (size_t i = 0; i < _operands.Size (); ++i) {
            Value *op = _operands[i];
            assert (op != nullptr && "Operand cannot be a nullptr");
            auto *link = new Use (op, this);
            _operandLinks.PushBack (link);
            if (op != nullptr) {
                op->AddUse (link);
            }
        }
    }

public:
    Inst (const Inst &) = delete;
    Inst (Inst &&)      = default;
    Inst &
    operator= (const Inst &) = delete;
    Inst &
    operator= (Inst &&) = default;

    ~Inst () override {
        for (size_t i = 0; i < _operandLinks.Size (); ++i) {
            Use *link = _operandLinks[i];
            if (link->Get () != nullptr) {
                link->Get ()->RemoveUse (link);
            }
            delete link;
        }
    }

    OpCode
    Opcode () const {
        return _opcode;
    }

    Value *
    Operand (size_t index) {
        return _operands[index];
    }

    void
    SetOperand (size_t index, Value *newVal) {
        assert (index < _operands.Size () && "Index out of range");
        Value *oldVal = _operands[index];
        if (oldVal == newVal) {
            return;
        }

        _operands[index] = newVal;

        Use *link = _operandLinks[index];
        if (oldVal != nullptr) {
            oldVal->RemoveUse (link);
        }
        link->_value = newVal;
        if (newVal != nullptr) {
            newVal->AddUse (link);
        }
    }

    Inst *
    Prev () const {
        return _prev;
    }

    Inst *
    Next () const {
        return _next;
    }

    BasicBlock *
    Parent () const {
        return _parent;
    }

    static bool
    ClassOf (Value *val) {
        return val->GetKind () == Value::Kind::Inst;
    }

    static bool
    ClassOf (const Value *val) {
        return val->GetKind () == Value::Kind::Inst;
    }
};

static inline const char *
OpCodeToStr (Inst::OpCode opcode) {
    using OpCode = Inst::OpCode;
    switch (opcode) {
    case OpCode::Add:
        return "add";
    case OpCode::Sub:
        return "sub";
    default:
        return "";
    }
}

}
