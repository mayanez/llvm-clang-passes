#include "MyFrontendAction.h"
#include "MyASTConsumer.h"

// For each source file provided to the tool, a new FrontendAction is created.
std::unique_ptr<ASTConsumer> MyFrontendAction::CreateASTConsumer(CompilerInstance &CI,
                                                StringRef file) {
  return llvm::make_unique<MyASTConsumer>();
}
