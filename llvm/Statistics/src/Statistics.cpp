#include "Statistics.h"
#include "llvm/IR/Instructions.h"
using namespace llvm;

// Ref: https://stackoverflow.com/questions/48333206/how-to-check-if-a-target-of-an-llvm-allocainst-is-a-function-pointer
bool Statistics::isFunctionPointerType(Type *T){
  if(auto pointerType = dyn_cast<PointerType>(T)){
    return isFunctionPointerType(pointerType->getElementType());
  }
  else if(T->isFunctionTy()){
    return  true;
  }
  return false;
}

void Statistics::print() {
  StatOut o;
  o.totalPointers = TotalPointerCounter;
  o.functionPointers = FunctionPointerCounter;
  o.calls = CallCounter;
  o.indirectCalls = IndirectCallCounter;
  o.storeInstructions = StoreInstructionCounter;
  llvm::errs() << o;
}

bool Statistics::runOnModule(Module &M) {

  for (auto &G : M.globals()) {
    auto T = G.getType();
    if (T->isPointerTy()) {
      ++TotalPointerCounter;
    }

    if (isFunctionPointerType(T)){
      ++FunctionPointerCounter;
    }
  }

  for (auto &F : M) {
    for (auto &BB : F) {
      for (auto &I : BB) {
        if (auto allocaInst = dyn_cast<AllocaInst>(&I)){
          auto T = allocaInst->getAllocatedType();

          if (T->isPointerTy()) {
            ++TotalPointerCounter;
          }

          if (isFunctionPointerType(T)){
            ++FunctionPointerCounter;
          }
        } else if (auto CI = dyn_cast<CallInst>(&I)) {
          ++CallCounter;
          if (CI->isIndirectCall()) {
            // NOTE: It is unclear whether this captures all casses.
            // https://lists.llvm.org/pipermail/llvm-dev/2018-August/125098.html
            // https://stackoverflow.com/questions/50761292/find-all-function-pointers-in-llvm-ir
            ++IndirectCallCounter;
          }
        } else if (llvm::isa<StoreInst>(&I)) {
          ++StoreInstructionCounter;
        }
      }
    }
  }

  print();
  return false;
}

char Statistics::ID = 0;
static RegisterPass<Statistics> X("mystats", "My Statistics Pass");
