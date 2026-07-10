#pragma once
#include <cstdint>

namespace anvil {

class Register {
    uint32_t _id{};

public:
    Register () = default;

    explicit constexpr Register (uint32_t id) : _id (id) {}

    uint32_t
    Id () const {
        return _id;
    }

    bool
    IsPhysical () const {
        return _id > 0 && _id < 1024;
    }

    bool
    IsVirtual () const {
        return _id >= 1024;
    }

    bool
    IsNone () const {
        return _id == 0;
    }

    bool
    operator== (Register &other) const {
        return _id == other._id;
    }

    bool
    operator!= (Register &other) const {
        return _id != other._id;
    }
};

}
