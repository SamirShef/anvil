#pragma once
#include "anvil/target/liveness_analysis.h"
#include "anvil/target/machine_function.h"
#include "anvil/target/machine_inst.h"
#include "anvil/target/machine_module.h"
#include "anvil/target/reg_allocator.h"
#include "anvil/target/x86/x86_register_info.h"

namespace anvil::x86 {

class X86RegisterAllocator : public RegisterAllocator {
public:
    X86RegisterAllocator (MachineContext &mctx, LivenessAnalysis &liveness)
        : RegisterAllocator (mctx, liveness) {
        _availableRegs = { RAX, RCX, RDX, RSI, RDI, R8, R9, R10, R11 };
    }

    void
    AllocModule (MachineModule &mmod) override {
        for (auto *func = mmod.FuncsStart (); func != nullptr; func = func->Next ()) {
            allocFunction (func);
        }
    }

private:
    void
    allocFunction (MachineFunction *func);

    void
    allocInst (MachineInst *inst);

    void
    allocReg (MachineOperand &op, MachineInst *inst);
};

}
