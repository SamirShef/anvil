#pragma once
#include "anvil/collections/small_vector.h"
#include "anvil/core/context.h"
#include "anvil/core/types.h"
#include "anvil/core/value.h"
#include "anvil/ir/argument.h"
#include "anvil/ir/basic_block.h"
#include "anvil/streams/raw_ostream.h"
#include "anvil/support/casting.h"
#include <cassert>

namespace anvil {

class Module;

class FunctionType : public Type {
    Type                  *_retType;
    SmallVector<Type *, 4> _args;

    FunctionType (Type *retType, SmallVector<Type *, 4> args)
        : _retType (retType), _args (std::move (args)), Type (Type::Kind::Function) {}

public:
    static FunctionType *
    Create (Context &ctx, Type *retType, SmallVector<Type *, 4> args) {
        auto *mem = ctx.Allocator ().Alloc<FunctionType> ();
        return ::new (mem) FunctionType (retType, std::move (args));
    }

    RawOstream &
    Print (RawOstream &os) override {
        os << "fn (";
        for (size_t i = 0; i < _args.Size (); ++i) {
            if (i != 0) {
                os << ", ";
            }
            _args[i]->Print (os);
        }
        os << "): ";
        _retType->Print (os);
        return os;
    }

    Type *
    ReturnType () const {
        return _retType;
    }

    size_t
    NumArgs () const {
        return _args.Size ();
    }

    Type *
    ParamType (size_t index) const {
        return _args[index];
    }

    static bool
    ClassOf (Type *ty) {
        return ty->GetKind () == Type::Kind::Function;
    }

    static bool
    ClassOf (const Type *ty) {
        return ty->GetKind () == Type::Kind::Function;
    }
};

class Function : public Value {
    friend class Module;
    SmallVector<Argument *, 4> _args;
    BasicBlock                *_start{};
    BasicBlock                *_end{};
    Function                  *_prev{};
    Function                  *_next{};

    Function (FunctionType *ty, std::string_view name, Module *mod);

public:
    RawOstream &
    Print (RawOstream &os) override {
        os << "fn #" << _name << "(";
        for (size_t i = 0; i < _args.Size (); ++i) {
            if (i != 0) {
                os << ", ";
            }
            _args[i]->Print (os);
        }
        os << "): ";
        FunctionTy ()->ReturnType ()->Print (os) << " {\n";
        for (auto *bb = _start; bb != nullptr; bb = bb->Next ()) {
            if (bb != _start) {
                os << '\n';
            }
            bb->Print (os);
        }
        return os << '}';
    }

    RawOstream &
    PrintAsOperand (RawOstream &os) override {
        return os << "<not implemented>";
    }

    static Function *
    Create (Module *mod, FunctionType *ty, std::string_view name);

    FunctionType *
    FunctionTy () const {
        return Cast<FunctionType> (_type);
    }

    size_t
    ArgCount () const {
        return _args.Size ();
    }

    Argument *
    Arg (size_t index) {
        return _args[index];
    }

    void
    AddBasicBlock (BasicBlock *block) {
        if (_end == nullptr) {
            _end = _start = block;
        } else {
            block->_prev = _end;
            _end->_next  = block;
            _end         = block;
        }
    }
};

}
