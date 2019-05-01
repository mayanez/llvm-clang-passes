#ifndef __PROCESSOR_H__
#define __PROCESSOR_H__

#include <string>
#include <sstream>
#include <fstream>
#include <vector>

#include "clang/AST/AST.h"
#include "clang/AST/RecordLayout.h"
#include "clang/AST/CharUnits.h"

using namespace llvm;
using namespace clang;

static const std::string CSV_HEADER = "RecordName,Sizeof,TotalPadSize,NumFields,NumPointers,Pads,isVirtual";

class Processor {
public:
  Processor(std::string ifn, std::string ofn) : InputFileName(ifn), OutputFile(ofn) {
    OutputFile << CSV_HEADER
               << "\n";
  }

  void process(const RecordDecl *RD, bool IsArray = false, CharUnits ArraySize = CharUnits::Zero());
  void process(const VarDecl *VD);

private:
  std::string InputFileName;
  std::ofstream OutputFile;

  // NOTE: We assume system libraries are in "/usr".
  std::vector<std::string> excludeIncludePaths = {"/usr/"};

  std::pair<std::string, uint32_t>
  getSourceLocation(const RecordDecl *RD);

  bool shouldSkipDecl(const RecordDecl *RD);

  std::pair<CharUnits, std::string>
  calculatePad(const RecordDecl *RD, const ASTContext &ASTContext,
               const ASTRecordLayout &RL);

  uint32_t calcNumPointers(const RecordDecl *RD);
};

#endif
