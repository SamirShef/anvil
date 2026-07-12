#include "anvil/core/context.h"
#include "anvil/core/module.h"
#include "anvil/ir/builder.h"
#include "anvil/ir/function.h"
#include "anvil/streams/raw_fd_ostream.h"
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

std::unique_ptr<anvil::Module>
TestMaxPressure (anvil::Context &ctx, anvil::IRBuilder &builder);

std::unique_ptr<anvil::Module>
TestPingPong (anvil::Context &ctx, anvil::IRBuilder &builder);

std::unique_ptr<anvil::Module>
TestDiamond (anvil::Context &ctx, anvil::IRBuilder &builder);

void
CompileAndEmitToOStream (
    std::unique_ptr<anvil::Module> (*predicate) (anvil::Context &, anvil::IRBuilder &),
    anvil::RawOstream &out);

int
main () {
    // anvil::RawFdOstream exampleFile ("example.s");
    // anvil::RawFdOstream maxPressureFile ("max_pressure.s");
    // anvil::RawFdOstream pingPongFile ("ping_pong.s");
    // anvil::RawFdOstream diamondFile ("diamond.s");
    // CompileAndEmitToOStream (ExampleMod, exampleFile);
    // anvil::Outs () << '\n';
    // CompileAndEmitToOStream (TestMaxPressure, maxPressureFile);
    // anvil::Outs () << '\n';
    // CompileAndEmitToOStream (TestPingPong, pingPongFile);
    // anvil::Outs () << '\n';
    // CompileAndEmitToOStream (TestDiamond, diamondFile);
    CompileAndEmitToOStream (ExampleMod, anvil::Outs ());
    anvil::Outs () << '\n';
    CompileAndEmitToOStream (TestMaxPressure, anvil::Outs ());
    anvil::Outs () << '\n';
    CompileAndEmitToOStream (TestPingPong, anvil::Outs ());
    anvil::Outs () << '\n';
    CompileAndEmitToOStream (TestDiamond, anvil::Outs ());
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
    auto *sum      = builder.CreateAdd (sumFunc->Arg (0), sumFunc->Arg (1), "sum_tmp");
    auto *add1     = builder.CreateAdd (c3, c1, "add1_tmp");
    auto *add2     = builder.CreateAdd (add1, c1, "add2_tmp");
    auto *add3     = builder.CreateAdd (add2, c1, "add3_tmp");
    auto *add4     = builder.CreateAdd (add3, c1, "add4_tmp");
    auto *add5     = builder.CreateAdd (add4, c1, "add5_tmp");
    auto *add6     = builder.CreateAdd (add5, c1, "add6_tmp");
    auto *add7     = builder.CreateAdd (add6, c1, "add7_tmp");
    auto *add8     = builder.CreateAdd (add7, c1, "add8_tmp");
    auto *add9     = builder.CreateAdd (add8, c1, "add9_tmp");
    auto *add10    = builder.CreateAdd (add9, c1, "add10_tmp");
    auto *res1     = builder.CreateAdd (add1, add2, "res1_tmp");
    auto *res2     = builder.CreateAdd (res1, add3, "res2_tmp");
    auto *res3     = builder.CreateAdd (res2, add4, "res3_tmp");
    auto *res4     = builder.CreateAdd (res3, add5, "res4_tmp");
    auto *res5     = builder.CreateAdd (res4, add6, "res5_tmp");
    auto *res6     = builder.CreateAdd (res5, add7, "res6_tmp");
    auto *res7     = builder.CreateAdd (res6, add8, "res7_tmp");
    auto *res8     = builder.CreateAdd (res7, add9, "res8_tmp");
    auto *resFinal = builder.CreateAdd (res8, add10, "resFinal_tmp");
    builder.CreateRet (resFinal);
    return std::move (mod);
}

std::unique_ptr<anvil::Module>
TestMaxPressure (anvil::Context &ctx, anvil::IRBuilder &builder) {
    auto  mod   = std::make_unique<anvil::Module> (ctx, "pressure_mod");
    auto *i32Ty = builder.GetInt32Ty ();
    auto *c2    = builder.GetIntVal (i32Ty, 2);

    auto *funcTy = anvil::FunctionType::Create (ctx, i32Ty, { i32Ty, i32Ty });
    auto *func   = anvil::Function::Create (mod.get (), funcTy, "test_pressure");
    auto *entry  = builder.CreateBasicBlock (func, "entry");
    builder.SetInsertPoint (entry);

    auto *longLive = builder.CreateAdd (func->Arg (0), func->Arg (1), "long_live");

    auto *v1 = builder.CreateAdd (c2, c2, "v1"); // 4
    auto *v2 = builder.CreateAdd (v1, c2, "v2"); // 6
    auto *v3 = builder.CreateAdd (v2, c2, "v3"); // 8
    auto *v4 = builder.CreateAdd (v3, c2, "v4"); // 10
    auto *v5 = builder.CreateAdd (v4, c2, "v5"); // 12
    auto *v6 = builder.CreateAdd (v5, c2, "v6"); // 14

    auto *sum1 = builder.CreateAdd (v1, v2, "sum1");
    auto *sum2 = builder.CreateAdd (sum1, v3, "sum2");
    auto *sum3 = builder.CreateAdd (sum2, v4, "sum3");
    auto *sum4 = builder.CreateAdd (sum3, v5, "sum4");
    auto *sum5 = builder.CreateAdd (sum4, v6, "sum5");

    auto *finalRes = builder.CreateAdd (sum5, longLive, "final_res");
    builder.CreateRet (finalRes);
    return std::move (mod);
}

std::unique_ptr<anvil::Module>
TestPingPong (anvil::Context &ctx, anvil::IRBuilder &builder) {
    auto  mod   = std::make_unique<anvil::Module> (ctx, "pingpong_mod");
    auto *i32Ty = builder.GetInt32Ty ();

    auto *funcTy = anvil::FunctionType::Create (ctx, i32Ty, { i32Ty, i32Ty });
    auto *func   = anvil::Function::Create (mod.get (), funcTy, "ping_pong");
    auto *entry  = builder.CreateBasicBlock (func, "entry");
    builder.SetInsertPoint (entry);

    auto *x = builder.CreateAdd (func->Arg (0), func->Arg (1), "x"); // a + b
    auto *y = builder.CreateAdd (func->Arg (1), x, "y");             // b + x
    auto *z = builder.CreateAdd (func->Arg (0), y, "z");             // a + y

    auto *res = builder.CreateAdd (x, z, "res");
    builder.CreateRet (res);
    return std::move (mod);
}

std::unique_ptr<anvil::Module>
TestDiamond (anvil::Context &ctx, anvil::IRBuilder &builder) {
    auto  mod   = std::make_unique<anvil::Module> (ctx, "diamond_mod");
    auto *i32Ty = builder.GetInt32Ty ();
    auto *c5    = builder.GetIntVal (i32Ty, 5);

    auto *funcTy = anvil::FunctionType::Create (ctx, i32Ty, { i32Ty });
    auto *func   = anvil::Function::Create (mod.get (), funcTy, "diamond");
    auto *entry  = builder.CreateBasicBlock (func, "entry");
    builder.SetInsertPoint (entry);

    auto *base = builder.CreateAdd (func->Arg (0), c5, "base"); // a + 5

    auto *left1 = builder.CreateAdd (base, c5, "left1");
    auto *left2 = builder.CreateAdd (left1, c5, "left2");

    auto *right1 = builder.CreateAdd (base, left2, "right1");
    auto *right2 = builder.CreateAdd (right1, base, "right2");

    builder.CreateRet (right2);
    return std::move (mod);
}

void
CompileAndEmitToOStream (
    std::unique_ptr<anvil::Module> (*predicate) (anvil::Context &, anvil::IRBuilder &),
    anvil::RawOstream &out) {
    anvil::Context   ctx;
    anvil::IRBuilder builder (ctx);
    auto             mod = predicate (ctx, builder);
    mod->Print (anvil::Outs ());

    anvil::Triple         triple ("x86_64-unknown-linux-gnu");
    anvil::DataLayout     dl;
    anvil::TargetMachine *tm{};
    if (triple.GetArch () == anvil::Arch::X86_64) {
        tm = new anvil::x86::X86TargetMachine (triple, dl);
    }

    anvil::MachineContext mctx;
    anvil::MachineModule  mmod (mctx, mod->Name ());

    auto asmPrinter = anvil::x86::X86AsmPrinter (out);

    tm->SelectInstructions (*mod, mmod);

    anvil::x86::X86LivenessAnalysis liveness;
    liveness.AnalyseModule (mmod);

    anvil::x86::X86RegisterCoalescer coalescer (liveness);
    coalescer.CoalesceModule (mmod);
    liveness.AnalyseModule (mmod);

    for (const auto &[reg, interval] : liveness.Intervals ()) {
        anvil::Outs () << reg.Id () << ": " << "[" << interval.Start << ", "
                       << interval.End << "]\n";
    }

    anvil::x86::X86RegisterAllocator regAllocator (mctx, liveness);
    regAllocator.AllocModule (mmod);

    anvil::x86::X86PEInserter peInserter;
    peInserter.InsertPrologEpilog (mmod, mctx);

    asmPrinter.EmitModule (mmod);

    delete tm;
}
