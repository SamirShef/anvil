#pragma once
#include "anvil/target/machine_inst.h"
#include <string_view>

namespace anvil {

class MachineFunction;

class MachineBasicBlock {
    friend class MachineFunction;
    std::string_view   _name;
    MachineFunction   *_parent{};
    MachineInst       *_start{};
    MachineInst       *_end{};
    MachineBasicBlock *_prev{};
    MachineBasicBlock *_next{};

public:
    MachineBasicBlock (std::string_view name, MachineFunction *parent)
        : _name (name), _parent (parent) {}

    std::string_view
    Name () const {
        return _name;
    }

    MachineFunction *
    Parent () const {
        return _parent;
    }

    MachineInst *
    Start () const {
        return _start;
    }

    void
    SetStart (MachineInst *start) {
        _start = start;
    }

    MachineInst *
    End () const {
        return _end;
    }

    MachineBasicBlock *
    Prev () const {
        return _prev;
    }

    MachineBasicBlock *
    Next () const {
        return _next;
    }

    void
    Emit (MachineInst *inst) {
        inst->_parent = this;
        if (_end == nullptr) {
            _end = _start = inst;
        } else {
            _end->_next = inst;
            inst->_prev = _end;
            _end        = inst;
        }
    }

    void
    InsertBefore (MachineInst *inst, MachineInst *point) {
        inst->_parent = this;
        if (point == _start) {
            inst->_next   = _start;
            _start->_prev = inst;
            _start        = inst;
        } else {
            auto *prev   = point->_prev;
            prev->_next  = inst;
            inst->_prev  = prev;
            inst->_next  = point;
            point->_prev = inst;
        }
    }
};

}
