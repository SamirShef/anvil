#pragma once

#include <cassert>

namespace anvil {

template <typename T1, typename T2>
bool
IsA (T2 *obj) {
    return T1::ClassOf (obj);
}

template <typename To, typename From>
To *
Cast (From *from) {
    assert (IsA<To> (from) && "Cannot cast incompatible types");
    return static_cast<To *> (from);
}

template <typename To, typename From>
const To *
Cast (const From *from) {
    assert (IsA<To> (from) && "Cannot cast incompatible types");
    return static_cast<const To *> (from);
}

template <typename To, typename From>
To *
DynCast (From *from) {
    if (!IsA<To> (from)) {
        return nullptr;
    }
    return static_cast<To *> (from);
}

template <typename To, typename From>
const To *
DynCast (const From *from) {
    if (!IsA<To> (from)) {
        return nullptr;
    }
    return static_cast<const To *> (from);
}

}
