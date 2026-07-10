#include "anvil/core/value.h"
#include "anvil/core/use.h"

namespace anvil {

void
Value::AddUse (Use *u) {
    assert (u != nullptr);
    u->_next = _firstUse;
    if (_firstUse != nullptr) {
        _firstUse->_prev = &u->_next;
    }
    _firstUse = u;
    u->_prev  = &_firstUse;
}

void
// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
Value::RemoveUse (Use *u) {
    assert (u != nullptr);
    if (u->_next != nullptr) {
        u->_next->_prev = u->_prev;
    }
    if (u->_prev != nullptr) {
        *u->_prev = u->_next;
    }
    u->_next = nullptr;
    u->_prev = nullptr;
}

}
