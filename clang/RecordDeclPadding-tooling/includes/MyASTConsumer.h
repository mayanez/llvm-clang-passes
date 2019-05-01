#ifndef __MYASTCONSUMER_H__
#define __MYASTCONSUMER_H__

#include <string>

#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "MyASTVisitor.h"

using namespace llvm;
using namespace clang;


class MyASTConsumer : public ASTConsumer {
public:
  MyASTConsumer(std::string ifn, std::string ofn) : Visitor(ifn, ofn) {}

  void HandleTranslationUnit(ASTContext &Context) override;

private:
  MyASTVisitor Visitor;
};
#endif
