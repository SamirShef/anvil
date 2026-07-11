#pragma once
#include "anvil/target/machine_function.h"
#include "anvil/target/machine_inst.h"
#include "anvil/target/machine_module.h"
#include "anvil/target/register.h"
#include "anvil/target/register_coalescer.h"

namespace anvil::x86 {

class X86RegisterCoalescer : public RegisterCoalescer {
public:
    void
    CoalesceModule (MachineModule &mmod) override {
        for (auto *func = mmod.FuncsStart (); func != nullptr; func = func->Next ()) {
            coalesceFunction (func);
        }
    }

private:
    void
    coalesceFunction (MachineFunction *func) {
        for (auto *mbb = func->BlockStart (); mbb != nullptr; mbb = mbb->Next ()) {
            for (auto *inst = mbb->Start (); inst != nullptr; inst = inst->Next ()) {
                coalesceInst (inst);
            }
        }
    }

    void
    coalesceInst (MachineInst *inst);

    void
    addRegIfNeed (Register reg, MachineOperand *op) {
        if (auto *found = _map.Find (reg)) {
            found->PushBack (op);
        } else {
            _map.Insert (reg, { op });
        }
    }
};

}
