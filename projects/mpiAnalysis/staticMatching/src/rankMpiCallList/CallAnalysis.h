#ifndef CALL_ANALYSIS_H
#define CALL_ANALYSIS_H
// Author: Robert Stoegbuchner
// Date: 1/11/2013

#include "rose.h"
#include "dataflow.h"
#include "mpiUtils/mpiUtils.h"
#include "mpiCFG/mpiCFG.h"
#include "cfgUtils.h"
#include "latticeFull.h"
#include "rankAnalysis/pSet.h"
#include "rankAnalysis/rankAnalysis.h"
#include "rankAnalysis/FinestPSetGranularity.h"
#include "loopAnalysis/loopAnalysis.h"
#include "rankMpiCallList/CallLattice.h"

extern int mpiCallAnalysisDebugLevel;

//=======================================================================================
class CallAnalysis : public IntraFWDataflow
{
protected:
  int start_value_;    //!< Value to avoid Segmentation fault from InterDataflowAnalysis.
                       //!< Seems to be required by the Lattice Constructor. ?
  SgProject* project_;    //!< SgProject in order to perform NodeQuery.

  /**
   *
   */
  RankAnalysis* rank_a_;

  /**
   *
   */
  FinestPSetGranularity* finest_a_;

  /**
   *
   */
  LoopAnalysis* loop_a_;

  /**
   *
   */
  CallLattice cl_;

public:
  /**
   * @brief Constructor that takes the whole project and rank analysis as argument.
   * @param project
   * @param ra
   * @param fg
   * @param la The loop analysis is optional. If there is no loop analysis, ever loop
   *           count will be one.
   */
  CallAnalysis(SgProject* project,
               RankAnalysis* ra,
               FinestPSetGranularity* fg,
               LoopAnalysis* la);

#if 0
  /**
   * @brief Init function to initialize member variables.
   */
  bool InitCallAnalysis();
#endif

  /**
   * @brief Generate the initial lattice state.
   * @param func
   * @param n   The DataflowNode that is being processed.
   * @param state
   * @param initLattices
   * @param initFacts
   * Generate the initial lattice state for the given DataflowNode, in the
   * given function, with the given NodeState.
   */
  void genInitState(const Function& func, const DataflowNode& n,
                    const NodeState& state, std::vector<Lattice*>& initLattices,
                    std::vector<NodeFact*>& initFacts);

  /**
   * @brief The transfer function that is applied to every node in the CFG.
   * @param n  The DataflowNode that is being processed.
   * @param state   The NodeState  object that describes the state of the node, as
   *                established by earlier analysis passes.
   * @param dfInfo   The Lattices that this transfer function operates on.
   */
  bool transfer(const Function& func, const DataflowNode& n,
                NodeState& state, const std::vector<Lattice*>& dfInfo);

  /**
   * @brief Get the MPI function call list.
   * @return Function call list is represented by the CallLattice.
   */
  CallLattice getPSetCallList();
};

#endif /*CALL_ANALYSIS_H*/
