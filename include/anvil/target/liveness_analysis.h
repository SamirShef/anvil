#pragma once
#include "anvil/collections/small_vector.h"
#include "anvil/target/machine_operand.h"

namespace anvil {

class MachineModule;
class MachineOperand;

class LivenessAnalysis {
protected:
    SmallVector<Register, 8> _liveRegs;

public:
    LivenessAnalysis ()                         = default;
    LivenessAnalysis (const LivenessAnalysis &) = delete;
    LivenessAnalysis (LivenessAnalysis &&)      = default;
    LivenessAnalysis &
    operator= (const LivenessAnalysis &) = delete;
    LivenessAnalysis &
    operator= (LivenessAnalysis &&) = default;
    virtual ~LivenessAnalysis ()    = default;

    virtual void
    AnalyseModule (MachineModule &mmod) = 0;
};

}
