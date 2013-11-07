#include "loopAnalysis/loopAnalysis.h"

#include <stdlib.h>

int loopAnalysisDebugLevel=1;

// **********************************************************************
//                     LoopAnalysis
// **********************************************************************

//=======================================================================================
LoopAnalysis::LoopAnalysis(SgProject* project)
{
  project_ = project;
  start_value_ = 0;
  ROSE_ASSERT(InitLoopAnalysis());
}

//=======================================================================================
bool LoopAnalysis::InitLoopAnalysis()
{
  return SetMPICommunicationInfo();
}

//=======================================================================================
bool LoopAnalysis::SetMPICommunicationInfo()
{
  bool found_size = false;

  Rose_STL_Container<SgNode*> functionCallExpList =
      NodeQuery::querySubTree(project_,V_SgFunctionCallExp);

  for(Rose_STL_Container<SgNode*>::iterator it = functionCallExpList.begin();
      it != functionCallExpList.end();
      ++it)
  {
    SgFunctionCallExp* fnCall = isSgFunctionCallExp(*it);
    Function calledFunc(fnCall);

    if(calledFunc.get_name().getString() == "MPI_Comm_size")
    {
      SgExpressionPtrList& args = fnCall->get_args()->get_expressions();
      // args[1]
      SgExpression* arg1 = *(++(args.begin()));
      // look at MPI_Comm_size's second argument and record that it depends on the number of processes
      if(isSgAddressOfOp(arg1) && varID::isValidVarExp(isSgAddressOfOp(arg1)->get_operand()))
      {
        std::cerr << "\nSet Size Variable to ........ ";
        size_var_ = SgExpr2Var(isSgAddressOfOp(arg1)->get_operand());
        std::cerr << size_var_ << ".......";
        found_size = true;
      }
      else
      {
        fprintf(stderr,
                "[rankDepAnalysis] MPIRankDepAnalysis::transfer() ERROR: "
                "second argument \"%s\" of function call \"%s\" "
                "is not a reference to a variable!\n",
                arg1->unparseToString().c_str(),
                (*it)->unparseToString().c_str());
        exit(-1);
      }
    }
  }
  if(found_size)
    return true;
  return false;
}

//=======================================================================================
void LoopAnalysis::genInitState(const Function& func, const DataflowNode& n,
                           const NodeState& state, std::vector<Lattice*>& initLattices,
                           std::vector<NodeFact*>& initFacts)
{
  LoopLattice* l = new LoopLattice(start_value_);
  MpiUtils mu = MpiUtils();
  l->setNode(n.getNode());
  //check if any loop, since if it is a not handle able loop,
  //the bound is going to be infinite.
  if(mu.isLoopStmt(n))
  {
    l->isHandledLoop(true);
    l->setFalseSuccessor(mu.getFalseSuccessor(n));
    l->setLoopCountStruc(getLoopBound(n));
  }
  initLattices.push_back(l);
}

//=======================================================================================
bool LoopAnalysis::transfer(const Function& func, const DataflowNode& n,
                            NodeState& state, const std::vector<Lattice*>& dfInfo)
{
  if(loopAnalysisDebugLevel >= 1)
    std::cerr << "\n                          LOOP  TRANSFER";
  LoopLattice* ll = dynamic_cast<LoopLattice*>(*(dfInfo.begin()));

  if(ll->isHandledLoop())
    return ll->addLoopCountStruct(ll->getNode(), ll->getLoopCountStruct());
//  if(loopAnalysisDebugLevel >= 1)
//    std::cerr << "          ELSE\n";
//  if(loopAnalysisDebugLevel >= 3)
//  {
//    std::cerr << lattice->stsd  r();
//    bool mod = lattice->pushBack(n.n,)
//    std::cerr << lattice->str();
//    return mod;
//  }
//  else
//    return lattice->pushPSetToOutSet();
  return false;
}

//=======================================================================================
loopCountStruct LoopAnalysis::getLoopBound(const DataflowNode& n) const
{
  MpiUtils mu = MpiUtils();
  if(!mu.isLoopStmt(n))
    return loopCountStruct();

  if(!isCoveredLoop(n))
    return loopCountStruct(true);

  SgForStatement* fstmt = isSgForStatement(n.getNode());
  if(fstmt == NULL)
    return loopCountStruct();

  SgForInitStatement* for_init_stmt = fstmt->get_for_init_stmt();
  SgStatement* condition = fstmt->get_test();
  SgExpression* increment = fstmt->get_increment();
  bound init_bound;
  bound end_bound;
  bound loop_bound;

  //get bound(init_value)
  if(for_init_stmt == NULL || condition == NULL || increment ==NULL)
      return loopCountStruct(true);
  SgStatementPtrList stmt_list = for_init_stmt->get_init_stmt();
  if(stmt_list.empty())
    return loopCountStruct(true);
  SgNode* tmp_stmt = *(stmt_list.begin());
  if(!isSgExprStatement(tmp_stmt))
    return loopCountStruct(true);
  if(!isSgAssignOp(isSgExprStatement(tmp_stmt)->get_expression()))
    return loopCountStruct(true);
  tmp_stmt = isSgAssignOp(isSgExprStatement(tmp_stmt)->get_expression());
  SgExpression* lhs = isSgBinaryOp(tmp_stmt)->get_lhs_operand();
  SgExpression* rhs = isSgBinaryOp(tmp_stmt)->get_rhs_operand();
  if(!(isSgVarRefExp(lhs) && isSgIntVal(rhs)))
    return loopCountStruct(true);
  init_bound = bound(isSgIntVal(rhs)->get_value());

  //get end_value
  if(!isSgExprStatement(condition))
    return loopCountStruct(true);
  if(!isSgNotEqualOp(isSgExprStatement(condition)->get_expression()))
    return loopCountStruct(true);
  tmp_stmt = isSgNotEqualOp(isSgExprStatement(condition)->get_expression());
  rhs = isSgBinaryOp(tmp_stmt)->get_rhs_operand();
  if(isSgIntVal(rhs) )
    end_bound = bound(isSgIntVal(rhs)->get_value());
  else if(isSgVarRefExp(rhs) &&
      isSgVarRefExp(rhs)->get_symbol()->get_name().getString() == this->size_var_.str())
    end_bound = bound(false,1,1,0);
  else
    return loopCountStruct(true);

  //get increment
  //if(increment == ++)
    // bound = end_value - init_value
  //if(increment == --)
    // bound = init_value - end_value
  std::cerr << end_bound.toStr() << "  " << init_bound.toStr();

  if(isSgPlusPlusOp(increment))
    loop_bound = end_bound - init_bound;
  else if(isSgMinusMinusOp(increment))
    loop_bound = init_bound - end_bound;
  else
    return loopCountStruct(true);

  if(loopAnalysisDebugLevel >= 2)
      std::cerr << "   Returning loop_bound  " << loop_bound.toStr();
  return loopCountStruct(false,loop_bound);
}

//=======================================================================================
bool LoopAnalysis::isCoveredLoop(const DataflowNode& n) const
{
  if(loopAnalysisDebugLevel >= 2)
    std::cerr << "\nCheck if n is a covered loop.";
  //TODO: only dealing with SgForStatement at the moment!
  SgForStatement* fstmt = isSgForStatement(n.getNode());
  if(fstmt == NULL)
  {
    if(loopAnalysisDebugLevel >= 2)
        std::cerr << "  fstm == NULL";
    return false;
  }
  SgForInitStatement* for_init_stmt =  fstmt->get_for_init_stmt();
  SgStatement* condition = fstmt->get_test();
  SgExpression* increment = fstmt->get_increment();
  if(for_init_stmt == NULL || condition == NULL || increment ==NULL)
  {
      if(loopAnalysisDebugLevel >= 2)
        std::cerr << "  for_init_stmt || condition || infrement == NULL";
      return false;
  }
  //init statement is a simple assignment of a const value to an int variable
  // i = SgIntVal
  SgStatementPtrList stmt_list = for_init_stmt->get_init_stmt();
  if(stmt_list.empty())
  {
    if(loopAnalysisDebugLevel >= 2)
      std::cerr << "  stmt_list.empty!";
    return false;
  }

  SgNode* tmp_stmt = *(stmt_list.begin());
  if(!isSgExprStatement(tmp_stmt))
  {
    if(loopAnalysisDebugLevel >= 2)
      std::cerr << " <" << tmp_stmt->class_name() << ">  no SgExprStatement!";
    return false;
  }

  if(!isSgAssignOp(isSgExprStatement(tmp_stmt)->get_expression()))
  {
    if(loopAnalysisDebugLevel >= 2)
      std::cerr << " <"
                << (isSgAssignOp(isSgExprStatement(tmp_stmt)->get_expression()))
                                                                        ->class_name()
                << ">  no SgAssignmentOp!";
    return false;
  }
  tmp_stmt = isSgAssignOp(isSgExprStatement(tmp_stmt)->get_expression());
  SgExpression* lhs = isSgBinaryOp(tmp_stmt)->get_lhs_operand();
  SgExpression* rhs = isSgBinaryOp(tmp_stmt)->get_rhs_operand();
  if(!(isSgVarRefExp(lhs) && isSgIntVal(rhs)))
  {
    if(loopAnalysisDebugLevel >= 2)
      std::cerr << "  wrong lhs or rhs elements";
    return false;
  }

  //condition is a simple inequality
  // i != SgIntVal
  if(!isSgExprStatement(condition))
  {
    if(loopAnalysisDebugLevel >= 2)
      std::cerr << "  condition is no SgExprStatement";
    return false;
  }
  if(!isSgNotEqualOp(isSgExprStatement(condition)->get_expression()))
  {
    if(loopAnalysisDebugLevel >= 2)
      std::cerr << "  condition Expression is not SgNotEqualOp";
    return false;
  }
  tmp_stmt = isSgNotEqualOp(isSgExprStatement(condition)->get_expression());
  lhs = isSgBinaryOp(tmp_stmt)->get_lhs_operand();
  rhs = isSgBinaryOp(tmp_stmt)->get_rhs_operand();
  if(!(  (isSgVarRefExp(lhs) && isSgVarRefExp(rhs))
      || (isSgVarRefExp(lhs) && isSgIntVal(rhs))))
  {
    if(loopAnalysisDebugLevel >= 2)
      std::cerr << "  wrong lhs or rhs elements";
    return false;
  }
  if(isSgVarRefExp(rhs))
  {
    if(isSgVarRefExp(rhs)->get_symbol()->get_name().getString() != this->size_var_.str())
    {
      if(loopAnalysisDebugLevel >= 2)
        std::cerr << "  SgVarRefExp is wrong variable for a manageable bound.";
      return false;
    }
  }


  //increment is either ++i or i++ SgUnaryOp // SgPlusPlusOp
  if(!(isSgPlusPlusOp(increment) || isSgMinusMinusOp(increment)))
  {
    if(loopAnalysisDebugLevel >= 2)
      std::cerr << "<" << increment->class_name() << ">  increment is no ++ or --";
    return false;
  }

  if(loopAnalysisDebugLevel >= 2)
    std::cerr << "  Yes, it is!!!  ";
  return true;
}

// **********************************************************************
//                   If related  Decisions
// **********************************************************************
std::vector<DataflowNode> LoopAnalysis::getDFNodes() const
{
  std::vector<DataflowNode> nodes;

  SgFunctionDeclaration* main_def_decl = SageInterface::findMain(project_);
  if(main_def_decl == NULL)
   ROSE_ASSERT(!"\nCannot get nodes without main function");
  SgFunctionDefinition* main_def = main_def_decl->get_definition();
  if(main_def == NULL)
   ROSE_ASSERT (!"Cannot get nodes for project without main function");

  DataflowNode funcCFGStart = cfgUtils::getFuncStartCFG(main_def, filter);
  DataflowNode funcCFGEnd = cfgUtils::getFuncEndCFG(main_def, filter);

  // iterate over all the nodes in this function
  for(VirtualCFG::iterator it(funcCFGStart); it!=VirtualCFG::dataflow::end(); it++)
  {
    DataflowNode n = *it;
    nodes.push_back(n);
  }
  return nodes;
}
