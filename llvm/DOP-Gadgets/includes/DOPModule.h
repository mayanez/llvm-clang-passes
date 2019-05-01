#pragma once

#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/Pass.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/InstVisitor.h"
#include "llvm/IR/DebugInfo.h"

#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/CommandLine.h"

#include "llvm/Analysis/MemoryDependenceAnalysis.h"
#include "llvm/IR/Dominators.h"

#include "llvm/IR/Operator.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/Analysis/CallGraph.h"

#include "llvm/Analysis/LoopPass.h" 

#include "llvm-c/Core.h"

#include <vector>
#include <set>
#include <iostream>
#include <fstream>
#include <string>

namespace llvm {
    enum GadgetScope
    {
        GLOBAL,
        LOCAL,
        MIXED
    };

    enum ResultType
    {
        GLOBAL_VAR = 1,
        ARGUMENT_ANY = 2,
        ARGUMENT_PTR = 4,
        FUNCTION_PTR = 8
    };

    enum GadgetType
    {
        DEREF = 1,
        ARITHMETIC = 2,
        LOGICAL = 4,
        MOV = 8,
        CONDITIONAL = 16
    };

    typedef std::pair<ResultType, Value*> value_pair_t;
    typedef std::pair<std::vector<Value *>, value_pair_t> inst_pair_t;

    typedef std::pair<Function *, Instruction *>  dyn_pair_t;

    typedef struct ResultVal
    {
        Value *V;
        unsigned int resultSet = 0;
        unsigned int gadgetSet = 0;
        std::vector<inst_pair_t> instList;
    } ResultVal_t;

    typedef struct StatGadgetType
    {
        unsigned int global = 0;
        unsigned int local = 0;
        unsigned int mixed = 0;
    } StatGadgetType_t;

    class InstInst
    {
    public:
            bool isPointerToPointer(const Value *);
            StoreInst *inst;
            bool interesting = false;
            ResultVal_t ptr;
            ResultVal_t value;

            void prepResult(ResultVal_t &);

            void printGadget(ResultVal_t &);
            void printResult(ResultVal_t &);
            InstInst();
            InstInst(StoreInst *);
            InstInst(const InstInst &);
            InstInst& operator=(const InstInst&);
            bool IsInteresting();

            ResultVal_t getPtrSet();
            ResultVal_t getValueSet();
            void Print();
    };

    class Backtrack : public InstVisitor<Backtrack>
    {
        private:
            bool result = false;
            bool isPointer;
            Loop *L;

            Value *caller_inst;

            std::set<Value *>visited;
            std::vector<Value *>worklist;
            std::vector<Value *>worklist_caller;
            std::vector<Value *>instruction;
            std::vector<inst_pair_t>instList;
        public:
            Backtrack(Value *, Loop *, bool, Value *);
            void visitLoadInst(LoadInst &I);
            void visitGetElementPtrInst(GetElementPtrInst &I);
            void visitBitCastInst(BitCastInst &I);
            void visitPHINode(PHINode &I);
            void visitCallInst(CallInst &I);
            void visitUnaryInstruction(UnaryInstruction &I);
            void visitInstruction(Instruction &I);
            void visitBinaryOperator(BinaryOperator &I);
            void visitSelectInst(SelectInst &I);
            void visitCmpInst(CmpInst &I);
            void visitInsertElementInst(InsertElementInst &I);
            void visitExtractElementInst(ExtractElementInst &I);
            void visitShuffleVectorInst(ShuffleVectorInst &I);

            bool isInteresting();

            std::vector<inst_pair_t> GetInstList();
            void PrintInstructionStack();
            // for debugging
            void setCallerInst(Value *me) { caller_inst = me; };
            /*
            void visitStoreInst(StoreInst &I);
            void visitBranchInst(BranchInst &I);
            void visitIndirectBrInst(IndirectBrInst &I);
            void visitBinaryOperator(BinaryOperator &I);
            void visitICmpInst(ICmpInst &I);
            void visitFCmpInst(FCmpInst &I);
            */
    };

    class DOPModule : public ModulePass
    {
        private:
            std::set<Value *>visited;
            std::map<StoreInst *, InstInst> result;
            MemoryDependenceAnalysis *MDA;
            AliasAnalysis *AA;
            LoopInfo *LA;
            std::set<GlobalVariable *> gvs;
            std::set<Value*> getSrcPtr(Value *);
            void checkFunction(Function *F);

            std::set<dyn_pair_t> dyn_map;

            CallGraph *CGP;

            StatGadgetType_t mov_stats;
            StatGadgetType_t arithmetic_stats;
            StatGadgetType_t deref_stats;
            StatGadgetType_t logical_stats;
            StatGadgetType_t conditional_stats;

            std::vector<std::string> functionList;
            std::vector<Loop *> loopList;

            void transitiveClosure(Module &M);

            void printStat(StatGadgetType_t &);
            void incStat(StatGadgetType_t &, GadgetScope);
            void printStats();
            void printDetail();
            void printSource();
            void sourceHelper(ResultVal_t &, ResultVal_t &, InstInst &);
            bool checkFunction(StringRef functionName);
            bool isInsnReachable(Instruction *IFrom, Instruction *ITo);
            bool isFunctionReachable(Function *FFrom, Function *FTo);
            GadgetScope getGadgetScope(ResultVal_t &, ResultVal_t &);
        public:
            int getBBIndex(BasicBlock *B);
            static char ID;
            DOPModule() : ModulePass(ID) {}
            void getAnalysisUsage(AnalysisUsage &Info) const override;
            bool runOnModule(Module &M) override;
    };
}
