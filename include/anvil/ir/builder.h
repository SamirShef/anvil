#pragma once
#include "anvil/core/context.h"
#include "anvil/core/types.h"
#include "anvil/core/value.h"
#include "anvil/ir/basic_block.h"
#include "anvil/ir/binary_op.h"
#include "anvil/ir/function.h"
#include "anvil/ir/inst.h"
#include "anvil/ir/ret_inst.h"
#include <cassert>

namespace anvil {

class IRBuilder {
    Context    &_ctx;
    BasicBlock *_insertBlock{};
    Inst       *_insertPoint{};

public:
    explicit IRBuilder (Context &ctx) : _ctx (ctx) {}

    void
    SetInsertPoint (BasicBlock *block) {
        _insertBlock = block;
        _insertPoint = nullptr;
    }

    void
    SetInsertPoint (Inst *point) {
        assert (
            point->Parent () != nullptr && "Insert point must has a parent BasicBlock");
        _insertBlock = point->Parent ();
        _insertPoint = point;
    }

    BasicBlock *
    InsertBlock () const {
        return _insertBlock;
    }

    Inst *
    InsertPoint () const {
        return _insertPoint;
    }

    BasicBlock *
    CreateBasicBlock (Function *parent, std::string_view name = "") {
        auto *mem = _ctx.Allocator ().Alloc<BasicBlock> ();
        auto *bb  = ::new (mem) BasicBlock (name);
        parent->AddBasicBlock (bb);
        return bb;
    }

    Value *
    CreateBinaryOperator (
        Inst::OpCode opcode, Value *lhs, Value *rhs, std::string_view name = "") {
        assert (
            lhs->GetType () == rhs->GetType ()
            && "Binary operator must have the same operand types");

        // TODO: uncomment
        // if (auto *folded = foldIfCan (opcode, lhs, rhs)) {
        //     return folded;
        // }

        auto *inst = BinaryOperator::Create (opcode, lhs, rhs, _ctx, name);
        if (_insertBlock != nullptr) {
            _insertBlock->AddInst (inst, _insertPoint);
        }
        return inst;
    }

    Value *
    CreateAdd (Value *lhs, Value *rhs, std::string_view name = "") {
        return CreateBinaryOperator (Inst::OpCode::Add, lhs, rhs, name);
    }

    Value *
    CreateSub (Value *lhs, Value *rhs, std::string_view name = "") {
        return CreateBinaryOperator (Inst::OpCode::Sub, lhs, rhs, name);
    }

    Value *
    CreateRet (Value *ret) {
        auto *mem  = _ctx.Allocator ().Alloc<RetInst> ();
        auto *inst = ::new (mem) RetInst (ret);
        if (_insertBlock != nullptr) {
            _insertBlock->AddInst (inst, _insertPoint);
        }
        return inst;
    }

    IntegerType *
    GetIntTy (unsigned width) {
        return _ctx.GetIntTy (width);
    }

    ConstantInt *
    GetIntVal (Type *type, uint64_t val) {
        return _ctx.GetIntVal (type, val);
    }

    // NOLINTBEGIN(bugprone-macro-parentheses)
#define get_int_ty(width)                                                                \
    IntegerType *GetInt##width##Ty () {                                                  \
        return GetIntTy (width);                                                         \
    }

    get_int_ty (1);
    get_int_ty (8);
    get_int_ty (16);
    get_int_ty (32);
    get_int_ty (64);

#undef get_int_ty

#define get_int_val(width, type)                                                         \
    ConstantInt *GetInt##width##Val (type val) {                                         \
        return GetIntVal (GetInt##width##Ty (), static_cast<uint64_t> (val));            \
    }

    get_int_val (1, bool);
    get_int_val (8, uint8_t);
    get_int_val (16, uint16_t);
    get_int_val (32, uint32_t);
    get_int_val (64, uint64_t);

#undef get_int_val
    // NOLINTEND(bugprone-macro-parentheses)

private:
    Value *
    foldIfCan (Inst::OpCode opcode, Value *lhs, Value *rhs);
};

}
