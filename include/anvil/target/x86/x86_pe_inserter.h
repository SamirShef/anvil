#pragma once
#include "anvil/target/machine_function.h"
#include "anvil/target/machine_module.h"
#include "anvil/target/pe_inserter.h"

namespace anvil::x86 {

class X86PEInserter : public PEInserter {
public:
    explicit X86PEInserter (MachineContext &mctx) : PEInserter (mctx) {}

    void
    InsertPrologEpilog (MachineModule &mmod, MachineContext &mctx) override {
        for (auto *func = mmod.FuncsStart (); func != nullptr; func = func->Next ()) {
            processFunction (func, mctx);
        }
    }

private:
    void
    processFunction (MachineFunction *func, MachineContext &mctx);

    void
    processInst (MachineBasicBlock *mbb, MachineInst *inst, uint32_t stackSize);

    void
    processRetInst (MachineBasicBlock *mbb, MachineInst *inst, uint32_t stackSize);

    MachineInst *
    emitMov (Register dst, Register src);

    MachineInst *
    emitPush (Register reg);

    MachineInst *
    emitPop (Register dst);

    MachineInst *
    emitSub (Register dst, int64_t imm);
};

}
