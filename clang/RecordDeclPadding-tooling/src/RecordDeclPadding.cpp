#include <experimental/filesystem>
#include <fstream>
#include <iterator>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/Frontend/ASTConsumers.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/ArgumentsAdjusters.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/CompilationDatabase.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"

#include "Processor.h"
#include "MyASTVisitor.h"
#include "MyASTConsumer.h"

#define OUTPUT_FILE_EXTENSION ".padinfo"

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

using namespace clang;
using namespace clang::driver;
using namespace clang::tooling;
using namespace llvm;
using namespace ento;

static llvm::cl::OptionCategory PaddingCheckCategory("Calculate Padding");

static std::string OutputFilePath;

class MyFrontendAction : public ASTFrontendAction {
public:
  MyFrontendAction() {}

  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 StringRef file) override {
    llvm::errs() << "** Creating AST consumer for: " << file << "\n";

    auto oFPath = OutputFilePath + "/" + std::string(file);
    auto dir = std::experimental::filesystem::path(oFPath).remove_filename();
    std::experimental::filesystem::create_directories(dir);

    std::string OutputFileName = oFPath + OUTPUT_FILE_EXTENSION;

    return llvm::make_unique<MyASTConsumer>(file, OutputFileName);
  }
};

int main(int argc, const char **argv) {

  OutputFilePath = std::experimental::filesystem::current_path();

  CommonOptionsParser op(argc, argv, PaddingCheckCategory);
  ClangTool Tool(op.getCompilations(), op.getSourcePathList());

  llvm::errs() << "Include Directories:\n";

  for (auto i : clangIncDirs) {
    llvm::errs() << i << "\n";
    ArgumentsAdjuster ardj = getInsertArgumentAdjuster(i.c_str());
    Tool.appendArgumentsAdjuster(ardj);
  }

  return Tool.run(newFrontendActionFactory<MyFrontendAction>().get());
}
