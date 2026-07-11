#pragma once
#include "anvil/target/machine_function.h"
#include "anvil/target/machine_inst.h"
#include "anvil/target/machine_module.h"
#include "anvil/target/register.h"
#include "anvil/target/register_coalescer.h"
#include "anvil/target/x86/x86_inst_info.h"

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
    coalesceInst (MachineInst *inst) {
        switch ((x86::OpCode) inst->Opcode ()) {
        case MOV64rr: {
            auto  dst       = inst->Operand (0);
            auto  src       = inst->Operand (1);
            auto *foundUses = _map.Find (src.AsReg ());
            if (!src.IsKill () || foundUses == nullptr) {
                addRegIfNeed (dst.AsReg (), &inst->Operand (0));
                addRegIfNeed (src.AsReg (), &inst->Operand (1));
                return;
            }
            auto uses = *foundUses;
            _map.Remove (src.AsReg ());
            for (size_t i = 0; i < uses.Size (); ++i) {
                auto &use = uses[i];
                *use      = MachineOperand::CreateReg (
                    dst.AsReg (),
                    use->IsDef (),
                    use->IsKill ());
                addRegIfNeed (dst.AsReg (), use);
            }

            inst->Operand (1) = MachineOperand::CreateReg (
                dst.AsReg (),
                false, // use
                src.IsKill ());

            addRegIfNeed (dst.AsReg (), &inst->Operand (0));
            addRegIfNeed (dst.AsReg (), &inst->Operand (1));
            break;
        }
        case ADD64rr:
        case SUB64rr: {
            auto &op0 = inst->Operand (0);
            addRegIfNeed (op0.AsReg (), &op0);
            auto &op1 = inst->Operand (1);
            addRegIfNeed (op1.AsReg (), &op1);
            break;
        }
        case MOV64ri:
        case RET: {
            auto &op = inst->Operand (0);
            addRegIfNeed (op.AsReg (), &op);
            break;
        }
        }
    }

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
