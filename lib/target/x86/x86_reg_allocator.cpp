#include "anvil/target/x86/x86_reg_allocator.h"

namespace anvil::x86 {

void
X86RegisterAllocator::allocFunction (MachineFunction *func) {
    _curFunc = func;
    _vregId2Physreg.Clear ();
    _actualRegs.clear ();
    _unhandledRegs.clear ();

    auto &intervals = _liveness.Intervals ();
    for (const auto &[reg, interval] : intervals) {
        if (reg.IsVirtual ()) {
            _unhandledRegs.emplace_back (const_cast<Register *> (&reg), interval);
        }
    }
    std::ranges::sort (
        _unhandledRegs,
        [] (const std::pair<Register *, LiveInterval> &a,
            const std::pair<Register *, LiveInterval> &b) {
            return a.second.Start < b.second.Start;
        });

    for (const auto &[cur, interval] : _unhandledRegs) {
        for (size_t j = 0; j < _actualRegs.size ();) {
            auto &[reg, end] = _actualRegs[j];
            if (end <= interval.Start) {
                _availableRegs.insert (reg.Id ());
                _actualRegs.erase (_actualRegs.begin () + (long) j);
            } else {
                ++j;
            }
        }
        std::ranges::sort (
            _actualRegs,
            [] (const std::pair<Register, size_t> &a,
                const std::pair<Register, size_t> &b) { return a.second < b.second; });
        if (auto *found = _vregId2Physreg.Find (cur->Id ())) {
            continue;
        }
        if (!_availableRegs.empty ()) {
            auto regId = *_availableRegs.begin ();
            _availableRegs.erase (regId);
            _actualRegs.emplace_back (Register (regId), interval.End);
            _vregId2Physreg.Insert (cur->Id (), regId);
        } else {
            abort ();
        }
    }

    for (auto *mbb = func->BlockStart (); mbb != nullptr; mbb = mbb->Next ()) {
        for (auto *inst = mbb->Start (); inst != nullptr; inst = inst->Next ()) {
            allocInst (inst);
        }
    }
}

void
X86RegisterAllocator::allocInst (MachineInst *inst) {
    for (size_t i = 0; i < inst->NumOperands (); ++i) {
        auto &op = inst->Operand (i);
        if (op.IsReg ()) {
            allocReg (op, inst);
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
        return;
    }
    abort ();
}

}
