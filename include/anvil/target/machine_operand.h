#pragma once
#include "anvil/target/register.h"

namespace anvil {

class MachineBasicBlock;

class MachineOperand {
public:
    enum Kind : uint8_t { RegOp, ImmOp, MBBOp, FrameIndexOp };

private:
    Kind _kind;
    bool _isDef{};
    bool _isKill{};

    union Data {
        Register           Reg;
        int64_t            Imm;
        MachineBasicBlock *MBB;
        int                FI;

        bool
        operator== (const Data &other) const {
            return Reg == other.Reg && Imm == other.Imm && MBB == other.MBB
                   && FI == other.FI;
        }

        bool
        operator!= (const Data &other) const {
            return !(*this == other);
        }
    } _storage{};

public:
    explicit MachineOperand (Kind kind) : _kind (kind) {}

    bool
    operator== (const MachineOperand &other) const = default;

    bool
    operator!= (const MachineOperand &other) const = default;

    static MachineOperand
    CreateReg (Register reg, bool isDef = false, bool isKill = false) {
        MachineOperand op (Kind::RegOp);
        op._isDef       = isDef;
        op._isKill      = isKill;
        op._storage.Reg = reg;
        return op;
    }

    static MachineOperand
    CreateImm (int64_t imm, bool isDef = false) {
        MachineOperand op (Kind::ImmOp);
        op._isDef       = isDef;
        op._storage.Imm = imm;
        return op;
    }

    static MachineOperand
    CreateMBB (MachineBasicBlock *mbb) {
        MachineOperand op (Kind::MBBOp);
        op._storage.MBB = mbb;
        return op;
    }

    static MachineOperand
    CreateFI (int fi, bool isDef = false) {
        MachineOperand op (Kind::FrameIndexOp);
        op._isDef      = isDef;
        op._storage.FI = fi;
        return op;
    }

    Kind
    GetKind () const {
        return _kind;
    }

    bool
    IsReg () const {
        return GetKind () == Kind::RegOp;
    }

    bool
    IsImm () const {
        return GetKind () == Kind::ImmOp;
    }

    bool
    IsMBB () const {
        return GetKind () == Kind::MBBOp;
    }

    bool
    IsFI () const {
        return GetKind () == Kind::FrameIndexOp;
    }

    bool
    IsDef () const {
        return _isDef;
    }

    bool
    IsUse () const {
        return !IsDef ();
    }

    bool
    IsKill () const {
        return _isKill;
    }

    void
    SetKill (bool isKill) {
        _isKill = isKill;
    }

    Register
    AsReg () const {
        return _storage.Reg;
    }

    int64_t
    AsImm () const {
        return _storage.Imm;
    }

    MachineBasicBlock *
    AsMBB () const {
        return _storage.MBB;
    }

    int
    AsFI () const {
        return _storage.FI;
    }
};

}
