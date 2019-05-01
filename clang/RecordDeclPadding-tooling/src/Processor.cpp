#include "Processor.h"

std::pair<std::string, uint32_t>
Processor::getSourceLocation(const RecordDecl *RD) {

    if (RD) {
      auto &ASTContext = RD->getASTContext();
      FullSourceLoc FullLocation = ASTContext.getFullLoc(RD->getBeginLoc());

      if (FullLocation.isValid() && !FullLocation.isInSystemHeader()) {
        auto pLoc = FullLocation.getPresumedLoc();
        return {pLoc.getFilename(), pLoc.getLine()};
      }
    }

    return {"", 0};
  }

bool Processor::shouldSkipDecl(const RecordDecl *RD) {

    auto SourceLoc = getSourceLocation(RD);

    // Skip any RecordDecls that are defined in the `exclude` paths. (i.e. system libs).
    for (auto &includePath : excludeIncludePaths) {
      if (SourceLoc.first.find(includePath) != std::string::npos) {
        return true;
      }
    }

    if (RD->field_empty()) {
      return true;
    }

    if (RD->isUnion()) {
      return true;
    }

    if (auto *CXXMD = dyn_cast<CXXMethodDecl>(RD)) {
      return true;
    }

    if (auto *CXXRD = dyn_cast<CXXRecordDecl>(RD)) {
      // Can't layout a template, so skip it. We do still layout the
      // instantiations though.
      if (CXXRD->getTypeForDecl()->isDependentType())
        return true;
      if (CXXRD->getTypeForDecl()->isInstantiationDependentType())
        return true;
    }

    auto IsTrickyField = [](const FieldDecl *FD) -> bool {
      // Bitfield layout is hard.
      if (FD->isBitField())
        return true;

      // Variable length arrays are tricky too.
      QualType Ty = FD->getType();
      if (Ty->isIncompleteArrayType())
        return true;
      return false;
    };

    if (std::any_of(RD->field_begin(), RD->field_end(), IsTrickyField)) {
      return true;
    }

    return false;
  }

  std::pair<CharUnits, std::string>
  Processor::calculatePad(const RecordDecl *RD, const ASTContext &ASTContext,
               const ASTRecordLayout &RL) {
    CharUnits PaddingSum;
    CharUnits Offset = ASTContext.toCharUnitsFromBits(RL.getFieldOffset(0));

    std::string Pads = "\"";

    for (const FieldDecl *FD : RD->fields()) {
      // NOTE: This checker only cares about the padded size of the
      // field, and not the data size. If the field is a record
      // with tail padding, then we won't put that number in our
      // total.
      // TODO: Is keeping track of tail padding necessary?
      CharUnits FieldSize = ASTContext.getTypeSizeInChars(FD->getType());
      auto FieldOffsetBits = RL.getFieldOffset(FD->getFieldIndex());
      CharUnits FieldOffset = ASTContext.toCharUnitsFromBits(FieldOffsetBits);
      auto Pad = (FieldOffset - Offset);

      if (!Pad.isZero()) {
        Pads += std::to_string(Pad.getQuantity()) + ",";
      }

      PaddingSum += Pad;
      Offset = FieldOffset + FieldSize;
    }

    auto Pad = (RL.getSize() - Offset);

    if (!Pad.isZero()) {
      Pads += std::to_string(Pad.getQuantity()) + ",";
    }

    PaddingSum += Pad;

    Pads.pop_back();

    if (!Pads.empty()) {
      // Remove trailing comma
      Pads += "\"";
    } else {
      // If no padding in record, set as empty string.
      Pads = "\"\"";
    }

    return {PaddingSum, Pads};
  }

uint32_t Processor::calcNumPointers(const RecordDecl *RD) {

    uint32_t numPointers = 0;

    for (const FieldDecl *FD : RD->fields()) {
      QualType t = FD->getType();
      if (t->isPointerType()) {
        numPointers++;
      }
    }

    return numPointers;
  }

void Processor::process(const RecordDecl *RD, bool IsArray, CharUnits ArraySize) {
    auto &ASTContext = RD->getASTContext();

    if (shouldSkipDecl(RD)) {
      return;
    }

    auto DefRD = RD->getDefinition();

    if (DefRD) {
      // NOTE: The decl represented must be a definition,
      // not a forward declaration to get ASTRecordLayout.
      const ASTRecordLayout &RL = ASTContext.getASTRecordLayout(DefRD);

      std::pair<CharUnits, std::string> PadInfo = calculatePad(DefRD, ASTContext, RL);
      auto TotalSizeof = IsArray ? (RL.getSize().getQuantity() * ArraySize.getQuantity()) : RL.getSize().getQuantity();
      auto TotalPadSize = IsArray ? (PadInfo.first.getQuantity() * ArraySize.getQuantity()) : PadInfo.first.getQuantity();
      auto RecordName = RD->getQualifiedNameAsString();

      if (RecordName.empty()) {
        RecordName = "unknown";
      }

      if (IsArray) {
        // eg. struct a[10];
        RecordName += "[" + std::to_string(ArraySize.getQuantity()) + "]";
      }

      bool isVirtual = false;
      if (auto CXXRD = dyn_cast<CXXRecordDecl>(DefRD)) {
        isVirtual = CXXRD->isPolymorphic();
      }

      OutputFile << RecordName << ","
                << TotalSizeof << ","
                << TotalPadSize << ","
                << std::distance(DefRD->fields().begin(), DefRD->fields().end()) << ","
                << calcNumPointers(DefRD) << ","
                << PadInfo.second << ","
                << isVirtual
                << "\n";
    }
  }

void Processor::process(const VarDecl *VD) {
    const clang::ArrayType *ArrTy = VD->getType()->getAsArrayTypeUnsafe();
    if (ArrTy == nullptr) {
      return;
    }

    if (const clang::ConstantArrayType *CArrTy = dyn_cast<ConstantArrayType>(ArrTy)) {
      auto Elts = CharUnits::fromQuantity(CArrTy->getSize().getZExtValue());

      if (Elts.isZero()) {
        return;
      }

      const RecordType *RT = ArrTy->getElementType()->getAs<RecordType>();
      if (RT == nullptr) {
        return;
      }

      process(RT->getDecl(), true, Elts);
    }
  }
