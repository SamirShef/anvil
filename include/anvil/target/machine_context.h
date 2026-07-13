#pragma once
#include "anvil/support/arena.h"

namespace anvil {

class MachineContext {
    ArenaAllocator _arena;

public:
    MachineContext ()                       = default;
    MachineContext (const MachineContext &) = delete;
    MachineContext (MachineContext &&)      = default;
    MachineContext &
    operator= (const MachineContext &) = delete;
    MachineContext &
    operator= (MachineContext &&) = default;
    ~MachineContext ()            = default;

    ArenaAllocator &
    Allocator () {
        return _arena;
    }
};

}
