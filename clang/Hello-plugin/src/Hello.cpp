#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "llvm/Support/raw_ostream.h"

#include "MyASTVisitor.h"
#include "MyASTConsumer.h"
#include "MyPluginASTAction.h"

using namespace clang;
using namespace llvm;

static FrontendPluginRegistry::Add<MyPluginASTAction>
    X("-hello", "Hello World Clang Plugin pass");
