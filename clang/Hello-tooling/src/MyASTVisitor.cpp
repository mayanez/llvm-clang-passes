#include "MyASTVisitor.h"

bool MyASTVisitor::VisitFunctionDecl(FunctionDecl *FD) {
  // Only function definitions (with bodies), not declarations.
  if (FD->hasBody()) {

    // Function name
    DeclarationName DeclName = FD->getNameInfo().getName();
    std::string FuncName = DeclName.getAsString();

    errs() << "Hello: ";
    errs().write_escaped(FuncName) << '\n';
  }

  return true;
}
