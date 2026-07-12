#pragma once
#include "anvil/target/liveness_analysis.h"
#include "anvil/target/machine_function.h"
#include "anvil/target/machine_inst.h"
#include "anvil/target/machine_module.h"
#include "anvil/target/reg_allocator.h"
#include "anvil/target/register.h"
#include "anvil/target/x86/x86_register_info.h"
#include <algorithm>
#include <cstdlib>

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
    allocFunction (MachineFunction *func) {
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
                    const std::pair<Register, size_t> &b) {
                    return a.second < b.second;
                });
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
    allocInst (MachineInst *inst);

    void
    allocReg (MachineOperand &op, MachineInst *inst);
};

}
