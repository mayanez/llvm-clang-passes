#include "CloneFunctions.h"
using namespace llvm;

bool CloneFunctions::runOnModule(Module &M) {

  std::vector<Function *> toClone;

  for (auto &F : M) {
    // NOTE: Do not clone "external" functions.
    if (!F.isDeclaration()) {
      toClone.push_back(&F);
    }
  }

  // Split into second loop to avoid infine loop by modifying iterator.
  for (auto F : toClone) {
    ValueToValueMapTy VMap;
    auto NewFunc = CloneFunction(F, VMap, nullptr);
    // Emit to .text.clone section
    NewFunc->setSectionPrefix(".clone");
  }

  return true;
}

char CloneFunctions::ID = 0;
static RegisterPass<CloneFunctions> X("clonefuncs",
                                      "Clone Functions in a Module");
