#pragma once
#include <cstdint>

namespace anvil {

class Module;
class MachineModule;
class MachineFunction;
class MachineBasicBlock;
class Inst;
class Value;

class InstructionSelector {
protected:
    uint32_t _nextVReg = 1024;

    uint32_t
    CreateVReg () {
        return _nextVReg++;
    }

public:
    InstructionSelector (const InstructionSelector &) = delete;
    InstructionSelector (InstructionSelector &&)      = default;
    InstructionSelector &
    operator= (const InstructionSelector &) = delete;
    InstructionSelector &
    operator= (InstructionSelector &&) = default;
    InstructionSelector ()             = default;
    virtual ~InstructionSelector ()    = default;

    virtual void
    SelectInstructions (Module &inMod, MachineModule &outMod) = 0;
};

}
