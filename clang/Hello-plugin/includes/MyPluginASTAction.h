#ifndef __MYPLUGINASTACTION_H__
#define __MYPLUGINASTACTION_H__

#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/Frontend/CompilerInstance.h"
#include "llvm/Support/raw_ostream.h"
using namespace clang;
using namespace llvm;


class MyPluginASTAction : public PluginASTAction {
protected:
  virtual std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 llvm::StringRef) override;
  virtual bool ParseArgs(const CompilerInstance &CI,
                 const std::vector<std::string> &args) override;

  virtual void PrintHelp(llvm::raw_ostream &ros);
};

#endif
