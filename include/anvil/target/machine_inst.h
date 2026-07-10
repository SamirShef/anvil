#pragma once
#include "anvil/collections/small_vector.h"
#include "anvil/target/machine_operand.h"
#include <cstdint>

namespace anvil {

class MachineBasicBlock;

class MachineInst {
    friend class MachineBasicBlock;
    uint32_t                       _opcode;
    SmallVector<MachineOperand, 2> _operands;
    MachineBasicBlock             *_parent{};
    MachineInst                   *_prev{};
    MachineInst                   *_next{};

public:
    explicit MachineInst (uint32_t opcode) : _opcode (opcode) {}

    uint32_t
    Opcode () const {
        return _opcode;
    }

    MachineBasicBlock *
    Parent () const {
        return _parent;
    }

    void
    AddOperand (const MachineOperand &op) {
        _operands.PushBack (op);
    }

    size_t
    NumOperands () const {
        return _operands.Size ();
    }

    MachineOperand &
    Operand (size_t index) {
        return _operands[index];
    }

    const MachineOperand &
    Operand (size_t index) const {
        return _operands[index];
    }

    MachineInst *
    Prev () const {
        return _prev;
    }

    MachineInst *
    Next () const {
        return _next;
    }
};

}
