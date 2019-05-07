#ifndef __STATISTICS_H__
#define __STATISTICS_H__

#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
using namespace llvm;

#define GET_VARIABLE_NAME(Variable) (#Variable)

#define DEBUG_TYPE "mystats"
STATISTIC(TotalPointerCounter, "Number of Pointers");
STATISTIC(FunctionPointerCounter, "Number of Function Pointers");
STATISTIC(CallCounter, "Number of Calls");
STATISTIC(IndirectCallCounter, "Number of Indirect Calls");
STATISTIC(StoreInstructionCounter, "Number of Stores");

struct StatOut {
  uint64_t totalPointers = 0;
  uint64_t functionPointers = 0;
  uint64_t calls = 0;
  uint64_t indirectCalls = 0;
  uint64_t storeInstructions = 0;

  friend llvm::raw_ostream& operator<< (llvm::raw_ostream& stream, const StatOut &so) {
    stream << GET_VARIABLE_NAME(totalPointers) << ","
           << GET_VARIABLE_NAME(functionPointers) << ","
           << GET_VARIABLE_NAME(calls) << ","
           << GET_VARIABLE_NAME(indirectCalls) << ","
           << GET_VARIABLE_NAME(storeInstructions) <<
           "\n";

    stream << so.totalPointers << ","
           << so.functionPointers << ","
           << so.calls << ","
           << so.indirectCalls << ","
           << so.storeInstructions <<
           "\n";
    return stream;
  }
};

struct Statistics : public ModulePass {
  static char ID; // Pass identification
  Statistics() : ModulePass(ID) {}

  virtual bool runOnModule(Module &M) override;

  virtual bool isFunctionPointerType(Type *type);
  virtual void print();
};
#endif
