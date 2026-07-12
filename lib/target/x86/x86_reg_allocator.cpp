#include "anvil/target/x86/x86_reg_allocator.h"

namespace anvil::x86 {

void
X86RegisterAllocator::allocInst (MachineInst *inst) {
    for (size_t i = 0; i < inst->NumOperands (); ++i) {
        auto &op = inst->Operand (i);
        if (op.IsReg ()) {
            allocReg (op, inst);
        }
    }
}

void
X86RegisterAllocator::allocReg (MachineOperand &op, MachineInst *inst) {
    if (!op.IsReg ()) {
        return;
    }
    auto oldReg = op.AsReg ();
    if (!oldReg.IsVirtual ()) {
        return;
    }
    if (auto *found = _vregId2Physreg.Find (oldReg.Id ())) {
        op = MachineOperand::CreateReg (Register (*found), op.IsDef (), op.IsKill ());
        return;
    }
    abort ();
}

}
