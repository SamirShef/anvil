#pragma once
#include "anvil/collections/hash_map.h"
#include <unordered_set>

namespace anvil {

class MachineModule;
class MachineOperand;
class Register;

class RegisterAllocator {
protected:
    std::unordered_set<uint8_t> _availableRegs;
    HashMap<uint32_t, uint8_t>  _vregId2Physreg;

public:
    RegisterAllocator ()                          = default;
    RegisterAllocator (const RegisterAllocator &) = default;
    RegisterAllocator (RegisterAllocator &&)      = delete;
    RegisterAllocator &
    operator= (const RegisterAllocator &) = default;
    RegisterAllocator &
    operator= (RegisterAllocator &&) = delete;
    virtual ~RegisterAllocator ()    = default;

    virtual void
    AllocModule (MachineModule &mmod) = 0;
};

}
