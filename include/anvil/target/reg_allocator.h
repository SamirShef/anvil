#pragma once
#include "anvil/collections/hash_map.h"
#include "anvil/collections/small_vector.h"
#include "anvil/target/live_interval.h"
#include "anvil/target/liveness_analysis.h"
#include "anvil/target/machine_function.h"
#include "anvil/target/register.h"
#include <utility>

namespace anvil {

class MachineContext;
class MachineFunction;
class MachineModule;
class MachineOperand;
class Register;

using VRegId    = uint32_t;
using PhysRegId = uint32_t;

class RegisterAllocator {
protected:
    SmallVector<PhysRegId, 14> _availableRegs;
    HashMap<VRegId, PhysRegId> _vreg2Phys;
    HashMap<VRegId, StackSlot> _vreg2Stack;
    // TODO: rewrite _activeRegs and _unhandledRegs declarations
    SmallVector<std::pair<Register, LiveInterval>, 14> _activeRegs;
    SmallVector<std::pair<VRegId, LiveInterval>, 8>    _unhandledRegs;
    MachineContext                                    &_mctx;
    LivenessAnalysis                                  &_liveness;

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
