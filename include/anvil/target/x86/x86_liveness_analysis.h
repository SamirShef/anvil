#pragma once
#include "anvil/target/liveness_analysis.h"
#include "anvil/target/machine_module.h"

namespace anvil::x86 {

class X86LivenessAnalysis : public LivenessAnalysis {
    HashMap<MachineInst *, size_t> _instToSlot;

public:
    void
    AnalyseModule (MachineModule &mmod) override {
        for (auto *func = mmod.FuncsStart (); func != nullptr; func = func->Next ()) {
            analyseFunction (func);
        }
    }

private:
    void
    analyseFunction (MachineFunction *func);

    void
    analyseInst (MachineInst *inst);

    void
    useReg (MachineOperand &op, size_t useSlot);

    void
    defReg (MachineOperand &op, size_t defSlot);
};

}
