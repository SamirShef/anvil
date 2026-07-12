#include "anvil/target/x86/x86_liveness_analysis.h"

namespace anvil::x86 {

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
            defReg (op, idx + 2);
        }
    }

    for (size_t i = 0; i < inst->NumOperands (); ++i) {
        auto &op = inst->Operand (i);
        if (op.IsReg () && op.IsUse ()) {
            useReg (op, idx + 1);
        }
    }
}

}
