#include "DOPModule.h"

using namespace llvm;

cl::opt<std::string> dop_dyn("dop-dyn", cl::init(""),
                             cl::desc("Dynamic dispatch loops."),
                             cl::value_desc("filename"));

cl::opt<std::string>
    vuln_path("vuln-list", cl::init(""),
              cl::desc("File containing list of vulnerable function."),
              cl::value_desc("filename"));

cl::opt<std::string>
    FL_path("function-list", cl::init(""),
            cl::desc("File containing list of functions to ignore."),
            cl::value_desc("filename"));

cl::opt<bool> dop_detail("details", cl::init(false),
                         cl::desc("Display details of interesting gadgets"),
                         cl::value_desc("true/false"));

cl::opt<bool> dop_source("source", cl::init(false),
                         cl::desc("Output Source mapping of IR instruction."),
                         cl::value_desc("true/false"));

cl::opt<bool> dop_danger("danger", cl::init(false),
                         cl::desc("Dangerous functionalities."),
                         cl::value_desc("true/false"));

Backtrack::Backtrack(Value *V, Loop *L, bool type, Value *VS) {
  worklist_caller.push_back(VS);
  worklist.push_back(V);
  this->L = L;
  this->isPointer = type;
}

void Backtrack::visitLoadInst(LoadInst &I) {
  worklist_caller.push_back(&I);
  worklist.push_back(I.getPointerOperand());
}

void Backtrack::visitGetElementPtrInst(GetElementPtrInst &I) {
  worklist_caller.push_back(&I);
  worklist.push_back(I.getPointerOperand());
}

void Backtrack::visitBitCastInst(BitCastInst &I) {
  worklist_caller.push_back(&I);
  worklist.push_back(I.getOperand(0));
}

void Backtrack::visitPHINode(PHINode &I) {
  for (unsigned int x = 0; x < I.getNumIncomingValues(); x++) {
    worklist_caller.push_back(&I);
    worklist.push_back(I.getIncomingValue(x));
  }
}

void Backtrack::visitBinaryOperator(BinaryOperator &I) {
  for (User::op_iterator opi = I.op_begin(); opi != I.op_end(); opi++) {
    worklist_caller.push_back(&I);
    worklist.push_back(*opi);
  }
}

void Backtrack::visitCallInst(CallInst &I) {}

void Backtrack::visitUnaryInstruction(UnaryInstruction &I) {
  for (User::op_iterator opi = I.op_begin(); opi != I.op_end(); opi++) {
    worklist_caller.push_back(&I);
    worklist.push_back(*opi);
  }
}

void Backtrack::visitSelectInst(SelectInst &I) {
  worklist_caller.push_back(&I);
  worklist.push_back(I.getTrueValue());
  worklist_caller.push_back(&I);
  worklist.push_back(I.getFalseValue());
}

void Backtrack::visitInstruction(Instruction &I) {
  // errs() << "INSTRUCTION IN BACKTRACK:" << I << "\n";
  exit(1);
}

void Backtrack::visitCmpInst(CmpInst &I) {
  for (User::op_iterator opi = I.op_begin(); opi != I.op_end(); opi++) {
    worklist_caller.push_back(&I);
    worklist.push_back(*opi);
  }
}

void Backtrack::visitInsertElementInst(InsertElementInst &I) {
  for (User::op_iterator opi = I.op_begin(); opi != I.op_end(); opi++) {
    worklist_caller.push_back(&I);
    worklist.push_back(*opi);
  }
}

void Backtrack::visitExtractElementInst(ExtractElementInst &I) {
  for (User::op_iterator opi = I.op_begin(); opi != I.op_end(); opi++) {
    worklist_caller.push_back(&I);
    worklist.push_back(*opi);
  }
}

void Backtrack::visitShuffleVectorInst(ShuffleVectorInst &I) {
  for (User::op_iterator opi = I.op_begin(); opi != I.op_end(); opi++) {
    worklist_caller.push_back(&I);
    worklist.push_back(*opi);
  }
}

void Backtrack::PrintInstructionStack() {
  /*
  for (auto inst_pair: instList)
  {
      errs() << "TYPE:" << inst_pair.second.first << " : ";
      errs() << *inst_pair.second.second << "\n";
      if (inst_pair.first.size())
          errs() << "INSTRUCTION STACK\n";
      for (auto tmpV=inst_pair.first.rbegin(); tmpV!=inst_pair.first.rend();
  tmpV++)
      {
          errs() << **tmpV << "\n";
      }
  }
  */
}

std::vector<inst_pair_t> Backtrack::GetInstList() { return this->instList; }

bool Backtrack::isInteresting() {
  bool a_load = false;

  // iterate through the worklist until it is empty
  while (!worklist.empty()) {
    Value *ptr = worklist.back();
    worklist.pop_back();
    Value *ptr_caller = worklist_caller.back();
    worklist_caller.pop_back();

    if (visited.find(ptr) != visited.end())
      continue;
    visited.insert(ptr);

    // skip null pointers...
    if (ptr == nullptr)
      continue;

    // we'll check if the value is an instruction or a value
    if (Instruction *I = dyn_cast<Instruction>(ptr)) {
      // recover the instruction stack up till the ptr
      while (!instruction.empty() && instruction.back() != ptr_caller)
        instruction.pop_back();
      instruction.push_back(I);
      if (dyn_cast<LoadInst>(I)) {
        a_load = true;
      }

      visit(*I);
    } else if (dyn_cast<Constant>(ptr)) {
      // it must be a constant value...
      if (ConstantExpr *CE = dyn_cast<ConstantExpr>(ptr)) {
        switch (CE->getOpcode()) {
        case Instruction::GetElementPtr:
        case Instruction::BitCast:
        case Instruction::PtrToInt:
        case Instruction::IntToPtr:
          worklist.push_back(CE->getOperand(0));
          worklist_caller.push_back(ptr);
          break;
        case Instruction::Add:
        case Instruction::Sub:
          for (User::op_iterator opi = CE->op_begin(); opi != CE->op_end();
               opi++) {
            worklist_caller.push_back(CE);
            worklist.push_back(*opi);
          }
          break;
        default:
          // errs() << "UNKNOWN CE:" << *CE << "\n";
          exit(1);
        }
      } else if (dyn_cast<GlobalVariable>(ptr)) {
        // errs() << "FOUND Global:" << *GV << "\n";
        instList.push_back(
            inst_pair_t(instruction, value_pair_t(GLOBAL_VAR, ptr)));
        result = true;
      } else if (dyn_cast<ConstantInt>(ptr)) {
        // errs() << "FOUND CONSTANTINT:" << *GV << "\n";
        if (a_load)
          result = true;
      } else if (dyn_cast<ConstantPointerNull>(ptr)) {
        // errs() << "FOUND CONSTANT NULL:" << *ptr << "\n";
      } else if (dyn_cast<ConstantStruct>(ptr)) {
        // errs() << "FOUND CONSTANT STRUCT:" << *ptr << "\n";
      } else if (dyn_cast<ConstantArray>(ptr)) {
      } else if (dyn_cast<Function>(ptr)) {
        instList.push_back(
            inst_pair_t(instruction, value_pair_t(FUNCTION_PTR, ptr)));
        result = true;
      } else if (dyn_cast<ConstantFP>(ptr)) {
      } else if (dyn_cast<ConstantVector>(ptr)) {
      } else if (dyn_cast<ConstantDataVector>(ptr)) {
      } else if (dyn_cast<ConstantArray>(ptr)) {
      } else if (dyn_cast<UndefValue>(ptr)) {
      } else {
        // errs() << *caller_inst << "\n";
        // errs() << "UNKNOWN VALUE:" << *ptr << "\n";
        // exit(1);
      }
    } else if (dyn_cast<Operator>(ptr)) {
      // it is an operator
      // errs() << "FOUND OPERATOR:" << *ptr << "\n";
    }
    /*
    else if (dyn_cast<User>(ptr))
    {
        errs() << "FOUND USER:" << *ptr << "\n";
    }
    */
    else if (Argument *A = dyn_cast<Argument>(ptr)) {
      // TODO: possible to check all callsites of the function and look for
      // interesting pointers...

      if (isPointer) {
        // pointer used by storeinst, only interesting if it is a pointer
        if (A->getType()->isPointerTy()) {
          instList.push_back(
              inst_pair_t(instruction, value_pair_t(ARGUMENT_PTR, A)));
          result = true;
        }
      } else {
        // value of storeinst, even if it is passbyval also interesting
        instList.push_back(
            inst_pair_t(instruction, value_pair_t(ARGUMENT_ANY, A)));
        result = true;
      }
    } else {
      // not supposed to be here
      errs() << "NOT SUPPOSED TO BE HERE!\n";
      exit(1);
    }
  }
  return result;
}

void InstInst::prepResult(ResultVal_t &target) {
  for (inst_pair_t inst_pair : target.instList) {
    target.resultSet |= inst_pair.second.first;
    for (auto tmpV = inst_pair.first.rbegin(); tmpV != inst_pair.first.rend();
         tmpV++) {

      if (LoadInst *LI = dyn_cast<LoadInst>(*tmpV)) {
        Value *addr = LI->getPointerOperand();
        if (addr->getType()->isPointerTy()) {
          if (isPointerToPointer(addr))
            target.gadgetSet |= DEREF;
          else
            target.gadgetSet |= MOV;
        }
      } else if (BinaryOperator *BO = dyn_cast<BinaryOperator>(*tmpV)) {
        Instruction::BinaryOps op = BO->getOpcode();
        // based on llvm/IR/Instruction.def
        if (op >= Instruction::Add && op <= Instruction::FRem)
          target.gadgetSet |= ARITHMETIC;
        else if (op >= Instruction::Shl && op <= Instruction::Xor)
          target.gadgetSet |= LOGICAL;
        else {
          // errs() << "UNKNOWN BINARYOP:" << *BO << "\n";
          // errs() << op << "\n";
          exit(1);
        }
      } else if (dyn_cast<GetElementPtrInst>(*tmpV)) {
      } else if (dyn_cast<BitCastInst>(*tmpV)) {
      } else if (dyn_cast<PHINode>(*tmpV)) {
      } else if (dyn_cast<TruncInst>(*tmpV)) {
      } else if (dyn_cast<CastInst>(*tmpV)) {
      } else if (dyn_cast<SelectInst>(*tmpV)) {
        target.gadgetSet |= CONDITIONAL;
      } else if (dyn_cast<CmpInst>(*tmpV)) {
        target.gadgetSet |= CONDITIONAL;
      } else if (dyn_cast<ICmpInst>(*tmpV)) {
        target.gadgetSet |= CONDITIONAL;
      } else if (dyn_cast<SwitchInst>(*tmpV)) {
        target.gadgetSet |= CONDITIONAL;
      } else if (dyn_cast<InsertElementInst>(*tmpV)) {
      } else if (dyn_cast<CallInst>(*tmpV)) {
        // errs() << "DUNNOE HOW TO HANDLE RECURSION! SKIPPING!\n";
      } else if (dyn_cast<ExtractElementInst>(*tmpV)) {
      } else if (dyn_cast<ShuffleVectorInst>(*tmpV)) {
      } else if (dyn_cast<AllocaInst>(*tmpV)) {
      } else {
        // errs() << "ANALYSIS UNKNOWN:" << **tmpV << "\n";
        // Print();
        exit(1);
      }
    }
  }
}

InstInst::InstInst() {}

InstInst::InstInst(StoreInst *SI) {
  this->inst = SI;

  // here, we'll get both the destination and source of the store.
  this->ptr.V = SI->getPointerOperand();
  this->value.V = SI->getValueOperand();

  Backtrack BTD(this->ptr.V, nullptr, true, SI);
  BTD.setCallerInst(SI);
  Backtrack BTS(this->value.V, nullptr, false, SI);
  BTS.setCallerInst(SI);

  // check if the destination is an interesting variable
  if (BTD.isInteresting() && BTS.isInteresting()) {
    interesting = true;
    this->ptr.instList = BTD.GetInstList();
    this->value.instList = BTS.GetInstList();

    prepResult(ptr);
    prepResult(value);
  }
}

void InstInst::printResult(ResultVal_t &target) {
  errs() << "[ ";
  if (target.resultSet & GLOBAL_VAR) {
    errs() << "GLOBAL_VAR ";
  }
  if (target.resultSet & ARGUMENT_ANY) {
    errs() << "ARGUMENT_ANY ";
  }
  if (target.resultSet & ARGUMENT_PTR) {
    errs() << "ARGUMENT_PTR ";
  }
  if (target.resultSet & FUNCTION_PTR) {
    errs() << "FUNCTION_PTR ";
  }
  errs() << " ]";
}

void InstInst::printGadget(ResultVal_t &target) {
  if (target.gadgetSet & DEREF) {
    errs() << "DEREF ";
  }
  if (target.gadgetSet & ARITHMETIC) {
    errs() << "ARITHMETIC ";
  }
  if (target.gadgetSet & LOGICAL) {
    errs() << "LOGICAL ";
  }
  if (target.gadgetSet & MOV) {
    errs() << "MOV ";
  }
  if (target.gadgetSet & CONDITIONAL) {
    errs() << "CONDITIONAL";
  }
}

InstInst::InstInst(const InstInst &II) {
  this->inst = II.inst;
  this->interesting = II.interesting;
  this->ptr.V = II.ptr.V;
  this->ptr.instList = II.ptr.instList;
  this->ptr.resultSet = II.ptr.resultSet;
  this->ptr.gadgetSet = II.ptr.gadgetSet;
  this->value.V = II.value.V;
  this->value.instList = II.value.instList;
  this->value.resultSet = II.value.resultSet;
  this->value.gadgetSet = II.value.gadgetSet;
}

InstInst &InstInst::operator=(const InstInst &II) {
  this->inst = II.inst;
  this->interesting = II.interesting;
  this->ptr.V = II.ptr.V;
  this->ptr.instList = II.ptr.instList;
  this->ptr.resultSet = II.ptr.resultSet;
  this->ptr.gadgetSet = II.ptr.gadgetSet;
  this->value.V = II.value.V;
  this->value.instList = II.value.instList;
  this->value.resultSet = II.value.resultSet;
  this->value.gadgetSet = II.value.gadgetSet;

  return *this;
}

bool InstInst::IsInteresting() { return interesting; }

bool InstInst::isPointerToPointer(const Value *V) {
  const Type *T = V->getType();
  return T->isPointerTy() && T->getContainedType(0)->isPointerTy();
}

ResultVal_t InstInst::getPtrSet() { return ptr; }

ResultVal_t InstInst::getValueSet() { return value; }

void InstInst::Print() {
  errs() << "INTERESTING:\t" << *inst << "\n";
  errs() << "FUNCTION:\t" << inst->getParent()->getParent()->getName() << "\n";
  printResult(value);
  errs() << "\t -> \t";
  printResult(ptr);
  errs() << "\n";
  errs() << "VALUE GADGET:\t";
  printGadget(value);
  errs() << "\n";
  errs() << "\n";
  errs() << "\n";

  for (auto inst_pair : ptr.instList) {
    errs() << "TYPE:" << inst_pair.second.first << " : ";
    errs() << *inst_pair.second.second << "\n";
    if (inst_pair.first.size())
      errs() << "INSTRUCTION STACK\n";
    for (auto tmpV = inst_pair.first.rbegin(); tmpV != inst_pair.first.rend();
         tmpV++) {
      errs() << **tmpV << "\n";
    }
  }
  for (auto inst_pair : value.instList) {
    errs() << "TYPE:" << inst_pair.second.first << " : ";
    errs() << *inst_pair.second.second << "\n";
    if (inst_pair.first.size())
      errs() << "INSTRUCTION STACK\n";
    for (auto tmpV = inst_pair.first.rbegin(); tmpV != inst_pair.first.rend();
         tmpV++) {
      errs() << **tmpV << "\n";
    }
  }
}

void DOPModule::incStat(StatGadgetType_t &target, GadgetScope scope) {
  switch (scope) {
  case GLOBAL:
    target.global++;
    break;
  case LOCAL:
    target.local++;
    break;
  case MIXED:
    target.mixed++;
    break;
  }
}
void DOPModule::printStat(StatGadgetType_t &target) {
  errs() << target.global << ","
         << target.mixed << ","
         << target.local;
}

void DOPModule::printDetail() {
  for (auto ResPair : result) {
    ResPair.second.Print();
  }
}

GadgetScope DOPModule::getGadgetScope(ResultVal_t &ptr, ResultVal_t &value) {
  GadgetScope scope;

  if (ptr.resultSet & GLOBAL_VAR || ptr.resultSet & FUNCTION_PTR) {
    if (value.resultSet & GLOBAL_VAR || value.resultSet & FUNCTION_PTR) {
      scope = GLOBAL;
    } else {
      scope = MIXED;
    }
  } else if (value.resultSet & GLOBAL_VAR || value.resultSet & FUNCTION_PTR) {
    scope = MIXED;
  } else {
    scope = LOCAL;
  }

  return scope;
}

void DOPModule::sourceHelper(ResultVal_t &ptr, ResultVal_t &value,
                             InstInst &I) {

  static const char *GadgetScopeStrings[] = {"global", "local", "mixed"};

  errs() << ",";
  GadgetScope scope = getGadgetScope(ptr, value);
  errs() << GadgetScopeStrings[scope] << ",";

  if (DILocation *Loc = I.inst->getDebugLoc()) {
    unsigned Line = Loc->getLine();
    StringRef File = Loc->getFilename();
    StringRef Dir = Loc->getDirectory();

    errs() << Dir << "/" << File << "," << Line << "\n";
  }
}

void DOPModule::printSource() {

  for (auto ResPair : result) {

    ResultVal_t ptr = ResPair.second.getPtrSet();
    ResultVal_t value = ResPair.second.getValueSet();

    if (value.gadgetSet & DEREF) {
      errs() << "deref";
      sourceHelper(ptr, value, ResPair.second);
    }
    if (value.gadgetSet & ARITHMETIC) {
      errs() << "arithmetic";
      sourceHelper(ptr, value, ResPair.second);
    }
    if (value.gadgetSet & LOGICAL) {
      errs() << "logical";
      sourceHelper(ptr, value, ResPair.second);
    }
    if (value.gadgetSet & CONDITIONAL) {
      errs() << "conditional";
      sourceHelper(ptr, value, ResPair.second);
    }
    if (value.gadgetSet == 8 || value.gadgetSet == 0) {
      errs() << "mov";
      sourceHelper(ptr, value, ResPair.second);
    }
  }
}

void DOPModule::printStats() {

  for (auto ResPair : result) {
    ResultVal_t ptr = ResPair.second.getPtrSet();
    ResultVal_t value = ResPair.second.getValueSet();

    GadgetScope scope;

    if (ptr.resultSet & GLOBAL_VAR || ptr.resultSet & FUNCTION_PTR) {
      if (value.resultSet & GLOBAL_VAR || value.resultSet & FUNCTION_PTR) {
        scope = GLOBAL;
      } else {
        scope = MIXED;
      }
    } else if (value.resultSet & GLOBAL_VAR || value.resultSet & FUNCTION_PTR) {
      scope = MIXED;
    } else {
      scope = LOCAL;
    }

    // get the gadget type and add it to the stats list
    if (value.gadgetSet & DEREF) {
      incStat(deref_stats, scope);
    }
    if (value.gadgetSet & ARITHMETIC) {
      incStat(arithmetic_stats, scope);
    }
    if (value.gadgetSet & LOGICAL) {
      incStat(logical_stats, scope);
    }
    if (value.gadgetSet & CONDITIONAL) {
      incStat(conditional_stats, scope);
    }
    if (value.gadgetSet == 8 || value.gadgetSet == 0) {
      incStat(mov_stats, scope);
    }
  }

  // TODO: Print as CSV
  printStat(mov_stats);
  errs() << ",";
  printStat(arithmetic_stats);
  errs() << ",";
  printStat(deref_stats);
  errs() << ",";
  printStat(logical_stats);
  errs() << ",";
  printStat(conditional_stats);
  errs() << "\n";
}

void DOPModule::checkFunction(Function *F) {}

bool DOPModule::checkFunction(StringRef functionName) {
  if (functionList.size()) {
    for (std::string temp : functionList) {
      if (functionName.str() == temp)
        return true;
    }
    return false;
  } else {
    return true;
  }
}

void DOPModule::transitiveClosure(Module &M) {
  // will be using Floyd-Warshall algorithm.

  // creating the data structure to store the transitive closure
  std::map<const Instruction *, std::map<const Instruction *, unsigned int>>
      closure_map;
  std::set<Instruction *> instset;

  // sets the distance of instruction to instruction to be 0
  // sets the next instruction to be 1
  for (auto &F : M)
    for (auto &BB : F)
      for (auto &I : BB) {
        instset.insert(&I);

        closure_map[&I][&I] = 0;

        if (CallInst *CI = dyn_cast<CallInst>(&I)) {
          Function *temp = CI->getCalledFunction();
          if (temp == nullptr)
            continue;
          if (temp->isDeclaration())
            continue;
          Instruction *next = &temp->front().front();
          closure_map[&I][next] = 1;
        } else if (I.isTerminator()) {
          for (unsigned int x = 0; x < I.getNumSuccessors(); x++) {
            Instruction *next = &(I.getSuccessor(x)->front());
            closure_map[&I][next] = 1;
          }
        } else {
          BasicBlock::InstListType::iterator it(I);
          it++;

          if (it != I.getParent()->getInstList().end()) {
            Instruction &next = *it;
            closure_map[&I][&next] = 1;
          } else {
            errs() << "ERROR!\n";
            exit(1);
          }
        }
      }

  exit(1);
}

bool DOPModule::runOnModule(Module &M) {
  // CSV Header
  llvm::errs() << "interesting_loops,total_loops,mov_global,mov_mixed,mov_local,arith_global,arith_mixed,arith_local,deref_global,deref_mixed,deref_local,logic_global,logic_mixed,logic_local,cond_global,cond_mixed,cond_local\n";

  // create the call graph of the module
  CallGraph CG(M);

  // we will calculate the transitive closure of the callgraph
  // TODO: this is chosen because of its simplicity, if performance is an
  // issue, we can look at other more complex algorithms to compute reachability
  // like treecover, 2hop labeling, pathtree cover.
  if (dop_danger)
    transitiveClosure(M);

  unsigned int total_loop = 0;
  unsigned int interesting_loop = 0;

  // read the function list from argument
  if (FL_path.compare("")) {
    std::string temp;
    std::ifstream FL(FL_path, std::ifstream::in);

    while (!FL.eof()) {
      std::getline(FL, temp);
      functionList.push_back(temp);
    }
  }

  if (vuln_path.compare("")) {
    // here, we want to process the set of functions that can occur after
    // the earliest time the vulnerable function can be called

    // read vulnerable function list
    std::vector<std::string> vulnFuncList;
    std::string temp;
    std::ifstream VF(vuln_path, std::ifstream::in);

    while (!VF.eof()) {
      std::getline(VF, temp);
      vulnFuncList.push_back(temp);
    }

    // iterate through the vulnerable functions
    // TODO: memoization can be used to optimize this code to remove redundant
    // traversing.
    for (std::string func : vulnFuncList) {
      if (func == "")
        continue;

      // get the set of callers for the vulnerable function
      // this is going to be convulated as the callgraph doesn't
      // support iterating through the callers

      // we are going to recursively find the caller of the function
      std::vector<Function *> worklist;
      std::set<Function *> visited;

      worklist.push_back(M.getFunction(func));

      while (!worklist.empty()) {
        Function *F = worklist.back();
        worklist.pop_back();

        if (F == nullptr)
          continue;
        if (visited.find(F) != visited.end())
          continue;
        if (F->isDeclaration())
          continue;

        visited.insert(F);

        // iterate through the entire user list (callsites that call the
        // function) identifying the function each call function is and adding
        // it back into the worklist. we will be keeping track of the visited
        // functions to deal with cyclic calls
        for (auto itr = F->user_begin(); itr != F->user_end(); itr++) {
          if (CallInst *CI = dyn_cast<CallInst>(*itr)) {
            Function *CallingFunc = CI->getParent()->getParent();
            worklist.push_back(CallingFunc);

            // we would like to also know what other functions are called
            // by this calling function. or do we?
          }
        }
      }

      // visited contains all the functions that _MAY_ directly or indirectly
      // call our vulnerable function F errs() << visited.size() << "\n";
    }
    exit(1);
  }

  if (dop_dyn.compare("")) {
    std::string temp;
    std::ifstream dyn_f(dop_dyn, std::ifstream::in);
    std::map<std::string, unsigned int> temp_map;

    while (!dyn_f.eof()) {
      std::string temp_func = "";
      unsigned int temp_lineno = 0;
      dyn_f >> temp_func >> temp_lineno;

      if (!temp_func.compare(""))
        continue;

      temp_map[temp_func] = temp_lineno;
    }

    // TODO: there must be some cleverer way here... but i'm desperate
    for (auto &F : M) {
      if (unsigned int lineno = temp_map[F.getName()]) {
        for (auto &BB : F)
          for (auto &I : BB) {
            const DebugLoc &DL = I.getDebugLoc();
            if (DL.getLine() == lineno) {
              dyn_map.insert(dyn_pair_t(&F, &I));
            }
          }
      }
    }
  }

  {
    // iterating through all the functions
    // less those in the functionlist
    for (auto &F : M) {
      if (F.isDeclaration())
        continue;

      bool interesting = false;
      LA = &getAnalysis<LoopInfoWrapperPass>(F).getLoopInfo();

      for (auto L : *LA) {
        if (!dop_dyn.compare("") && !FL_path.compare(""))
          goto desperate;
        if (FL_path.compare("") && checkFunction(F.getName()))
          goto desperate;
        if (dop_dyn.compare("")) {
          for (auto a : dyn_map) {
            if (L->contains(a.second))
              goto desperate;
          }
        }
        continue;

      desperate:
        // L contains all the top-level loops in the function
        // for each of the loops, we'll iterate through the basicblocks
        // within it.
        total_loop++;

        for (auto i = L->block_begin(); i != L->block_end(); i++) {
          // look at each instruction
          for (auto &I : **i) {
            // we have a store, we're going to check it
            if (StoreInst *SI = dyn_cast<StoreInst>(&I)) {
              InstInst ii(SI);
              if (ii.IsInteresting()) {
                interesting = true;
                result[SI] = ii;
              }
            } else if (CallInst *CI = dyn_cast<CallInst>(&I)) {
              // there is a call within the loop
              // we will get the functions called by this function
              // and explore all instructions in them
              if (Function *F = CI->getCalledFunction()) {
                for (auto &BB : *F) {
                  // look at each instruction
                  for (auto &I : BB) {
                    // we have a store, we're going to check it
                    if (StoreInst *SI = dyn_cast<StoreInst>(&I)) {
                      InstInst ii(SI);
                      if (ii.IsInteresting()) {
                        interesting = true;
                        result[SI] = ii;
                      }
                    }
                  }
                }
                CallGraphNode *CN = CG[F];
                for (unsigned int x = 0; x < CN->size(); x++) {
                  if (Function *FP = (*CN)[x]->getFunction()) {
                    if (FP->isDeclaration())
                      continue;
                    for (auto &BB : *FP) {
                      // look at each instruction
                      for (auto &I : BB) {
                        // we have a store, we're going to check it
                        if (StoreInst *SI = dyn_cast<StoreInst>(&I)) {
                          InstInst ii(SI);
                          if (ii.IsInteresting()) {
                            interesting = true;
                            result[SI] = ii;
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
      if (interesting)
        interesting_loop++;
    }

    // we're done running through the whole program
    // now we can output the results.

    if (dop_detail) {
      printDetail();
    } else if (dop_source) {
      printSource();
    } else {
      errs() << interesting_loop << ","
             << total_loop << ",";
      printStats();
    }
  }

  return false;
}

void DOPModule::getAnalysisUsage(AnalysisUsage &Info) const {
  Info.addRequired<LoopInfoWrapperPass>();
  Info.addRequiredTransitive<DominatorTreeWrapperPass>();
}

char DOPModule::ID = 0;
static RegisterPass<DOPModule> X("dop-module", "DOP Module", false, true);
