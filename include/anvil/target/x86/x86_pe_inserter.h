#pragma once
#include "anvil/target/machine_function.h"
#include "anvil/target/machine_module.h"
#include "anvil/target/pe_inserter.h"
#include "anvil/target/x86/x86_inst_info.h"
#include "anvil/target/x86/x86_register_info.h"

namespace anvil::x86 {

class X86PEInserter : public PEInserter {
public:
    X86PEInserter () = default;

    void
    InsertPrologEpilog (MachineModule &mmod, MachineContext &mctx) override {
        for (auto *func = mmod.FuncsStart (); func != nullptr; func = func->Next ()) {
            processFunction (func, mctx);
        }
    }

private:
    void
    processFunction (MachineFunction *func, MachineContext &mctx) {
        auto *entryBB = func->BlockStart ();
        if (entryBB == nullptr) {
            return;
        }

        size_t numSlots = func->NumStackSlots ();

        // System V ABI: align stack by 16 bytes before calls
        uint32_t stackSize = ((numSlots * 8) + 15) & ~15;

        // pushq %rbp
        auto *pushRbp = mctx.Allocator ().Alloc<MachineInst> ();
        ::new (pushRbp) MachineInst (x86::PUSH64r);
        pushRbp->AddOperand (MachineOperand::CreateReg (Register (RBP)));

        // movq %rsp, %rbp
        auto *movRbp = mctx.Allocator ().Alloc<MachineInst> ();
        ::new (movRbp) MachineInst (x86::MOV64rr);
        movRbp->AddOperand (MachineOperand::CreateReg (Register (RBP), true));
        movRbp->AddOperand (MachineOperand::CreateReg (Register (RSP), false));

        pushRbp->SetNext (movRbp);
        movRbp->SetPrev (pushRbp);

        if (entryBB->Start () != nullptr) {
            movRbp->SetNext (entryBB->Start ());
            entryBB->Start ()->SetPrev (movRbp);
        }
        entryBB->SetStart (pushRbp);

        if (stackSize > 0) {
            // subq $stackSize, %rsp
            auto *subRsp = mctx.Allocator ().Alloc<MachineInst> ();
            ::new (subRsp) MachineInst (x86::SUB64ri);
            subRsp->AddOperand (MachineOperand::CreateReg (Register (RSP), true));
            subRsp->AddOperand (MachineOperand::CreateImm (stackSize));

            subRsp->SetNext (movRbp->Next ());
            if (movRbp->Next () != nullptr) {
                movRbp->Next ()->SetPrev (subRsp);
            }
            movRbp->SetNext (subRsp);
            subRsp->SetPrev (movRbp);
        }

        for (auto *mbb = func->BlockStart (); mbb != nullptr; mbb = mbb->Next ()) {
            for (auto *inst = mbb->Start (); inst != nullptr; inst = inst->Next ()) {
                if (inst->Opcode () == x86::RET) {
                    // popq %rbp
                    auto *popRbp = mctx.Allocator ().Alloc<MachineInst> ();
                    ::new (popRbp) MachineInst (x86::POP64r);
                    popRbp->AddOperand (MachineOperand::CreateReg (Register (RBP)));

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
                        auto *restoreRsp = mctx.Allocator ().Alloc<MachineInst> ();
                        ::new (restoreRsp) MachineInst (x86::MOV64rr);
                        restoreRsp->AddOperand (
                            MachineOperand::CreateReg (Register (RSP), true));
                        restoreRsp->AddOperand (
                            MachineOperand::CreateReg (Register (RBP), false));

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
            }
        }
    }
};

}
