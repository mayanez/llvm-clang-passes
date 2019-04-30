#ifndef __HELLO_H__
#define __HELLO_H__

#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
using namespace llvm;

#define DEBUG_TYPE "hello"
STATISTIC(HelloCounter, "Counts number of functions greeted");

// Hello - The first implementation, without getAnalysisUsage.
struct Hello : public FunctionPass {
  static char ID; // Pass identification
  Hello() : FunctionPass(ID) {}

  virtual bool runOnFunction(Function &F) override;
};

// Hello2 - The second implementation with getAnalysisUsage implemented.
struct Hello2 : public FunctionPass {
  static char ID; // Pass identification
  Hello2() : FunctionPass(ID) {}

  virtual bool runOnFunction(Function &F) override;

  // We don't modify the program, so we preserve all analyses.
  virtual void getAnalysisUsage(AnalysisUsage &AU) const override;
};
#endif
