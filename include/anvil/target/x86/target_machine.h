#pragma once
#include "anvil/target/target_machine.h"

namespace anvil {

class X86TargetMachine : public TargetMachine {
public:
    X86TargetMachine (Triple triple, DataLayout dl) : TargetMachine (triple, dl) {}

    void
    SelectInstructions (Module &inMod, MachineModule &outMod) override {
        // TODO: implement
    }
};

}
