#include "MyASTConsumer.h"

void MyASTConsumer::HandleTranslationUnit(ASTContext &Context) {
  auto D = Context.getTranslationUnitDecl();
  Visitor.TraverseDecl(D);
}
