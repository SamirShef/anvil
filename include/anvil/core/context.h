#pragma once
#include "anvil/collections/hash_map.h"
#include "anvil/core/constant_int.h"
#include "anvil/core/types.h"
#include "anvil/support/arena.h"
#include <utility>

namespace anvil {

class Context {
    ArenaAllocator                                      _arena;
    HashMap<unsigned, IntegerType *>                    _intTypes;
    HashMap<std::pair<Type *, uint64_t>, ConstantInt *> _constants;

public:
    Context ()                = default;
    Context (const Context &) = delete;
    Context (Context &&)      = default;
    Context &
    operator= (const Context &) = delete;
    Context &
    operator= (Context &&) = default;
    ~Context ()            = default;

    ArenaAllocator &
    Allocator () {
        return _arena;
    }

    IntegerType *
    GetIntTy (unsigned width) {
        if (auto *ty = _intTypes.Find (width)) {
            return *ty;
        }
        auto *ty = _arena.Alloc<IntegerType> ();
        ::new (ty) IntegerType (width);
        _intTypes.Insert (width, ty);
        return ty;
    }

    ConstantInt *
    GetIntVal (Type *type, uint64_t val) {
        if (auto *c = _constants.Find ({ type, val }); c != nullptr) {
            return *c;
        }
        auto *c = _arena.Alloc<ConstantInt> ();
        ::new (c) ConstantInt (type, val);
        _constants.Insert ({ type, val }, c);
        return c;
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
};

}
