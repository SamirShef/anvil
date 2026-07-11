#include "anvil/target/x86/x86_liveness_analysis.h"
#include "anvil/target/x86/x86_inst_info.h"

namespace anvil::x86 {

void
X86LivenessAnalysis::analyzeInst (MachineInst *inst) {
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

}
