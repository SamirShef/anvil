#pragma once
#include "anvil/target/machine_basic_block.h"
#include "anvil/target/machine_context.h"

namespace anvil {

class MachineFunction {
    friend class MachineModule;
    std::string_view   _name;
    MachineBasicBlock *_blocksStart{};
    MachineBasicBlock *_blocksEnd{};
    int                _nextFrameIndex = 0;
    MachineFunction   *_prev{};
    MachineFunction   *_next{};

public:
    explicit MachineFunction (std::string_view name) : _name (name) {}

    std::string_view
    Name () const {
        return _name;
    }

    MachineBasicBlock *
    CreateBasicBlock (MachineContext &ctx, std::string_view name) {
        auto *mbb = ctx.Allocator ().Alloc<MachineBasicBlock> ();
        ::new (mbb) MachineBasicBlock (name, this);
        return mbb;
    }

    void
    AddBasicBlock (MachineBasicBlock *mbb) {
        if (_blocksEnd == nullptr) {
            _blocksEnd = _blocksStart = mbb;
        } else {
            _blocksEnd->_next = mbb;
            mbb->_prev        = _blocksEnd;
            _blocksEnd        = mbb;
        }
    }

    int
    CreateStackSlot () {
        return _nextFrameIndex++;
    }

    int
    NumStackSlots () const {
        return _nextFrameIndex;
    }

    MachineBasicBlock *
    BlockStart () const {
        return _blocksStart;
    }

    MachineBasicBlock *
    BlockEnd () const {
        return _blocksEnd;
    }

    MachineFunction *
    Prev () const {
        return _prev;
    }

    MachineFunction *
    Next () const {
        return _next;
    }
};

}
