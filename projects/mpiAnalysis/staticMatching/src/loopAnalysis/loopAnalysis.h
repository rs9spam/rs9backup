#ifndef LOOP_ANALYSIS_H
#define LOOP_ANALYSIS_H
// Author: Robert Stoegbuchner
// Date: 1/11/2013

#include "rose.h"
#include "dataflow.h"
#include "mpiUtils.h"
#include "cfgUtils.h"
#include "latticeFull.h"
#include "rankAnalysis/pSet.h"
#include "loopAnalysis/LoopLattice.h"

extern int loopAnalysisDebugLevel;

//=======================================================================================
class LoopAnalysis : public IntraFWDataflow
{
protected:
  int start_value_;    //!< Value to avoid Segmentation fault from InterDataflowAnalysis.
                       //!< Seems to be required by the Lattice Constructor. ????????????
  SgProject* project_;    //!< SgProject in order to perform NodeQuery.

  varID size_var_;    //!< The variable which contains the information about the Size
                      //!< of all executing processes, to be used at the loop reasoning
                      //!< step.

public:
  //! Constructor that takes the whole project as argument.
  //
  //! ...
  //! ...
  LoopAnalysis(SgProject* project);

  //! Init function to initialize member variables
  //
  //! varID rank_var_
  //! varID size_var_
  bool InitLoopAnalysis();

  //! Ast Query for MPI_Comm_Size function and for MPI_Comm_Rank function
  //! to set the rank_var_ and size_var_ member varID variables.
  //
  //! Detailed Description ....
  //! @return: true if varID's for rank and size are found.
  bool SetMPICommunicationInfo();

  //! Generate the initial lattice state for the given dataflow node, in the
  //! given function, with the given NodeState.
  void genInitState(const Function& func, const DataflowNode& n,
                    const NodeState& state, std::vector<Lattice*>& initLattices,
                    std::vector<NodeFact*>& initFacts);

  //! The transfer function that is applied to every node in the CFG
  //
  //! n - The dataflow node that is being processed
  //! state - The NodeState  object that describes the state of the node, as
  //!         established by earlier analysis passes
  //! dfInfo - The Lattices that this transfer function operates on. The
  //!            funtion takes theses lattices as input and overwrites the with
  bool transfer(const Function& func, const DataflowNode& n,
                NodeState& state, const std::vector<Lattice*>& dfInfo);

//=======================================================================================
#if 0
  //! Check if the DataflowNode n is a conditional statement, e.g. IfStmtm, While, For,
  //! which contains a constraint depending on the MPI rank variable.
  //
  //! return true, if DataflowNode n is dependent on MPI rank.
  //! return false, if DataflowNode n is not dependent on MPI rank.
  bool isMPIDepConditional(const DataflowNode& n) const;
  //! Check if it is a simple inequality that we can handle
  bool isSupportedStmt(const DataflowNode& n) const;
  //! Check if the constraint expression contains a var_ref_of MPIRankVar.
  bool containsMPIRankVar(/*const*/ SgNode* node) const;
  //! Check if the constraint expression contains a var_ref_of MPISizeVar.
  bool containsMPISizeVar(const SgNode* node) const;
  //!
  bool onlyConstValues(const SgNode* node) const;
#endif
//=======================================================================================
  //!
  std::vector<DataflowNode> getDFNodes() const;
};

#endif /*LOOP_ANALYSIS_H*/
