#ifndef __MYFRONTENDACTION_H__
#define __MYFRONTENDACTION_H__

#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/Frontend/ASTConsumers.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendActions.h"

using namespace llvm;
using namespace clang;

class MyFrontendAction : public ASTFrontendAction {
public:
  MyFrontendAction() {}

  virtual std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 StringRef file) override;
};

#endif
