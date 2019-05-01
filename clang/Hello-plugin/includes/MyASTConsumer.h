#ifndef __MYASTCONSUMER_H__
#define __MYASTCONSUMER_H__

#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "MyASTVisitor.h"

using namespace llvm;
using namespace clang;

class MyASTConsumer : public ASTConsumer {
public:
  MyASTConsumer() {}

  void HandleTranslationUnit(ASTContext& Context) override;

private:
  MyASTVisitor Visitor;
};
#endif
