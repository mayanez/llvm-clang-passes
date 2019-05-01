#include "MyPluginASTAction.h"
#include "MyASTConsumer.h"

std::unique_ptr<ASTConsumer> MyPluginASTAction::CreateASTConsumer(CompilerInstance &CI,
                                               llvm::StringRef) {
  return llvm::make_unique<MyASTConsumer>();
}

bool MyPluginASTAction::ParseArgs(const CompilerInstance &CI,
               const std::vector<std::string> &args) {
  for (unsigned i = 0, e = args.size(); i != e; ++i) {
    llvm::errs() << "Hello arg = " << args[i] << "\n";
  }

  if (!args.empty() && args[0] == "help")
    PrintHelp(llvm::errs());

  return true;
}

void MyPluginASTAction::PrintHelp(llvm::raw_ostream &ros) {
  ros << "A helpful help message\n";
}
