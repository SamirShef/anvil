#include "anvil/target/x86/x86_pe_inserter.h"
#include "anvil/target/x86/x86_inst_info.h"
#include "anvil/target/x86/x86_register_info.h"

namespace anvil::x86 {

void
X86PEInserter::processFunction (MachineFunction *func, MachineContext &mctx) {
    auto *entryBB = func->BlockStart ();
    if (entryBB == nullptr) {
        return;
    }

    size_t numSlots = func->NumStackSlots ();

    // System V ABI: align stack by 16 bytes before calls
    uint32_t stackSize = ((numSlots * 8) + 15) & ~15;

    // pushq %rbp
    auto *pushRbp = emitPush (Register (RBP));

    // movq %rsp, %rbp
    auto *movRbp = emitMov (Register (RBP), Register (RSP));

    pushRbp->SetNext (movRbp);
    movRbp->SetPrev (pushRbp);

    if (entryBB->Start () != nullptr) {
        movRbp->SetNext (entryBB->Start ());
        entryBB->Start ()->SetPrev (movRbp);
    }
    entryBB->SetStart (pushRbp);

    if (stackSize > 0) {
        // subq $stackSize, %rsp
        auto *subRsp = emitSub (Register (RSP), stackSize);

        subRsp->SetNext (movRbp->Next ());
        if (movRbp->Next () != nullptr) {
            movRbp->Next ()->SetPrev (subRsp);
        }
        movRbp->SetNext (subRsp);
        subRsp->SetPrev (movRbp);
    }

    for (auto *mbb = func->BlockStart (); mbb != nullptr; mbb = mbb->Next ()) {
        for (auto *inst = mbb->Start (); inst != nullptr; inst = inst->Next ()) {
            processInst (mbb, inst, stackSize);
        }
    }
}

void
X86PEInserter::processInst (
    MachineBasicBlock *mbb, MachineInst *inst, uint32_t stackSize) {
    if (inst->Opcode () == RET) {
        processRetInst (mbb, inst, stackSize);
    }
}

void
X86PEInserter::processRetInst (
    MachineBasicBlock *mbb, MachineInst *inst, uint32_t stackSize) {
    // popq %rbp
    auto *popRbp = emitPop (Register (RBP));

    auto *prevInst = inst->Prev ();
    if (prevInst != nullptr) {
        prevInst->SetNext (popRbp);
        popRbp->SetPrev (prevInst);
    } else {
        mbb->SetStart (popRbp);
    }
    popRbp->SetNext (inst);
    inst->SetPrev (popRbp);

    if (stackSize > 0) {
        // movq %rbp, %rsp
        auto *restoreRsp = emitMov (Register (RSP), Register (RBP));

        auto *pPrev = popRbp->Prev ();
        if (pPrev != nullptr) {
            pPrev->SetNext (restoreRsp);
            restoreRsp->SetPrev (pPrev);
        } else {
            mbb->SetStart (restoreRsp);
        }
        restoreRsp->SetNext (popRbp);
        popRbp->SetPrev (restoreRsp);
    }
}

MachineInst *
X86PEInserter::emitMov (Register dst, Register src) {
    auto *movMI = _mctx.Allocator ().Alloc<MachineInst> ();
    ::new (movMI) MachineInst (MOV64rr);
    movMI->AddOperand (MachineOperand::CreateReg (dst, true));
    movMI->AddOperand (MachineOperand::CreateReg (src, false));
    return movMI;
}

MachineInst *
X86PEInserter::emitPush (Register reg) {
    auto *pushMI = _mctx.Allocator ().Alloc<MachineInst> ();
    ::new (pushMI) MachineInst (PUSH64r);
    pushMI->AddOperand (MachineOperand::CreateReg (reg));
    return pushMI;
}

MachineInst *
X86PEInserter::emitPop (Register dst) {
    auto *popMI = _mctx.Allocator ().Alloc<MachineInst> ();
    ::new (popMI) MachineInst (POP64r);
    popMI->AddOperand (MachineOperand::CreateReg (dst));
    return popMI;
}

MachineInst *
X86PEInserter::emitSub (Register dst, int64_t imm) {
    auto *subMI = _mctx.Allocator ().Alloc<MachineInst> ();
    ::new (subMI) MachineInst (SUB64ri);
    subMI->AddOperand (MachineOperand::CreateReg (dst, true));
    subMI->AddOperand (MachineOperand::CreateImm (imm));
    return subMI;
}

}
