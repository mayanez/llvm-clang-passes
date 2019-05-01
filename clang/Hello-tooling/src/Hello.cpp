#include <sstream>
#include <string>

#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/ASTConsumers.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/raw_ostream.h"

#include "MyFrontendAction.h"
#include "MyASTVisitor.h"
#include "MyASTConsumer.h"

using namespace llvm;
using namespace clang;
using namespace clang::driver;
using namespace clang::tooling;

#define QUOTE(name) #name
#define STR(macro) QUOTE(macro)

// NOTE: We support up to 8 Include directories at the moment.
#ifdef CLANG_INC_DIR1
std::vector<std::string> const clangIncDirs = { "-I" STR(CLANG_INC_DIR1),
                                              #ifdef CLANG_INC_DIR2
                                                "-I" STR(CLANG_INC_DIR2),
                                              #endif
                                              #ifdef CLANG_INC_DIR3
                                                "-I" STR(CLANG_INC_DIR3),
                                              #endif
                                              #ifdef CLANG_INC_DIR4
                                                "-I" STR(CLANG_INC_DIR4),
                                              #endif
                                              #ifdef CLANG_INC_DIR5
                                                "-I" STR(CLANG_INC_DIR5),
                                              #endif
                                              #ifdef CLANG_INC_DIR6
                                                "-I" STR(CLANG_INC_DIR6),
                                              #endif
                                              #ifdef CLANG_INC_DIR7
                                                "-I" STR(CLANG_INC_DIR7),
                                              #endif
                                              #ifdef CLANG_INC_DIR8
                                                "-I" STR(CLANG_INC_DIR8)
                                              #endif
};
#else
std::vector<std::string> const clangIncDirs;
#endif

static llvm::cl::OptionCategory ToolingSampleCategory("Tooling Sample");

int main(int argc, const char **argv) {
  CommonOptionsParser op(argc, argv, ToolingSampleCategory);
  ClangTool Tool(op.getCompilations(), op.getSourcePathList());

  // Add Clang internal include directories.
  for (auto i : clangIncDirs) {
    ArgumentsAdjuster ardj = getInsertArgumentAdjuster(i.c_str());
    Tool.appendArgumentsAdjuster(ardj);
  }

  // ClangTool::run accepts a FrontendActionFactory, which is then used to
  // create new objects implementing the FrontendAction interface. Here we use
  // the helper newFrontendActionFactory to create a default factory that will
  // return a new MyFrontendAction object every time.
  // To further customize this, we could create our own factory class.
  return Tool.run(newFrontendActionFactory<MyFrontendAction>().get());
}
