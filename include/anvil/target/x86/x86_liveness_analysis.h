#pragma once
#include "anvil/target/liveness_analysis.h"
#include "anvil/target/machine_inst.h"
#include "anvil/target/machine_module.h"
#include "anvil/target/machine_operand.h"
#include "anvil/target/x86/x86_inst_info.h"

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
                analyzeInst (inst);
            }
        }
    }

    void
    analyzeInst (MachineInst *inst) {
        switch ((x86::OpCode) inst->Opcode ()) {
        case MOV64ri: {
            defReg (inst->Operand (0));
            break;
        }
        case MOV64rr: {
            defReg (inst->Operand (0));
            useReg (inst->Operand (1));
            break;
        }
        case ADD64rr:
        case SUB64rr: {
            defReg (inst->Operand (0));
            useReg (inst->Operand (0));
            useReg (inst->Operand (1));
            break;
        }
        case RET: {
            useReg (inst->Operand (0));
            break;
        }
        }
    }

    void
    useReg (MachineOperand &op) {
        if (_liveRegs.Contains (op)) {
            return;
        }
        _liveRegs.PushBack (op);
        op.SetKill (true);
    }

    void
    defReg (MachineOperand &op) {
        _liveRegs.Remove (op);
    }
};

}
