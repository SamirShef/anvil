#pragma once

namespace anvil {

class MachineModule;
class MachineContext;

class PEInserter {
protected:
    MachineContext &_mctx;

public:
    PEInserter (const PEInserter &) = default;
    PEInserter (PEInserter &&)      = delete;
    PEInserter &
    operator= (const PEInserter &) = delete;
    PEInserter &
    operator= (PEInserter &&) = delete;

    explicit PEInserter (MachineContext &mctx) : _mctx (mctx) {}

    virtual ~PEInserter () = default;

    virtual void
    InsertPrologEpilog (MachineModule &mmod, MachineContext &mctx) = 0;
};

}
