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
  CallLattice* cl = new CallLattice(start_value_);
  cl->initPSets(finest_a_->getPSets());
  initLattices.push_back(cl);
}

//=======================================================================================
bool CallAnalysis::transfer(const Function& func, const DataflowNode& n,
                            NodeState& state, const std::vector<Lattice*>& dfInfo)
{
  bool modi = false;
  if(mpiCallAnalysisDebugLevel >= 1)
    std::cerr << "\n                          CALL  TRANSFER";
  CallLattice* cl = dynamic_cast<CallLattice*>(*(dfInfo.begin()));

  if(MpiUtils::isMPIFunctionCall(n))
  {
    RankLattice* rank_l = NULL;
    LoopLattice* loop_l = NULL;
    if(state.isInitialized(rank_a_))
    {
      std::vector<Lattice*> lv = state.getLatticeBelow(rank_a_);
      if(lv.empty())
        return "ERROR lattice vector == NULL!";
      Lattice* lattice = *(lv.begin());
      if(lattice == NULL)
        return "ERROR lattice == NULL";
      rank_l = dynamic_cast<RankLattice*>(lattice);
    }
    if(state.isInitialized(loop_a_))
    {
      std::vector<Lattice*> lv = state.getLatticeBelow(loop_a_);
      if(lv.empty())
        return "ERROR lattice vector == NULL!";
      Lattice* lattice = *(lv.begin());
      if(lattice == NULL)
        return "ERROR lattice == NULL";
      loop_l = dynamic_cast<LoopLattice*>(lattice);
    }

    if(rank_l != NULL && loop_l != NULL)
      modi = cl->add(rank_l->getPSets(), n.n, loop_l->getLoopCount()) || modi;
    else if(rank_l != NULL)
      modi = cl->add(rank_l->getPSets(), n.n, _Loop_Count_()) || modi;
    else
      std::cerr << "ERROR: Rank Analysis required for CFGNode: "
                << n.toStringForDebugging();
  }
  return modi;
}
