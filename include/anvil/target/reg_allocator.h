#pragma once
#include "anvil/collections/hash_map.h"
#include "anvil/collections/small_vector.h"
#include <unordered_set>

namespace anvil {

class MachineContext;
class MachineFunction;
class MachineModule;
class MachineOperand;
class Register;

class RegisterAllocator {
protected:
    std::unordered_set<uint8_t> _availableRegs;
    HashMap<uint32_t, uint8_t>  _vregId2Physreg;
    HashMap<uint32_t, int>      _vregId2StackSlot;
    SmallVector<int, 2>         _emergencySlots;
    MachineContext             &_mctx;
    MachineFunction            *_curFunc{};

public:
    explicit RegisterAllocator (MachineContext &mctx) : _mctx (mctx) {}

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
