#pragma once
#include "anvil/collections/hash_map.h"
#include "anvil/collections/small_vector.h"

namespace anvil {

class MachineOperand;
class MachineModule;
class Register;

class RegisterCoalescer {
protected:
    HashMap<Register, SmallVector<MachineOperand *, 4>> _map;

public:
    RegisterCoalescer ()                          = default;
    RegisterCoalescer (const RegisterCoalescer &) = delete;
    RegisterCoalescer (RegisterCoalescer &&)      = default;
    RegisterCoalescer &
    operator= (const RegisterCoalescer &) = delete;
    RegisterCoalescer &
    operator= (RegisterCoalescer &&) = default;
    virtual ~RegisterCoalescer ()    = default;

    virtual void
    CoalesceModule (MachineModule &mmod) = 0;
};

}
