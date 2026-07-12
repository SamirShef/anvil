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
    analyseFunction (MachineFunction *func) {
        _intervals.Clear ();
        _instToSlot.Clear ();
        size_t currentSlot = 0;

        for (auto *mbb = func->BlockStart (); mbb != nullptr; mbb = mbb->Next ()) {
            for (auto *inst = mbb->Start (); inst != nullptr; inst = inst->Next ()) {
                _instToSlot.Insert (inst, currentSlot);
                currentSlot += 4;
            }
        }

        _liveRegs.Clear ();

        for (auto *mbb = func->BlockEnd (); mbb != nullptr; mbb = mbb->Prev ()) {
            for (auto *inst = mbb->End (); inst != nullptr; inst = inst->Prev ()) {
                analyseInst (inst);
            }
        }
        for (size_t i = 0; i < _liveRegs.Size (); ++i) {
            auto  reg      = _liveRegs[i];
            auto *interval = _intervals.Find (reg);
            if (interval != nullptr) {
                interval->Define (0);
            }
        }
    }

    void
    analyseInst (MachineInst *inst);

    void
    useReg (MachineOperand &op, size_t useSlot) {
        if (!op.IsReg ()) {
            return;
        }
        auto          reg      = op.AsReg ();
        LiveInterval *interval = _intervals.Find (reg);
        if (interval == nullptr) {
            _intervals.Insert (reg, LiveInterval{});
            interval = _intervals.Find (reg);
        }
        if (_liveRegs.Contains (reg)) {
            op.SetKill (false);
            return;
        }
        interval->Use (useSlot);
        _liveRegs.PushBack (reg);
        op.SetKill (true);
    }

    void
    defReg (MachineOperand &op, size_t defSlot) {
        if (!op.IsReg ()) {
            return;
        }
        auto          reg      = op.AsReg ();
        LiveInterval *interval = _intervals.Find (reg);
        if (interval == nullptr) {
            _intervals.Insert (reg, LiveInterval{});
            interval = _intervals.Find (reg);
        }
        interval->Define (defSlot);
        _liveRegs.Remove (reg);
    }
};

}
