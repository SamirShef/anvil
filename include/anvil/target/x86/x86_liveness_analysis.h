#pragma once
#include "anvil/target/liveness_analysis.h"
#include "anvil/target/machine_module.h"

namespace anvil::x86 {

class X86LivenessAnalysis : public LivenessAnalysis {
public:
    void
    AnalyseModule (MachineModule &mmod) override {
        for (auto *func = mmod.FuncsStart (); func != nullptr; func = func->Next ()) {
            analyseFunction (func);
        }
    }

private:
    void
    analyseFunction (MachineFunction *func) {
        for (auto *mbb = func->BlockEnd (); mbb != nullptr; mbb = mbb->Prev ()) {
            for (auto *inst = mbb->End (); inst != nullptr; inst = inst->Prev ()) {
                analyseInst (inst);
            }
        }
    }

    void
    analyseInst (MachineInst *inst);

    void
    useReg (MachineOperand &op) {
        if (!op.IsReg ()) {
            return;
        }
        auto reg = op.AsReg ();
        if (_liveRegs.Contains (reg)) {
            return;
        }
        _liveRegs.PushBack (reg);
        op.SetKill (true);
    }

    void
    defReg (MachineOperand &op) {
        if (!op.IsReg ()) {
            return;
        }
        _liveRegs.Remove (op.AsReg ());
    }
};

}
