#include "anvil/ir/builder.h"
#include "anvil/support/casting.h"

namespace anvil {

using OpCode = Inst::OpCode;

Value *
IRBuilder::foldIfCan (OpCode opcode, Value *lhs, Value *rhs) {
    if (!IsA<Constant> (lhs) || !IsA<Constant> (rhs)) {
        return nullptr;
    }
    auto *lc = Cast<Constant> (lhs);
    auto *rc = Cast<Constant> (rhs);
    switch (opcode) {
    case OpCode::Add: {
        if (lc->GetConstantKind () == Constant::Integer) {
            auto *lci = Cast<ConstantInt> (lc);
            auto *rci = Cast<ConstantInt> (rc);
            return _ctx.GetIntVal (lci->GetType (), lci->Val () + rci->Val ());
        }
        return nullptr;
    }
    case OpCode::Sub: {
        if (lc->GetConstantKind () == Constant::Integer) {
            auto *lci = Cast<ConstantInt> (lc);
            auto *rci = Cast<ConstantInt> (rc);
            return _ctx.GetIntVal (lci->GetType (), lci->Val () - rci->Val ());
        }
        return nullptr;
    }
    default:
        return nullptr;
    }
}

}
