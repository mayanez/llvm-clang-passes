#ifndef __CLONEFUNCTIONS_H__
#define __CLONEFUNCTIONS_H__

#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Utils/Cloning.h"
using namespace llvm;

#define DEBUG_TYPE "clonefuncs"

struct CloneFunctions : public ModulePass {
  static char ID;
  CloneFunctions() : ModulePass(ID) {}

  bool runOnModule(Module &M) override;
};
#endif
