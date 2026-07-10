#pragma once
#include "anvil/collections/hash_map.h"
#include "anvil/core/module.h"
#include "anvil/ir/inst.h"
#include "anvil/target/inst_selector.h"
#include "anvil/target/machine_context.h"
#include "anvil/target/machine_module.h"
#include "anvil/target/register.h"
#include "anvil/target/x86/x86_inst_info.h"

namespace anvil::x86 {

class X86InstructionSelector : public InstructionSelector {
    MachineContext            &_mctx;
    MachineBasicBlock         *_currentMBB{};
    HashMap<Value *, uint32_t> _vregMap;

public:
    explicit X86InstructionSelector (MachineContext &mctx) : _mctx (mctx) {}

    void
    SelectInstructions (Module &inMod, MachineModule &outMod) override {
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

private:
    uint32_t
    getOrCreateVReg (Value *v) {
        if (_vregMap.Find (v) == nullptr) {
            auto vreg = CreateVReg ();
            _vregMap.Insert (v, vreg);
            if (auto *c = DynCast<ConstantInt> (v)) {
                auto *movImmMI = _mctx.Allocator ().Alloc<MachineInst> ();
                ::new (movImmMI) MachineInst (x86::MOV64ri);

                movImmMI->AddOperand (MachineOperand::CreateReg (Register (vreg), true));
                movImmMI->AddOperand (
                    MachineOperand::CreateImm (static_cast<int64_t> (c->Val ())));

                _currentMBB->Emit (movImmMI);
            }
        }
        return *_vregMap.Find (v);
    }

    void
    selectFunction (const Function &srcFunc, MachineFunction &destFunc) {
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
    selectInstruction (Inst &inst) {
        switch (inst.Opcode ()) {
        case Inst::OpCode::Add: {
            uint32_t dstVReg = getOrCreateVReg (&inst);
            uint32_t lhsVReg = getOrCreateVReg (inst.Operand (0));
            uint32_t rhsVReg = getOrCreateVReg (inst.Operand (1));

            auto *movMI = _mctx.Allocator ().Alloc<MachineInst> ();
            ::new (movMI) MachineInst (x86::MOV64rr);
            movMI->AddOperand (MachineOperand::CreateReg (Register (dstVReg), true));
            movMI->AddOperand (MachineOperand::CreateReg (Register (lhsVReg), false));
            _currentMBB->Emit (movMI);

            auto *addMI = _mctx.Allocator ().Alloc<MachineInst> ();
            ::new (addMI) MachineInst (x86::ADD64rr);
            addMI->AddOperand (MachineOperand::CreateReg (Register (dstVReg), true));
            addMI->AddOperand (MachineOperand::CreateReg (Register (rhsVReg), false));
            _currentMBB->Emit (addMI);
            break;
        }
        case Inst::OpCode::Ret: {
            auto *retMI = _mctx.Allocator ().Alloc<MachineInst> ();
            ::new (retMI) MachineInst (x86::RET);
            _currentMBB->Emit (retMI);
            break;
        }
        default:
            break;
        }
    }
};

}
