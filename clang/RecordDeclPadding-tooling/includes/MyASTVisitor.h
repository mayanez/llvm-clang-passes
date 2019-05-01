#ifndef __MYASTVISITOR_H__
#define __MYASTVISITOR_H__

#include "clang/AST/AST.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "Processor.h"

using namespace llvm;
using namespace clang;

class MyASTVisitor : public RecursiveASTVisitor<MyASTVisitor> {
public:
  MyASTVisitor(std::string ifn, std::string ofn) : P(ifn, ofn) {}

  virtual bool VisitRecordDecl(const RecordDecl *RD);
  virtual bool VisitVarDecl(const VarDecl *VD);

private:
  Processor P;
};

#endif
