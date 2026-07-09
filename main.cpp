#include "anvil/core/context.h"
#include "anvil/ir/builder.h"
#include "anvil/streams/raw_ostream.h"

int
main () {
    anvil::Context   ctx;
    anvil::IRBuilder builder (ctx);
    auto            *bb = builder.CreateBasicBlock ("entry");
    builder.SetInsertPoint (bb);
    auto *i32Ty = builder.GetInt32Ty ();
    auto *c1    = builder.GetIntVal (i32Ty, 2);
    auto *c2    = builder.GetIntVal (i32Ty, 3);
    auto *add   = builder.CreateAdd (c1, c2, "add_tmp");
    auto *sub   = builder.CreateSub (add, c2, "sub_tmp");
    bb->Print (anvil::Outs ());
    return 0;
}
