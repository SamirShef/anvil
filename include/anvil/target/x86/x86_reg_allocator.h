#pragma once
#include "anvil/target/machine_function.h"
#include "anvil/target/machine_inst.h"
#include "anvil/target/machine_module.h"
#include "anvil/target/reg_allocator.h"
#include "anvil/target/register.h"
#include "anvil/target/x86/x86_inst_info.h"
#include "anvil/target/x86/x86_register_info.h"

namespace anvil::x86 {

class X86RegisterAllocator : public RegisterAllocator {
public:
    X86RegisterAllocator () {
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
        for (auto *mbb = func->BlockStart (); mbb != nullptr; mbb = mbb->Next ()) {
            for (auto *inst = mbb->Start (); inst != nullptr; inst = inst->Next ()) {
                allocInst (inst);
            }
        }
    }

    void
    allocInst (MachineInst *inst) {
        switch ((x86::OpCode) inst->Opcode ()) {
        case MOV64ri: {
            allocReg (inst->Operand (0));
            break;
        }
        case MOV64rr:
        case ADD64rr:
        case SUB64rr: {
            allocReg (inst->Operand (1)); // src
            allocReg (inst->Operand (0)); // dst
            break;
        }
        case RET: {
            allocReg (inst->Operand (0)); // rax
            break;
        }
        }
    }

    void
    allocReg (MachineOperand &op) {
        if (!op.IsReg ()) {
            return;
        }
        auto oldReg = op.AsReg ();
        if (!oldReg.IsVirtual ()) {
            return;
        }
        if (auto *found = _vregId2Physreg.Find (oldReg.Id ())) {
            op = MachineOperand::CreateReg (Register (*found), op.IsDef (), op.IsKill ());
            if (op.IsKill ()) {
                _availableRegs.insert (*found);
                _vregId2Physreg.Remove (oldReg.Id ());
            }
            return;
        }
        auto     physRegId = *_availableRegs.begin ();
        Register physReg (physRegId);
        _vregId2Physreg.Insert (oldReg.Id (), physRegId);
        _availableRegs.erase (physRegId);
        op = MachineOperand::CreateReg (physReg, op.IsDef (), op.IsKill ());
        if (op.IsKill ()) {
            _availableRegs.insert (physRegId);
            _vregId2Physreg.Remove (oldReg.Id ());
        }
    }
};

}
