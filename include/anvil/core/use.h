#pragma once

namespace anvil {

class Inst;
class Value;

class Use {
private:
    Value *_value = nullptr;
    Inst  *_user  = nullptr;
    Use   *_next  = nullptr;
    Use  **_prev  = nullptr;

    friend class Value;
    friend class Inst;

public:
    Use (Value *value, Inst *user) : _value (value), _user (user) {}

    Value *
    Get () const {
        return _value;
    }

    Inst *
    User () const {
        return _user;
    }

    Use *
    Next () const {
        return _next;
    }
};

}
