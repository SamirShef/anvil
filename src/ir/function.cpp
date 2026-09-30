#include "anvil/ir/function.h"
#include "anvil/core/module.h"

namespace anvil {

Function::Function (FunctionType *ty, std::string_view name, Module *mod)
    : Value (Value::Kind::Function, ty, name) {
    auto &alloc = mod->Ctx ().Allocator ();
    for (size_t i = 0; i < ty->NumArgs (); ++i) {
        auto *argMem = alloc.Alloc<Argument> ();
        auto *arg    = ::new (argMem) Argument (ty->ParamType (i), this, i);
        _args.PushBack (arg);
    }
    mod->AddFunction (this);
}

Function *
Function::Create (Module *mod, FunctionType *ty, std::string_view name) {
    auto *mem = mod->Ctx ().Allocator ().Alloc<Function> ();
    return ::new (mem) Function (ty, name, mod);
}

}
