#pragma once
#include "anvil/streams/raw_ostream.h"
#include "anvil/target/asm_printer.h"
#include "anvil/target/machine_inst.h"
#include "anvil/target/machine_module.h"
#include "anvil/target/machine_operand.h"

namespace anvil::x86 {

class X86AsmPrinter : public AsmPrinter {
public:
    explicit X86AsmPrinter (RawOstream &os) : AsmPrinter (os) {}

    void
    EmitModule (MachineModule &mmod) override {
        for (auto *func = mmod.FuncsStart (); func != nullptr; func = func->Next ()) {
            emitFunction (func);
        }
    }

private:
    void
    emitFunction (MachineFunction *func);

    void
    emitInst (MachineInst *inst);

    void
    printOperand (MachineOperand operand);

    static const char *
    physicalRegName (uint32_t id);
};

}
