#include "anvil/core/context.h"
#include "anvil/core/module.h"
#include "anvil/ir/builder.h"
#include "anvil/ir/function.h"
#include "anvil/streams/raw_ostream.h"
#include "anvil/target/data_layout.h"
#include "anvil/target/machine_context.h"
#include "anvil/target/machine_module.h"
#include "anvil/target/target_machine.h"
#include "anvil/target/triple.h"
#include "anvil/target/x86/target_machine.h"
#include <memory>

std::unique_ptr<anvil::Module>
ExampleMod (anvil::Context &ctx, anvil::IRBuilder &builder);

int
main () {
    anvil::Context   ctx;
    anvil::IRBuilder builder (ctx);
    auto             mod = ExampleMod (ctx, builder);
    mod->Print (anvil::Outs ());

    anvil::Triple         triple ("x86_64-unknown-linux-gnu");
    anvil::DataLayout     dl;
    anvil::TargetMachine *tm{};
    if (triple.GetArch () == anvil::Arch::X86_64) {
        tm = new anvil::X86TargetMachine (triple, dl);
    }

    anvil::MachineContext mctx;
    anvil::MachineModule  mmod (mctx, mod->Name ());

    tm->SelectInstructions (*mod, mmod);

    delete tm;

    return 0;
}

std::unique_ptr<anvil::Module>
ExampleMod (anvil::Context &ctx, anvil::IRBuilder &builder) {
    auto  mod   = std::make_unique<anvil::Module> (ctx, "test_mod");
    auto *i32Ty = builder.GetInt32Ty ();
    auto *c1    = builder.GetIntVal (i32Ty, 2);
    auto *c2    = builder.GetIntVal (i32Ty, 3);
    auto *c3    = builder.GetIntVal (i32Ty, 0);
    auto *c4    = builder.GetIntVal (i32Ty, 5);

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
    return std::move (mod);
}
