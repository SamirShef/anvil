#pragma once
#include "anvil/core/context.h"
#include "anvil/ir/function.h"
#include "anvil/streams/raw_ostream.h"

namespace anvil {

class Module {
    Context         &_ctx;
    std::string_view _name;
    Function        *_funcsStart{};
    Function        *_funcsEnd{};

public:
    Module (Context &ctx, std::string_view name) : _ctx (ctx), _name (name) {}

    Context &
    Ctx () {
        return _ctx;
    }

    std::string_view
    Name () const {
        return _name;
    }

    RawOstream &
    Print (RawOstream &os) {
        for (auto *func = _funcsStart; func != nullptr; func = func->_next) {
            if (func != _funcsStart) {
                os << "\n";
            }
            func->Print (os) << '\n';
        }
        return os;
    }

    void
    AddFunction (Function *func) {
        if (_funcsEnd == nullptr) {
            _funcsEnd = _funcsStart = func;
        } else {
            _funcsEnd->_next = func;
            func->_prev      = _funcsEnd;
            _funcsEnd        = func;
        }
    }

    Function *
    FuncsStart () const {
        return _funcsStart;
    }

    Function *
    FuncsEnd () const {
        return _funcsEnd;
    }
};

}
