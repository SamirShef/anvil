#pragma once
#include "anvil/collections/hash_map.h"
#include "anvil/collections/small_vector.h"
#include "anvil/target/liveness_analysis.h"

namespace anvil {

class MachineOperand;
class MachineModule;
class Register;

class RegisterCoalescer {
protected:
    HashMap<Register, SmallVector<MachineOperand *, 4>> _map;
    LivenessAnalysis                                   &_liveness;

public:
    explicit RegisterCoalescer (LivenessAnalysis &liveness) : _liveness (liveness) {}

    RegisterCoalescer (const RegisterCoalescer &) = delete;
    RegisterCoalescer (RegisterCoalescer &&)      = default;
    RegisterCoalescer &
    operator= (const RegisterCoalescer &) = delete;
    RegisterCoalescer &
    operator= (RegisterCoalescer &&) = delete;
    virtual ~RegisterCoalescer ()    = default;

    virtual void
    CoalesceModule (MachineModule &mmod) = 0;
};

}
