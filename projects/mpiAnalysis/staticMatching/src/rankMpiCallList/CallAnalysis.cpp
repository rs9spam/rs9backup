#include "rankMpiCallList/CallAnalysis.h"
#include "nodeState.h"
#include <stdlib.h>
#include <boost/foreach.hpp>

int mpiCallAnalysisDebugLevel=1;

// **********************************************************************
//                     CallAnalysis
// **********************************************************************

//=======================================================================================
CallAnalysis::CallAnalysis(SgProject* project,
                           RankAnalysis* ra,
                           FinestPSetGranularity* fg,
                           LoopAnalysis* la)
{
  project_ = project;
  rank_a_ = ra;
  finest_a_ = fg;
  loop_a_ = la;
  start_value_ = 0;
}

//=======================================================================================
void CallAnalysis::genInitState(const Function& func, const DataflowNode& n,
                           const NodeState& state, std::vector<Lattice*>& initLattices,
                           std::vector<NodeFact*>& initFacts)
{
#if 0
  //if(mpiCallAnalysisDebugLevel >= 2)
    std::cerr << "\nCallAnalysis::init: BEGIN"
              << "----------------------------------------------------------------------"
              << "-------------";
#endif
  CallLattice* cl = new CallLattice(start_value_);
#if 0
    std::cerr << "\nCallAnalysis::init: after new CallLattice"
              << "--------------------------------";
#endif
  cl->initPSets(finest_a_->getPSets());
#if 0
    std::cerr << "\nCallAnalysis::init: InitPSets with finest_a_->getPsets()"
              << "-----------------";
#endif
  initLattices.push_back(cl);
#if 0
    std::cerr << "\nCallAnalysis::init: END."
              << "----------------------------------------------------------------------"
              << "--------------";
#endif
}

//=======================================================================================
bool CallAnalysis::transfer(const Function& func, const DataflowNode& n,
                            NodeState& state, const std::vector<Lattice*>& dfInfo)
{
#if 0
  std::cerr << "\nCallAnalysis::transfer:  idx: "
            << n.getIndex()
            << "  line: "
            << isSgLocatedNode(n.getNode())->get_file_info()->get_line()
            << "  class: "
            << n.getNode()->class_name()
            << "\n";
#endif

  bool modi = false;
  if(mpiCallAnalysisDebugLevel >= 1)
    std::cerr << "\n                          CALL  TRANSFER";
  CallLattice* cl = dynamic_cast<CallLattice*>(*(dfInfo.begin()));

  if(mpiCallAnalysisDebugLevel >= 2)
  {
    std::cerr << "\nBefore change happened:" << cl->toStringForDebugging();
    std::cerr << "\nNODE: | " << &n << "  |" << n.toStringForDebugging();
  }

  if(MpiUtils::isMPIFunctionCall(n))
  {
    RankLattice* rank_l = NULL;
    LoopLattice* loop_l = NULL;
    if(state.isInitialized(rank_a_))
    {
      std::vector<Lattice*> lv = state.getLatticeBelow(rank_a_);
      if(lv.empty())
        ROSE_ASSERT("ERROR lattice vector == NULL!");
      Lattice* lattice = *(lv.begin());
      if(lattice == NULL)
        ROSE_ASSERT("ERROR lattice == NULL");
      rank_l = dynamic_cast<RankLattice*>(lattice);
    }
    if(state.isInitialized(loop_a_))
    {
      std::vector<Lattice*> lv = state.getLatticeBelow(loop_a_);
      if(lv.empty())
        ROSE_ASSERT("ERROR lattice vector == NULL!");
      Lattice* lattice = *(lv.begin());
      if(lattice == NULL)
        ROSE_ASSERT("ERROR lattice == NULL");
      loop_l = dynamic_cast<LoopLattice*>(lattice);
    }

    if(rank_l != NULL && loop_l != NULL)
    {
      modi = cl->add(rank_l->getPSets(), n.n, loop_l->getLoopCount()) || modi;
      if(mpiCallAnalysisDebugLevel >= 2)
        std::cerr << "\nAfter changes: modified = " << (modi? "true" : "false")
                  << cl->toStringForDebugging();
    }
    else if(rank_l != NULL)
    {
      _Loop_Count_ lc = _Loop_Count_();
      modi = cl->add(rank_l->getPSets(), n.n, lc) || modi;
      if(mpiCallAnalysisDebugLevel >= 2)
        std::cerr << "\nAfter changes: modified = " << (modi? "true" : "false")
                  << cl->toStringForDebugging();
    }
    else
    {
      std::cerr << "ERROR: Rank Analysis required for CFGNode: "
                << n.toStringForDebugging();
    }
  }
  return modi;
}

//=======================================================================================
CallLattice CallAnalysis::getPSetCallList()
{
  CallLattice* cl = NULL;

  SgFunctionDeclaration* main_def_decl = SageInterface::findMain(project_);
  if(main_def_decl == NULL)
   ROSE_ASSERT(!"\nCannot get nodes without main function");
  SgFunctionDefinition* main_def = main_def_decl->get_definition();
  if(main_def == NULL)
   ROSE_ASSERT (!"Cannot get nodes for project without main function");

  DataflowNode funcCFGStart = cfgUtils::getFuncStartCFG(main_def, filter);

  bool found = false;
  DataflowNode Mpi_Finalize_CFG = cfgUtils::getFuncStartCFG(main_def, filter);
  for(VirtualCFG::iterator it(funcCFGStart); it!=VirtualCFG::dataflow::end(); it++)
  {
    NodeState* state = NodeState::getNodeState(*it,0);

    if(state->isInitialized(this))
    {
      std::vector<Lattice*> lv = state->getLatticeAbove(this);
      if(lv.empty())
        ROSE_ASSERT("ERROR lattice vector == NULL!");
      Lattice* lattice = *(lv.begin());
      if(lattice == NULL)
        ROSE_ASSERT("ERROR lattice == NULL");
      cl = dynamic_cast<CallLattice*>(lattice);
//      std::cerr << "\nAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
//      std::cerr << cl->toStringForDebugging();
    }


    if(MpiUtils::isMPIFinalize(*it))
    {
      Mpi_Finalize_CFG = *it;
      found = true;
      break;
    }
  }

  if(found)
  {
    NodeState* state = NodeState::getNodeState(Mpi_Finalize_CFG,0);

    if(state->isInitialized(this))
    {
//      std::cerr << "\nState IS INITIALIZED";
      std::vector<Lattice*> lv = state->getLatticeBelow(this);
      if(lv.empty())
        ROSE_ASSERT("ERROR lattice vector == NULL!");
      Lattice* lattice = *(lv.begin());
      if(lattice == NULL)
        ROSE_ASSERT("ERROR lattice == NULL");
      cl = dynamic_cast<CallLattice*>(lattice);
    }
  }
  else
  {
    std::cerr << "\nERROR: CallAnalysis:getPSetCallList(): "
              << "Could not find MPI_Finalize().";
  }

  if(cl != NULL)
    return *cl;
  else
    return CallLattice();
}
