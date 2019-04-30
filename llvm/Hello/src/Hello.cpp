#include "Hello.h"
using namespace llvm;

bool Hello::runOnFunction(Function &F) {
  ++HelloCounter;
  errs() << "Hello: ";
  errs().write_escaped(F.getName()) << '\n';
  return false;
}

char Hello::ID = 0;
static RegisterPass<Hello> X("myhello", "Hello World Pass");

bool Hello2::runOnFunction(Function &F) {
  ++HelloCounter;
  errs() << "Hello: ";
  errs().write_escaped(F.getName()) << '\n';
  return false;
}

// We don't modify the program, so we preserve all analyses.
void Hello2::getAnalysisUsage(AnalysisUsage &AU) const {
  AU.setPreservesAll();
}

char Hello2::ID = 0;
static RegisterPass<Hello2>
Y("myhello2", "Hello World Pass (with getAnalysisUsage implemented)");
