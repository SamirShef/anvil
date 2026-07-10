#include "anvil/core/module.h"
#include "anvil/ir/builder.h"
#include "anvil/ir/function.h"
#include "anvil/streams/raw_ostream.h"
#include <memory>

int
main () {
    anvil::Context   ctx;
    anvil::IRBuilder builder (ctx);
    auto             mod   = std::make_unique<anvil::Module> (ctx, "test_mod");
    auto            *i32Ty = builder.GetInt32Ty ();
    auto            *c1    = builder.GetIntVal (i32Ty, 2);
    auto            *c2    = builder.GetIntVal (i32Ty, 3);
    auto            *c3    = builder.GetIntVal (i32Ty, 0);
    auto            *c4    = builder.GetIntVal (i32Ty, 5);

    auto *mainFuncTy    = anvil::FunctionType::Create (ctx, i32Ty, {});
    auto *mainFunc      = anvil::Function::Create (mod.get (), mainFuncTy, "main");
    auto *mainFuncEntry = builder.CreateBasicBlock (mainFunc, "entry");
    builder.SetInsertPoint (mainFuncEntry);
    auto *add               = builder.CreateAdd (c1, c2, "add_tmp");
    auto *sub               = builder.CreateSub (add, c2, "sub_tmp");
    auto *mainFuncSomeBlock = builder.CreateBasicBlock (mainFunc, "some_block");
    builder.SetInsertPoint (mainFuncSomeBlock);
    auto *sub2 = builder.CreateSub (c3, c4, "neg_tmp");
    builder.CreateRet (c3);

    auto *sumFuncTy = anvil::FunctionType::Create (ctx, i32Ty, { i32Ty, i32Ty });
    auto *sumFunc   = anvil::Function::Create (mod.get (), sumFuncTy, "sum");
    sumFunc->Arg (0)->SetName ("a");
    sumFunc->Arg (1)->SetName ("b");
    auto *sumFuncEntry = builder.CreateBasicBlock (sumFunc, "entry");
    builder.SetInsertPoint (sumFuncEntry);
    auto *sum = builder.CreateAdd (sumFunc->Arg (0), sumFunc->Arg (1), "add_tmp");
    builder.CreateRet (sum);

    mod->Print (anvil::Outs ());
    return 0;
}
