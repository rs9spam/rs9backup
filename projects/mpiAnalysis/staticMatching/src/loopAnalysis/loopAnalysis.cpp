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
  RankLattice* l = new RankLattice(start_value_);
  l->initialize();
  initLattices.push_back(l);
}

//=======================================================================================
bool LoopAnalysis::transfer(const Function& func, const DataflowNode& n,
                            NodeState& state, const std::vector<Lattice*>& dfInfo)
{
  if(loopAnalysisDebugLevel >= 1)
    std::cerr << "\n                                TRANSFER";
  LoopLattice* lattice = dynamic_cast<LoopLattice*>(*(dfInfo.begin()));


//  if(loopAnalysisDebugLevel >= 1)
//    std::cerr << "          ELSE\n";
//  if(loopAnalysisDebugLevel >= 3)
//  {
//    std::cerr << lattice->str();
//    bool mod = lattice->pushBack(n.n,)
//    std::cerr << lattice->str();
//    return mod;
//  }
//  else
//    return lattice->pushPSetToOutSet();
  return false;
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
//    const vector<NodeState*> nodeStates = NodeState::getNodeStates(n);
//    // Visit each CFG node
//    for(vector<NodeState*>::const_iterator itS = nodeStates.begin();
//        itS!=nodeStates.end();
//        itS++)
//      visit(func, n, *(*itS));
  }
  return nodes;
}
