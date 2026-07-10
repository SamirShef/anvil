#pragma once
#include "anvil/target/machine_context.h"
#include "anvil/target/machine_function.h"
#include <string_view>

namespace anvil {

class MachineModule {
    MachineContext  &_ctx;
    std::string_view _name;
    MachineFunction *_funcsStart{};
    MachineFunction *_funcsEnd{};

public:
    MachineModule (MachineContext &ctx, std::string_view name)
        : _ctx (ctx), _name (name) {}

    void
    AddFunction (MachineFunction *func) {
        if (_funcsEnd == nullptr) {
            _funcsEnd = _funcsStart = func;
        } else {
            _funcsEnd->_next = func;
            func->_prev      = _funcsEnd;
            _funcsEnd        = func;
        }
    }

    MachineFunction *
    FuncsStart () const {
        return _funcsStart;
    }

    MachineFunction *
    FuncsEnd () const {
        return _funcsEnd;
    }
};

}
