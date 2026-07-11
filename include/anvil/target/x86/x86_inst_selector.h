#pragma once
#include "anvil/core/module.h"
#include "anvil/target/inst_selector.h"
#include "anvil/target/machine_module.h"

namespace anvil::x86 {

class X86InstructionSelector : public InstructionSelector {
    MachineContext            &_mctx;
    MachineBasicBlock         *_currentMBB{};
    HashMap<Value *, uint32_t> _vregMap;

public:
    explicit X86InstructionSelector (MachineContext &mctx) : _mctx (mctx) {}

    void
    SelectInstructions (Module &inMod, MachineModule &outMod) override;

private:
    uint32_t
    getOrCreateVReg (Value *v);

    void
    selectFunction (const Function &srcFunc, MachineFunction &destFunc);

    void
    selectInstruction (Inst &inst);

    MachineInst *
    emitMov (Register dst, Register src);

    MachineInst *
    emitMov (Register dst, int64_t imm);

    MachineInst *
    emitAdd (Register dst, Register src);

    MachineInst *
    emitSub (Register dst, Register src);

    MachineInst *
    emitRet (Register valReg);
};

}
