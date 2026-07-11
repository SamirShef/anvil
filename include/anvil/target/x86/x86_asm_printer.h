#pragma once
#include "anvil/streams/raw_ostream.h"
#include "anvil/target/asm_printer.h"
#include "anvil/target/machine_inst.h"
#include "anvil/target/machine_module.h"
#include "anvil/target/machine_operand.h"
#include "anvil/target/x86/x86_inst_info.h"
#include "anvil/target/x86/x86_register_info.h"

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
    emitFunction (MachineFunction *func) {
        const auto &name = func->Name ();
        _os << "    .text\n";
        _os << "    .globl " << name << '\n';
        _os << "    .type " << name << ", @function\n";
        _os << name << ":\n";
        for (auto *mbb = func->BlockStart (); mbb != nullptr; mbb = mbb->Next ()) {
            if (mbb != func->BlockStart () || !mbb->Name ().empty ()) {
                _os << '.' << name << '_' << mbb->Name () << ":\n";
            }
            for (auto *mi = mbb->Start (); mi != nullptr; mi = mi->Next ()) {
                emitInst (mi);
            }
        }
        _os << "    .size    " << name << ", .-" << name << '\n';
        _os << "    .section .note.GNU-stack,\"\",@progbits\n";
    }

    void
    emitInst (MachineInst *inst) {
        switch ((x86::OpCode) inst->Opcode ()) {
        case x86::OpCode::MOV64ri:
        case x86::OpCode::MOV64rr: {
            if (inst->Operand (0).AsReg () == inst->Operand (1).AsReg ()) {
                return;
            }
            _os << "    movq     ";
            printOperand (inst->Operand (1)); // src
            _os << ", ";
            printOperand (inst->Operand (0)); // dst
            _os << '\n';
            break;
        }
        case ADD64rr: {
            _os << "    addq     ";
            printOperand (inst->Operand (1)); // src
            _os << ", ";
            printOperand (inst->Operand (0)); // dst
            _os << '\n';
            break;
        }
        case SUB64rr: {
            _os << "    subq     ";
            printOperand (inst->Operand (1)); // src
            _os << ", ";
            printOperand (inst->Operand (0)); // dst
            _os << '\n';
            break;
        }
        case RET: {
            _os << "    ret\n";
            break;
        }
        }
    }

    void
    printOperand (MachineOperand operand) {
        switch (operand.GetKind ()) {
        case MachineOperand::RegOp: {
            auto reg = operand.AsReg ();
            if (reg.IsNone ()) {
                _os << "%noreg";
            } else if (reg.IsPhysical ()) {
                _os << "%" << physicalRegName (reg.Id ());
            } else if (reg.IsVirtual ()) {
                _os << "%vreg" << reg.Id () - 1024;
            }
            break;
        }
        case MachineOperand::ImmOp: {
            _os << "$" << operand.AsImm ();
            break;
        }
        case MachineOperand::MBBOp: {
            auto *mbb = operand.AsMBB ();
            _os << '.' << mbb->Parent ()->Name () << '_' << mbb->Name ();
            break;
        }
        case MachineOperand::FrameIndexOp: {
            _os << "[stack_slot_" << operand.AsFI () << ']';
            break;
        }
        }
    }

    static const char *
    physicalRegName (uint32_t id) {
        switch ((x86::RegInfo) id) {
        case RAX:
            return "rax";
        case RCX:
            return "rcx";
        case RDX:
            return "rdx";
        case RSI:
            return "rsi";
        case RDI:
            return "rdi";
        case R8:
            return "r8";
        case R9:
            return "r9";
        case R10:
            return "r10";
        case R11:
            return "r11";
        }
        return "nophysreg";
    }
};

}
