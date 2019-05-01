#include "MyASTVisitor.h"

bool MyASTVisitor::VisitRecordDecl(const RecordDecl *RD) {
    P.process(RD);
    return true;
}

bool MyASTVisitor::VisitVarDecl(const VarDecl *VD) {
    P.process(VD);
    return true;
}
