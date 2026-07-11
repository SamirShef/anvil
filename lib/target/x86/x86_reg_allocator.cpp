#include "anvil/target/x86/x86_reg_allocator.h"
#include "anvil/target/x86/x86_inst_info.h"

namespace anvil::x86 {

void
X86RegisterAllocator::allocInst (MachineInst *inst) {
    switch ((x86::OpCode) inst->Opcode ()) {
    case MOV64rm:
    case MOV64ri: {
        allocReg (inst->Operand (0), inst);
        break;
    }
    case MOV64rr:
    case ADD64rr:
    case SUB64rr: {
        allocReg (inst->Operand (1), inst); // src
        allocReg (inst->Operand (0), inst); // dst
        break;
    }
    case RET: {
        allocReg (inst->Operand (0), inst); // rax
        break;
    }
    case MOV64mr: {
        allocReg (inst->Operand (1), inst);
        break;
    }
    case SUB64ri:
    case PUSH64r:
    case POP64r: {
        allocReg (inst->Operand (0), inst);
        break;
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
        if (op.IsKill ()) {
            _availableRegs.insert (*found);
            _vregId2Physreg.Remove (oldReg.Id ());
        }
        return;
    }
    if (!_availableRegs.empty ()) {
        auto     physRegId = *_availableRegs.begin ();
        Register physReg (physRegId);
        _vregId2Physreg.Insert (oldReg.Id (), physRegId);
        _availableRegs.erase (physRegId);

        if (auto *slot = _vregId2StackSlot.Find (oldReg.Id ())) {
            if (op.IsUse ()) {
                auto *reloadMI = _mctx.Allocator ().Alloc<MachineInst> ();
                ::new (reloadMI) MachineInst (x86::MOV64rm);
                reloadMI->AddOperand (MachineOperand::CreateReg (physReg, true));
                reloadMI->AddOperand (MachineOperand::CreateFI (*slot));
                inst->Parent ()->InsertBefore (reloadMI, inst);
            }
        }

        op = MachineOperand::CreateReg (physReg, op.IsDef (), op.IsKill ());
        if (op.IsKill ()) {
            _availableRegs.insert (physRegId);
            _vregId2Physreg.Remove (oldReg.Id ());
        }
        return;
    }

    uint32_t             stolenRegId  = 0;
    static const RegInfo CANDIDATES[] = { RAX, RCX, RDX, RSI, RDI, R8, R9, R10, R11 };

    for (auto cand : CANDIDATES) {
        bool isUsedInCurrentInst = false;
        for (size_t i = 0; i < inst->NumOperands (); ++i) {
            if (inst->Operand (i).IsReg () && inst->Operand (i).AsReg ().Id () == cand) {
                isUsedInCurrentInst = true;
                break;
            }
        }
        if (!isUsedInCurrentInst) {
            stolenRegId = cand;
            break;
        }
    }

    if (_vregId2StackSlot.Find (oldReg.Id ()) == nullptr) {
        int fi = _curFunc->CreateStackSlot ();
        _vregId2StackSlot.Insert (oldReg.Id (), fi);
    }
    int fi = *_vregId2StackSlot.Find (oldReg.Id ());

    auto *pushMI = _mctx.Allocator ().Alloc<MachineInst> ();
    ::new (pushMI) MachineInst (x86::PUSH64r);
    pushMI->AddOperand (MachineOperand::CreateReg (Register (stolenRegId)));
    inst->Parent ()->InsertBefore (pushMI, inst);

    if (op.IsUse ()) {
        auto *loadMI = _mctx.Allocator ().Alloc<MachineInst> ();
        ::new (loadMI) MachineInst (x86::MOV64rm);
        loadMI->AddOperand (MachineOperand::CreateReg (Register (stolenRegId), true));
        loadMI->AddOperand (MachineOperand::CreateFI (fi));
        inst->Parent ()->InsertBefore (loadMI, inst);
    }

    op = MachineOperand::CreateReg (Register (stolenRegId), op.IsDef (), op.IsKill ());

    auto *popMI = _mctx.Allocator ().Alloc<MachineInst> ();
    ::new (popMI) MachineInst (x86::POP64r);
    popMI->AddOperand (MachineOperand::CreateReg (Register (stolenRegId)));
    inst->Parent ()->InsertAfter (popMI, inst);

    if (op.IsDef ()) {
        auto *storeMI = _mctx.Allocator ().Alloc<MachineInst> ();
        ::new (storeMI) MachineInst (x86::MOV64mr);
        storeMI->AddOperand (MachineOperand::CreateFI (fi));
        storeMI->AddOperand (MachineOperand::CreateReg (Register (stolenRegId)));
        inst->Parent ()->InsertAfter (storeMI, inst);
    }
}

}
