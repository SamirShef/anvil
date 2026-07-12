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
#include "anvil/target/x86/x86_asm_printer.h"
#include "anvil/target/x86/x86_liveness_analysis.h"
#include "anvil/target/x86/x86_pe_inserter.h"
#include "anvil/target/x86/x86_reg_allocator.h"
#include "anvil/target/x86/x86_register_coalescer.h"
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
        tm = new anvil::x86::X86TargetMachine (triple, dl);
    }

    anvil::MachineContext mctx;
    anvil::MachineModule  mmod (mctx, mod->Name ());

    auto asmPrinter = anvil::x86::X86AsmPrinter (anvil::Outs ());

    tm->SelectInstructions (*mod, mmod);

    anvil::x86::X86LivenessAnalysis liveness;
    liveness.AnalyseModule (mmod);

    anvil::x86::X86RegisterCoalescer coalescer (liveness);
    coalescer.CoalesceModule (mmod);

    anvil::x86::X86RegisterAllocator regAllocator (mctx);
    regAllocator.AllocModule (mmod);

    anvil::x86::X86PEInserter peInserter;
    peInserter.InsertPrologEpilog (mmod, mctx);

    asmPrinter.EmitModule (mmod);

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

    auto *sumFuncTy = anvil::FunctionType::Create (ctx, i32Ty, { i32Ty, i32Ty });
    auto *sumFunc   = anvil::Function::Create (mod.get (), sumFuncTy, "sum");
    sumFunc->Arg (0)->SetName ("a");
    sumFunc->Arg (1)->SetName ("b");
    auto *sumFuncEntry = builder.CreateBasicBlock (sumFunc, "entry");
    builder.SetInsertPoint (sumFuncEntry);
    auto *sum = builder.CreateAdd (sumFunc->Arg (0), sumFunc->Arg (1), "add_tmp");
    // auto *add1     = builder.CreateAdd (c3, c1, "add1_tmp");
    // auto *add2     = builder.CreateAdd (add1, c1, "add2_tmp");
    // auto *add3     = builder.CreateAdd (add2, c1, "add3_tmp");
    // auto *add4     = builder.CreateAdd (add3, c1, "add4_tmp");
    // auto *add5     = builder.CreateAdd (add4, c1, "add5_tmp");
    // auto *add6     = builder.CreateAdd (add5, c1, "add6_tmp");
    // auto *add7     = builder.CreateAdd (add6, c1, "add7_tmp");
    // auto *add8     = builder.CreateAdd (add7, c1, "add8_tmp");
    // auto *add9     = builder.CreateAdd (add8, c1, "add9_tmp");
    // auto *add10    = builder.CreateAdd (add9, c1, "add10_tmp");
    // auto *res1     = builder.CreateAdd (add1, add2, "res1_tmp");
    // auto *res2     = builder.CreateAdd (res1, add3, "res2_tmp");
    // auto *res3     = builder.CreateAdd (res2, add4, "res3_tmp");
    // auto *res4     = builder.CreateAdd (res3, add5, "res4_tmp");
    // auto *res5     = builder.CreateAdd (res4, add6, "res5_tmp");
    // auto *res6     = builder.CreateAdd (res5, add7, "res6_tmp");
    // auto *res7     = builder.CreateAdd (res6, add8, "res7_tmp");
    // auto *res8     = builder.CreateAdd (res7, add9, "res8_tmp");
    // auto *resFinal = builder.CreateAdd (res8, add10, "resFinal_tmp");
    builder.CreateRet (sum);
    return std::move (mod);
}
