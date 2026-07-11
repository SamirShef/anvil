#include "anvil/target/x86/x86_inst_selector.h"
#include "anvil/target/x86/x86_inst_info.h"
#include "anvil/target/x86/x86_register_info.h"

namespace anvil::x86 {

void
X86InstructionSelector::SelectInstructions (Module &inMod, MachineModule &outMod) {
    for (auto *srcFunc = inMod.FuncsStart (); srcFunc != nullptr;
         srcFunc       = srcFunc->Next ()) {
        if (srcFunc->IsDeclaration ()) {
            continue;
        }

        auto *mf = _mctx.Allocator ().Alloc<MachineFunction> ();
        ::new (mf) MachineFunction (srcFunc->GetName ());

        selectFunction (*srcFunc, *mf);

        outMod.AddFunction (mf);
    }
}

uint32_t
X86InstructionSelector::getOrCreateVReg (Value *v) {
    if (_vregMap.Find (v) == nullptr) {
        auto vreg = CreateVReg ();
        _vregMap.Insert (v, vreg);
        if (auto *c = DynCast<ConstantInt> (v)) {
            emitMov (Register (vreg), static_cast<int64_t> (c->Val ()));
        }
    }
    return *_vregMap.Find (v);
}

void
X86InstructionSelector::selectFunction (
    const Function &srcFunc, MachineFunction &destFunc) {
    for (auto *srcBB = srcFunc.Start (); srcBB != nullptr; srcBB = srcBB->Next ()) {
        _currentMBB = destFunc.CreateBasicBlock (_mctx, srcBB->GetName ());
        destFunc.AddBasicBlock (_currentMBB);

        for (auto *srcInst = srcBB->Start (); srcInst != nullptr;
             srcInst       = srcInst->Next ()) {
            selectInstruction (*srcInst);
        }
    }
}

void
X86InstructionSelector::selectInstruction (Inst &inst) {
    switch (inst.Opcode ()) {
    case Inst::OpCode::Add: {
        uint32_t dstVReg = getOrCreateVReg (&inst);
        uint32_t lhsVReg = getOrCreateVReg (inst.Operand (0));
        uint32_t rhsVReg = getOrCreateVReg (inst.Operand (1));
        emitMov (Register (dstVReg), Register (lhsVReg));
        emitAdd (Register (dstVReg), Register (rhsVReg));
        break;
    }
    case Inst::OpCode::Sub: {
        uint32_t dstVReg = getOrCreateVReg (&inst);
        uint32_t lhsVReg = getOrCreateVReg (inst.Operand (0));
        uint32_t rhsVReg = getOrCreateVReg (inst.Operand (1));
        emitMov (Register (dstVReg), Register (lhsVReg));
        emitSub (Register (dstVReg), Register (rhsVReg));
        break;
    }
    case Inst::OpCode::Ret: {
        uint32_t valVReg = getOrCreateVReg (inst.Operand (0));
        emitMov (Register (RAX), Register (valVReg));
        emitRet (Register (RAX));
        break;
    }
    default:
        break;
    }
}

MachineInst *
X86InstructionSelector::emitMov (Register dst, Register src) {
    auto *movMI = _mctx.Allocator ().Alloc<MachineInst> ();
    ::new (movMI) MachineInst (x86::MOV64rr);
    movMI->AddOperand (MachineOperand::CreateReg (dst, true));
    movMI->AddOperand (MachineOperand::CreateReg (src, false));
    _currentMBB->Emit (movMI);
    return movMI;
}

MachineInst *
X86InstructionSelector::emitMov (Register dst, int64_t imm) {
    auto *movMI = _mctx.Allocator ().Alloc<MachineInst> ();
    ::new (movMI) MachineInst (x86::MOV64rr);
    movMI->AddOperand (MachineOperand::CreateReg (dst, true));
    movMI->AddOperand (MachineOperand::CreateImm (imm));
    _currentMBB->Emit (movMI);
    return movMI;
}

MachineInst *
X86InstructionSelector::emitAdd (Register dst, Register src) {
    auto *addMI = _mctx.Allocator ().Alloc<MachineInst> ();
    ::new (addMI) MachineInst (x86::ADD64rr);
    addMI->AddOperand (MachineOperand::CreateReg (dst, true));
    addMI->AddOperand (MachineOperand::CreateReg (src, false));
    _currentMBB->Emit (addMI);
    return addMI;
}

MachineInst *
X86InstructionSelector::emitSub (Register dst, Register src) {
    auto *subMI = _mctx.Allocator ().Alloc<MachineInst> ();
    ::new (subMI) MachineInst (x86::SUB64rr);
    subMI->AddOperand (MachineOperand::CreateReg (dst, true));
    subMI->AddOperand (MachineOperand::CreateReg (src, false));
    _currentMBB->Emit (subMI);
    return subMI;
}

MachineInst *
X86InstructionSelector::emitRet (Register valReg) {
    auto *retMI = _mctx.Allocator ().Alloc<MachineInst> ();
    ::new (retMI) MachineInst (x86::RET);
    retMI->AddOperand (MachineOperand::CreateReg (Register (RAX)));
    _currentMBB->Emit (retMI);
    return retMI;
}

}
