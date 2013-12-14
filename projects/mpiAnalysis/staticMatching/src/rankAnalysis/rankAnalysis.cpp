#include "rankAnalysis/rankAnalysis.h"
#include "mpiUtils/mpiUtils.h"
#include <stdlib.h>

int rrankAnalysisDebugLevel=1;

// **********************************************************************
//                     RankAnalysis
// **********************************************************************

#if 0
//=======================================================================================
RankAnalysis::RankAnalysis(int x)
{
  project_ = NULL;
  start_value_ = x;
}
#endif

//=======================================================================================
RankAnalysis::RankAnalysis(SgProject* project)
{
  project_ = project;
  start_value_ = 0;
  ROSE_ASSERT(InitRankAnalysis());
}

//=======================================================================================
bool RankAnalysis::InitRankAnalysis()
{
  return SetMPICommunicationInfo();
}

//=======================================================================================
bool RankAnalysis::SetMPICommunicationInfo()
{
  bool found_rank = false;
  bool found_size = false;

  Rose_STL_Container<SgNode*> functionCallExpList =
      NodeQuery::querySubTree(project_,V_SgFunctionCallExp);

  for(Rose_STL_Container<SgNode*>::iterator it = functionCallExpList.begin();
      it != functionCallExpList.end();
      ++it)
  {
    SgFunctionCallExp* fnCall = isSgFunctionCallExp(*it);
    Function calledFunc(fnCall);

    if(calledFunc.get_name().getString() == "MPI_Comm_rank")
    {
      SgExpressionPtrList& args = fnCall->get_args()->get_expressions();
      // args[1]
      SgExpression* arg1 = *(++(args.begin()));

      // Look at MPI_Comm_rank's second argument
      // and record that it depends on the process' rank.
      if(isSgAddressOfOp(arg1)
         && varID::isValidVarExp(isSgAddressOfOp(arg1)->get_operand()))
      {
        rank_var_ = SgExpr2Var(isSgAddressOfOp(arg1)->get_operand());
        if(rrankAnalysisDebugLevel >= 1)
          std::cerr << "\nSet Rank Variable to: " << rank_var_.str();
        found_rank = true;
      }
      else
      {
        fprintf(stderr,
                "[rankDepAnalysis] MPIRankDepAnalysis::transfer() ERROR: "
                "second argument \"%s\" of functfion call \"%s\" "
                "is not a reference to a variable!\n",
                arg1->unparseToString().c_str(),
                (*it)->unparseToString().c_str());
        exit(-1);
      }
    }
    else if(calledFunc.get_name().getString() == "MPI_Comm_size")
    {
      SgExpressionPtrList& args = fnCall->get_args()->get_expressions();
      // args[1]
      SgExpression* arg1 = *(++(args.begin()));
      // look at MPI_Comm_size's second argument and record that it depends on the number of processes
      if(isSgAddressOfOp(arg1) && varID::isValidVarExp(isSgAddressOfOp(arg1)->get_operand()))
      {
        size_var_ = SgExpr2Var(isSgAddressOfOp(arg1)->get_operand());
        if(rrankAnalysisDebugLevel >= 1)
          std::cerr << "\nSet Size Variable to: " << size_var_;
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
  if(found_rank && found_size)
    return true;
  if(!found_rank)
    std::cerr << "\nERROR MPI Program without rank information.\n";
  if(!found_size)
    std::cerr << "\nERROR MPI Program without size information.\n";
  return false;
}

//=======================================================================================
void RankAnalysis::genInitState(const Function& func, const DataflowNode& n,
                           const NodeState& state, std::vector<Lattice*>& initLattices,
                           std::vector<NodeFact*>& initFacts)
{
  RankLattice* l = new RankLattice(start_value_);

//  std::cerr << "\nGenerate Init State for node:\n"
//            << n.str();
  l->setNode(n.getNode());

  if(isMPIDepConditional(n) && isSupportedStmt(n))
  {
    l->setDepCondScope(true);
//    std::cerr << "\nsetScopeStatement() true at Conditional node!";
  }

  if(MpiUtils::isMPIInit(n))
  {
    _Bound_ lb = _Bound_(true,1,1,0);
    _Bound_ hb = _Bound_(false,1,1,-1);
    PSet ps = PSet(false,lb,hb);
    l->setPset(ps);
//    std::cerr << "\n$$$$$ Found MPI_Init node! $$$$$";
  }

  l->addPSet(PSet());
//  std::cerr << "\nLattice: " << l->toString() <<"\n\n";
  initLattices.push_back(l);
}

//=======================================================================================
bool RankAnalysis::transfer(const Function& func, const DataflowNode& n,
                            NodeState& state, const std::vector<Lattice*>& dfInfo)
{
  if(rrankAnalysisDebugLevel >= 1)
    std::cerr << "\n                                TRANSFER";
  RankLattice* lattice = dynamic_cast<RankLattice*>(*(dfInfo.begin()));

  if(lattice->isDepCondScope())
  {
    if(rrankAnalysisDebugLevel >= 1)
      std::cerr << "   IF\n";
    if(rrankAnalysisDebugLevel >= 2)
      std::cerr << lattice->str();
    //find out _Bound_
    bool modified = false;
    SgNode* node = n.getNode();
    SgNode* cond_node;
    std::vector<PSet> true_sets;
    std::vector<PSet> false_sets;
    PSet ps;
    SgNode* a;
    SgNode* b;

    if(lattice->isDepCondScope())
    {
      if(isSgIfStmt(node))
      {
        cond_node = isSgIfStmt(node)->get_conditional();
        if(isSgExprStatement(cond_node))
        {
          cond_node = isSgExprStatement(cond_node)->get_expression();
          //(rank == int) || (int == rank)
          if(isSgEqualityOp(cond_node))
          {
            //split off true and false sets.
            a = isSgBinaryOp(cond_node)->get_lhs_operand();
            b = isSgBinaryOp(cond_node)->get_rhs_operand();
            if(isSgVarRefExp(a) && isSgIntVal(b))
              if(isSgVarRefExp(a)->get_symbol()->get_name().getString() == rank_var_.str())
                ps = PSet(false, isSgIntVal(b)->get_value());
            if(isSgVarRefExp(b) && isSgIntVal(a))
              if(isSgVarRefExp(b)->get_symbol()->get_name().getString() == rank_var_.str())
                ps = PSet(false, isSgIntVal(a)->get_value());
            true_sets = lattice->intersect(ps);
            false_sets = lattice->remove(ps);
          }
          //(rank != int) || (int != rank)
          if(isSgNotEqualOp(cond_node))
          {
            a = isSgBinaryOp(cond_node)->get_lhs_operand();
            b = isSgBinaryOp(cond_node)->get_rhs_operand();
            if(isSgVarRefExp(a) && isSgIntVal(b))
              if(isSgVarRefExp(a)->get_symbol()->get_name().getString() == rank_var_.str())
                ps = PSet(false, isSgIntVal(b)->get_value());
            if(isSgVarRefExp(b) && isSgIntVal(a))
              if(isSgVarRefExp(b)->get_symbol()->get_name().getString() == rank_var_.str())
                ps = PSet(false, isSgIntVal(a)->get_value());
            true_sets = lattice->remove(ps);
            false_sets = lattice->intersect(ps);
          }
          //(rank < int) || (int < rank)
          //(rank <= int) || (int <= rank)
          //(rank > int) || (int > rank)
          //(rank >= int) || (int >= rank)
#if 0
//          std::cerr << "\nTRUE_SET " << lattice->procSetsStr(true_sets);
//          std::cerr << "\nFALSE_SET " << lattice->procSetsStr(false_sets);
          std::vector<DataflowEdge> out_edges = n.outEdges();
          std::vector<DataflowEdge>::iterator it = out_edges.begin();
          SgNode* true_target;
          SgNode* false_target;
          for(; it != out_edges.end(); ++it)
          {
            if(it->condition() == VirtualCFG::eckTrue)
              true_target = it->target().getNode();
            if(it->condition() == VirtualCFG::eckFalse)
              false_target = it->target().getNode();
//            std::cerr << "\nNODE at CONDITIONAL: ID: " << it->id()
//                                          << " Node: " << it->target().str();
          }
//          std::cerr << "\nTRUE_TARGET " << true_target;
//          std::cerr << "\nFALSE_TARGET " << false_target;

          //add true and false sets (if one of them does not exist set modified to true.)
          modified = lattice->pushPSetToOutVec(true_sets, true_target);
          modified = lattice->pushPSetToOutVec(false_sets, false_target);
//          std::cerr << "\nLATTICE FROM AFTER IF INSERT:\n" << lattice->toString();
#endif
          modified = lattice->pushPSetToOutVec(true_sets,
                                               MpiUtils::getTrueSuccessor(n));
          modified = lattice->pushPSetToOutVec(false_sets,
                                               MpiUtils::getFalseSuccessor(n));
        }
      }
      else
      {
        std::cerr << "/n/nERROR  1/n";
        true_sets = lattice->getPSets();

        if(isSgLocatedNode(node))
        {
          SgLocatedNode* sgln = isSgLocatedNode(node);
          size_t nr = sgln->get_numberOfTraversalSuccessors();
          for(size_t idx = 0; idx < nr; ++idx)
          {
            modified = lattice->pushPSetToOutVec(
                                     true_sets, sgln->get_traversalSuccessorByIndex(idx))
                       || modified;
          }
        }
        else
          std::cerr << "/n/nERROR  2/n";
      }
      //return true if changes to the out_sets_vec_ were introduced
      //return false if no changes to the out_sets_vec_ happend
    }
    if(rrankAnalysisDebugLevel >= 2)
      std::cerr << lattice->str();
    return modified;
  }
  else
  {
    if(rrankAnalysisDebugLevel >= 1)
      std::cerr << "          ELSE\n";
    if(rrankAnalysisDebugLevel >= 3)
    {
      std::cerr << lattice->str();
      bool mod = lattice->pushPSetToOutSet();
      std::cerr << lattice->str();
      return mod;
    }
    else
      return lattice->pushPSetToOutSet();
  }
}

// **********************************************************************
//                   If related  Decisions
// **********************************************************************
//=======================================================================================
bool RankAnalysis::isMPIDepConditional(const DataflowNode& n) const
{
  SgNode* node = n.getNode();
  SgNode* conditional;
  if(!isSgScopeStatement(node))
    return false;

  if(isSgIfStmt(node))
    conditional = isSgIfStmt(node)->get_conditional();
//  else if(isSg...(node))
//    cond = ...
  else
    return false;

  if(!containsMPIRankVar(conditional))
    return false;

  return true;
}

//=======================================================================================
bool RankAnalysis::isSupportedStmt(const DataflowNode& n) const
{
  SgNode* node = n.getNode();
  SgNode* lhs;
  SgNode* rhs;
  //statements supported at the moment:
    //rank == int_
    //rank != int_
    //rank < int_
    //rank > int_
    //rank == var_, if var_ evaluates to constant at constant propagation.

  if(isSgIfStmt(node))
  {
    node = isSgIfStmt(node)->get_conditional();
    if(isSgExprStatement(node))
    {
      node = isSgExprStatement(node)->get_expression();
      //(rank BO int) || (int BO rank)   for every Binary Operator BO
      if(isSgBinaryOp(node))
      {
        lhs = isSgBinaryOp(node)->get_lhs_operand();
        rhs = isSgBinaryOp(node)->get_rhs_operand();
        if(isSgVarRefExp(rhs) && isSgIntVal(lhs))
          if(isSgVarRefExp(rhs)->get_symbol()->get_name().getString() == rank_var_.str())
            return true;
        if(isSgVarRefExp(lhs) && isSgIntVal(rhs))
          if(isSgVarRefExp(lhs)->get_symbol()->get_name().getString() == rank_var_.str())
            return true;
      }
    }
  }
  return false;
}

//=======================================================================================
bool RankAnalysis::containsMPIRankVar(/*const*/ SgNode* node) const
{
  if(isSgVarRefExp(node))
  {
    SgVarRefExp* vre = isSgVarRefExp(node);
    if(rank_var_.str() == vre->get_symbol()->get_name().getString())
      return true;
  }

  if(isSgLocatedNode(node))
  {
    SgLocatedNode* sgln = isSgLocatedNode(node);
    size_t nr = sgln->get_numberOfTraversalSuccessors();
#if 0
    std::cerr<< "\nNo SgVarRefExp, but " << node->class_name() << " has "
             << nr << " successor nodes!";
#endif
    for(size_t idx = 0; idx < nr; ++idx)
    {
      return containsMPIRankVar(isSgLocatedNode(node)
                                 ->get_traversalSuccessorByIndex(idx));
    }
  }
  return false;
}

//=======================================================================================
bool RankAnalysis::containsMPISizeVar(const SgNode* node) const
{
//TODO:
  return true;
}

//=======================================================================================
bool RankAnalysis::onlyConstValues(const SgNode* node) const
{
//TODO:
  return true;
}

// **********************************************************************
//                     Temp stuff
// **********************************************************************

std::vector<DataflowNode> RankAnalysis::getDFNodes() const
{
  std::vector<DataflowNode> nodes;

  SgFunctionDeclaration* main_def_decl = SageInterface::findMain(project_);
  if(main_def_decl == NULL)
   ROSE_ASSERT(!"\nCannot get nodes without main function");
  SgFunctionDefinition* main_def = main_def_decl->get_definition();
  if(main_def == NULL)
   ROSE_ASSERT (!"Cannot get nodes for project without main function");

  DataflowNode funcCFGStart = cfgUtils::getFuncStartCFG(main_def, filter);

  // iterate over all the nodes in this function
  for(VirtualCFG::iterator it(funcCFGStart); it!=VirtualCFG::dataflow::end(); it++)
  {
    DataflowNode n = *it;
    nodes.push_back(n);
  }
  return nodes;
}
