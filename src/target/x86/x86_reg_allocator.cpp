#include "anvil/target/x86/x86_reg_allocator.h"
#include "anvil/collections/utils.h"

namespace anvil::x86 {

void
X86RegisterAllocator::linearScan (MachineFunction *func) {
    auto &intervals = _liveness.Intervals ();
    for (const auto &[reg, interval] : intervals) {
        if (reg.IsVirtual ()) {
            _unhandledRegs.PushBack ({ reg.Id (), interval });
        }
    }
    Sort (_unhandledRegs, [] (const auto &a, const auto &b) {
        return a.second.Start < b.second.Start;
    });
}

void
X86RegisterAllocator::rewriteRegs (MachineFunction *func) {}

void
X86RegisterAllocator::allocFunction (MachineFunction *func) {
    _vreg2Phys.Clear ();
    _activeRegs.Clear ();
    _unhandledRegs.Clear ();

    linearScan (func);
    rewriteRegs (func);
}

}
