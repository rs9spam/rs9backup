#ifndef NON_DET_ANALYSIS_H
#define NON_DET_ANALYSIS_H

#include "genericDataflowCommon.h"
#include "cfgUtils.h"
#include "VirtualCFGIterator.h"
#include "cfgUtils.h"
#include "CFGRewrite.h"
#include "CallGraphTraverse.h"
#include "analysisCommon.h"
#include "analysis.h"
#include "dataflow.h"
#include "latticeFull.h"
#include "printAnalysisStates.h"
#include "liveDeadVarAnalysis.h"

extern int MPINonDetAnalysisDebugLevel;

//=============================================================================================
//                        MPINonDetLattice
//=============================================================================================

// Maintains information about a variable's dependence on MPI rank or number of ranks
class MPINonDetLattice : public FiniteLattice
{
public:

//             <true , true>  ---->  SgNode* non_det_function_node_
//                   |
//             <true , false>
//                   |
//            <false , false>  ...  <dependent_, condition_>
//                   |
//             uninitialized

private:

  bool initialized_;
  bool dependent_;
//  bool condition_;
//  std::list<SgNode*> scope_depth;  //Try to find the required scope information on the fly.
  SgNode* non_det_function_node_;

public:

  //
  MPINonDetLattice()
  {
    initialized_ = false;
    dependent_ = false;
    non_det_function_node_ = NULL;
  }

  //
  MPINonDetLattice(const MPINonDetLattice& that)
  {
    this->initialized_ = that.initialized_;
    this->dependent_ = that.dependent_;
    this->non_det_function_node_ = that.non_det_function_node_;
  }

  // initializes this Lattice to its default state, if it is not already initialized
  void initialize()
  {
    if(!initialized_)
    {
      initialized_ = true;
      dependent_ = false;
      non_det_function_node_ = NULL;
    }
  }

  // returns a copy of this lattice
  Lattice* copy() const;

  // overwrites the state of this Lattice with that of that Lattice
  void copy(Lattice* that);

  // computes the meet of this and that and saves the result in this
  // returns true if this causes this to change and false otherwise
  bool meetUpdate(Lattice* that);

  bool operator==(Lattice* that);

  // returns the current state of this object
  bool getDep() const;

  SgNode* getDepNode() const;

  // set the current state of this object, returning true if it causes
  // the object to change and false otherwise
  bool setDep(bool dep);

  bool setDepNode(SgNode* node);

  // Sets the state of this lattice to bottom (false, NULL)
  // returns true if this causes the lattice's state to change, false otherwise
  bool setToBottom();

  string str(string indent="");
};


//=============================================================================================
//                        MPINonDetAnalysis
//=============================================================================================

class MPINonDetAnalysis : public IntraFWDataflow
{
private:
  //LiveDeadVarsAnalysis* ldva;
  std::vector<SgNode*> non_det_nodes_;
  ClassHierarchyWrapper* classHierarchy_;

protected:
  // Adds the argument node to the non_det_nodes_ vector if it is not in it.
  // Returns: True, if it added the node to the vector.
  //          False, if the node was already an element of the non_det_nodes_ vector.
  bool addNonDetNode(SgNode* node);

public:
  //
  //MPINonDetAnalysis(): IntraFWDataflow() {}
  MPINonDetAnalysis(ClassHierarchyWrapper* classHierarchy): IntraFWDataflow()
  {
    classHierarchy_ = classHierarchy;
  }

  //
//  MPINonDetAnalysis(LiveDeadVarsAnalysis* ldva): IntraFWDataflow()
//  { this->ldva = ldva; }

  //
  std::vector<SgNode*> getNonDetNodes()
  { return non_det_nodes_; }

  // generates the initial lattice state for the given dataflow node, in the given function, with the given NodeState
  //vector<Lattice*> genInitState(const Function& func, const DataflowNode& n, const NodeState& state);
  void genInitState(const Function& func, const DataflowNode& n, const NodeState& state,
                    vector<Lattice*>& initLattices, vector<NodeFact*>& initFacts);

  bool transfer(const Function& func, const DataflowNode& n, NodeState& state, const vector<Lattice*>& dfInfo);

  //=============================================================================================
  bool isMpiInScope(SgNode* scope);
  bool isMpiInScope(SgNode* scope, set<SgNode*>& explored);
  bool isMpiInScopeAfter(SgNode* scope, SgNode* stmt);
};

//
MPINonDetAnalysis* runMPINonDetAnalysis(
                            SgIncidenceDirectedGraph* graph,
  //                          LiveDeadVarsAnalysis* ldva=NULL,
                            string indent="");

// Prints the Lattices set by the given MPINonDetAnalysis
void printMPINonDetAnalysisStates(string indent="");
void printMPINonDetAnalysisStates(MPINonDetAnalysis* nonDetAnal, string indent="");

// Checks if the argument is an MPI function call
// Returns true if it is an MPI function call
//         false if it is no MPI function call
bool isMpiFunctionCall(SgNode* node);

// Returns whether the given variable at the given DataflowNode depends on the process' rank
bool isMPIRankVarDep(const Function& func, const DataflowNode& n, varID var);

// Returns whether the given variable at the given DataflowNode depends on the number of processes
bool isMPINprocsVarDep(const Function& func, const DataflowNode& n, varID var);

// Sets nonDet and nprocsDep to true if some variable in the expression depends on the process' rank or
// the number of processes, respectively. False otherwise.
bool isMPIDep(const Function& func, const DataflowNode& n, bool& rankDep, bool& nprocsDep);

// Returns whether some variable at the given DataflowNode depends on the process' rank
bool isMPIRankDep(const Function& func, const DataflowNode& n);

// Returns whether some variable at the given DataflowNode depends on the number of processes
bool isMPINprocsDep(const Function& func, const DataflowNode& n);

// @Return: the parent scope (not basic block) of the SgNode
SgNode* getParentScope(SgNode* sgn);

// @Return: True, if an MPI function is in the scope of the argument scope.
//          False, if no MPI function is within the scope.
bool isMpiInScope(SgNode* scope);

// @Return: True, if an MPI function call is in the scope of the argument scope after the
//          node stmt.
//          False, if there is no MPI function call in this range.
bool isMpiInScopeAfter(SgNode* scope, SgNode* stmt);

bool isBreakInScope(SgStatement* scope);

bool isContinueInScope(SgStatement* scope);

SgNode* getFirstBreakStmtNodeFromScope(SgNode* scope);

#endif
