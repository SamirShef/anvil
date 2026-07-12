#pragma once
#include "anvil/collections/hash_map.h"
#include "anvil/target/live_interval.h"
#include "anvil/target/liveness_analysis.h"
#include "anvil/target/register.h"
#include <unordered_set>
#include <utility>
#include <vector>

namespace anvil {

class MachineContext;
class MachineFunction;
class MachineModule;
class MachineOperand;
class Register;

class RegisterAllocator {
protected:
    std::unordered_set<uint8_t>                      _availableRegs;
    HashMap<uint32_t, uint8_t>                       _vregId2Physreg;
    std::vector<std::pair<Register, size_t>>         _actualRegs;
    std::vector<std::pair<Register *, LiveInterval>> _unhandledRegs;
    MachineContext                                  &_mctx;
    LivenessAnalysis                                &_liveness;
    MachineFunction                                 *_curFunc{};

public:
    RegisterAllocator (MachineContext &mctx, LivenessAnalysis &liveness)
        : _mctx (mctx), _liveness (liveness) {}

    RegisterAllocator (const RegisterAllocator &) = default;
    RegisterAllocator (RegisterAllocator &&)      = delete;
    RegisterAllocator &
    operator= (const RegisterAllocator &) = delete;
    RegisterAllocator &
    operator= (RegisterAllocator &&) = delete;
    virtual ~RegisterAllocator ()    = default;

    virtual void
    AllocModule (MachineModule &mmod) = 0;
};

}
