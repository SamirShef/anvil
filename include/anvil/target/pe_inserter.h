#pragma once

namespace anvil {

class MachineModule;
class MachineContext;

class PEInserter {
public:
    PEInserter (const PEInserter &) = default;
    PEInserter (PEInserter &&)      = delete;
    PEInserter &
    operator= (const PEInserter &) = default;
    PEInserter &
    operator= (PEInserter &&) = delete;
    PEInserter ()             = default;
    virtual ~PEInserter ()    = default;

    virtual void
    InsertPrologEpilog (MachineModule &mmod, MachineContext &mctx) = 0;
};

}
