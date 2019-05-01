#ifndef __MYASTVISITOR_H__
#define __MYASTVISITOR_H__

#include "clang/AST/AST.h"
#include "clang/AST/RecursiveASTVisitor.h"
using namespace llvm;
using namespace clang;

// By implementing RecursiveASTVisitor, we can specify which AST nodes
// we're interested in by overriding relevant methods.
class MyASTVisitor : public RecursiveASTVisitor<MyASTVisitor> {
public:
  MyASTVisitor() {}

  virtual bool VisitFunctionDecl(FunctionDecl *f);
};

#endif
