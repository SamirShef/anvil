#include "anvil/target/x86/x86_reg_allocator.h"
#include "anvil/target/x86/x86_inst_info.h"

namespace anvil::x86 {

void
X86RegisterAllocator::allocInst (MachineInst *inst) {
    switch ((x86::OpCode) inst->Opcode ()) {
    case MOV64rm:
    case MOV64ri: {
        allocReg (inst->Operand (0));
        break;
    }
    case MOV64rr:
    case ADD64rr:
    case SUB64rr: {
        allocReg (inst->Operand (1)); // src
        allocReg (inst->Operand (0)); // dst
        break;
    }
    case RET: {
        allocReg (inst->Operand (0)); // rax
        break;
    }
    case MOV64mr: {
        allocReg (inst->Operand (1));
        break;
    }
    case SUB64ri:
    case PUSH64r:
    case POP64r: {
        allocReg (inst->Operand (0));
        break;
    }
    }
}

void
X86RegisterAllocator::allocReg (MachineOperand &op) {
    if (!op.IsReg ()) {
        return;
    }
    auto oldReg = op.AsReg ();
    if (!oldReg.IsVirtual ()) {
        return;
    }
    if (auto *found = _vregId2Physreg.Find (oldReg.Id ())) {
        op = MachineOperand::CreateReg (Register (*found), op.IsDef (), op.IsKill ());
        if (op.IsKill ()) {
            _availableRegs.insert (*found);
            _vregId2Physreg.Remove (oldReg.Id ());
        }
        return;
    }
    auto     physRegId = *_availableRegs.begin ();
    Register physReg (physRegId);
    _vregId2Physreg.Insert (oldReg.Id (), physRegId);
    _availableRegs.erase (physRegId);
    op = MachineOperand::CreateReg (physReg, op.IsDef (), op.IsKill ());
    if (op.IsKill ()) {
        _availableRegs.insert (physRegId);
        _vregId2Physreg.Remove (oldReg.Id ());
    }
}

}
