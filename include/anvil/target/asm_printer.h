#pragma once
#include "anvil/streams/raw_ostream.h"

namespace anvil {

class MachineModule;

class AsmPrinter {
protected:
    RawOstream &_os;

public:
    AsmPrinter (const AsmPrinter &) = delete;
    AsmPrinter (AsmPrinter &&)      = delete;
    AsmPrinter &
    operator= (const AsmPrinter &) = delete;
    AsmPrinter &
    operator= (AsmPrinter &&) = delete;

    explicit AsmPrinter (RawOstream &os) : _os (os) {}

    virtual ~AsmPrinter () = default;

    virtual void
    EmitModule (MachineModule &mmod) = 0;
};

}
