#pragma once
#include "anvil/target/machine_module.h"
#include "anvil/target/target_machine.h"
#include "anvil/target/x86/x86_inst_selector.h"

namespace anvil::x86 {

class X86TargetMachine : public TargetMachine {
public:
    X86TargetMachine (Triple triple, DataLayout dl) : TargetMachine (triple, dl) {}

    void
    SelectInstructions (Module &inMod, MachineModule &outMod) override {
        auto                       &mctx = outMod.Context ();
        x86::X86InstructionSelector selector (mctx);
        selector.SelectInstructions (inMod, outMod);
    }
};

}
