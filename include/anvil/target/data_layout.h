#pragma once
#include "anvil/core/types.h"
#include "anvil/support/casting.h"
#include <cstdint>

namespace anvil {

class DataLayout {
    bool    _isLittleEndian = true;
    uint8_t _ptrSize        = 8;

public:
    DataLayout () = default;

    bool
    IsLittleEndian () const {
        return _isLittleEndian;
    }

    uint8_t
    PtrSize () const {
        return _ptrSize;
    }

    uint64_t
    TypeAllocSizeInBits (Type *type) const {
        switch (type->GetKind ()) {
        case Type::Kind::Integer:
            return Cast<IntegerType> (type)->Width ();
        case Type::Kind::Function:
            return _ptrSize;
        }
        return 0;
    }

    uint64_t
    TypeAllocSizeInBytes (Type *type) const {
        return (TypeAllocSizeInBits (type) + 7) / 8;
    }
};

}
