#include "anvil/target/x86/x86_liveness_analysis.h"

namespace anvil::x86 {

void
X86LivenessAnalysis::analyseFunction (MachineFunction *func) {
    _intervals.Clear ();
    _instToSlot.Clear ();
    size_t currentSlot = 0;

    for (auto *mbb = func->BlockStart (); mbb != nullptr; mbb = mbb->Next ()) {
        for (auto *inst = mbb->Start (); inst != nullptr; inst = inst->Next ()) {
            _instToSlot.Insert (inst, currentSlot);
            currentSlot += 4;
        }
    }

    // TODO: implement CFG building
    for (auto *mbb = func->BlockEnd (); mbb != nullptr; mbb = mbb->Prev ()) {
        _liveRegs.Clear ();
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
X86LivenessAnalysis::analyseInst (MachineInst *inst) {
    size_t *slotPtr = _instToSlot.Find (inst);
    if (slotPtr == nullptr) {
        return;
    }
    size_t idx = *slotPtr;

    for (size_t i = 0; i < inst->NumOperands (); ++i) {
        auto &op = inst->Operand (i);
        if (op.IsReg () && op.IsDef ()) {
            defReg (op, idx);
        }
    }

    for (size_t i = 0; i < inst->NumOperands (); ++i) {
        auto &op = inst->Operand (i);
        if (op.IsReg () && op.IsUse ()) {
            useReg (op, idx);
        }
    }
}

void
X86LivenessAnalysis::useReg (MachineOperand &op, size_t useSlot) {
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
X86LivenessAnalysis::defReg (MachineOperand &op, size_t defSlot) {
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

}
