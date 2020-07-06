#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Support/raw_ostream.h"
#include <functional>

using namespace llvm;

#define STOP_AFTER_PASS_NAME  "sroa"
#define STOP_AFTER_NUM_TIMES  (1)

namespace {
class StopAfterInstrument {
  unsigned Counter;
  bool Stopped;

  bool beforePass(StringRef PassID, Any IR) {
    // Allow all printing pass
    bool ShouldSkip = Stopped && !PassID.contains_lower("print");

    StringRef PassKind;
    StringRef FuncName;

    // Showing how to use IR of type llvm::Any
    if(any_isa<const Function*>(IR)) {
      PassKind = "FunctionPass";
      const auto* F = any_cast<const Function*>(IR);
      if(F->hasName()) {
        FuncName = F->getName();
      }
    }
    if(any_isa<const Module*>(IR)) {
      PassKind = "ModulePass";
    }

    if(ShouldSkip) {
      errs() << "Skipping " << PassKind << " " << PassID;
      if(FuncName.size()) {
        errs() << " @ function " << FuncName;
      }
      errs() << "\n";
    }
    return !ShouldSkip;
  }

  void afterPass(StringRef PassID, Any IR) {
    Stopped |= PassID.contains_lower(STOP_AFTER_PASS_NAME) &&
               (++Counter >= STOP_AFTER_NUM_TIMES);
  }

public:
  StopAfterInstrument()
    : Counter(0U), Stopped(false) {}

  void registerCallbacks(PassInstrumentationCallbacks& PIC) {
    using namespace std::placeholders;
    PIC.registerBeforePassCallback(
      std::bind(&StopAfterInstrument::beforePass, this, _1, _2));
    PIC.registerAfterPassCallback(
      std::bind(&StopAfterInstrument::afterPass, this, _1, _2));
  }
};
} // end anonymous namespace

static StopAfterInstrument TheStopAfterInstrument;

extern "C" ::llvm::PassPluginLibraryInfo LLVM_ATTRIBUTE_WEAK
llvmGetPassPluginInfo() {
  return {
    LLVM_PLUGIN_API_VERSION, "NewPMStopAfterInstrumentPlugin", "v0.1",
    [](PassBuilder& PB) {
      auto& PIC = *PB.getPassInstrumentationCallbacks();
      TheStopAfterInstrument.registerCallbacks(PIC);
    }
  };
}
