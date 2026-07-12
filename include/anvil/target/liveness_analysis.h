#pragma once
#include "anvil/collections/hash_map.h"
#include "anvil/collections/small_vector.h"
#include "anvil/target/live_interval.h"
#include "anvil/target/register.h"

namespace anvil {

class MachineModule;
class MachineOperand;

class LivenessAnalysis {
protected:
    SmallVector<Register, 8>        _liveRegs;
    HashMap<Register, LiveInterval> _intervals;

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

    const LiveInterval *
    GetInterval (Register reg) const {
        return _intervals.Find (reg);
    }

    HashMap<Register, LiveInterval> &
    Intervals () {
        return _intervals;
    }
};

}
