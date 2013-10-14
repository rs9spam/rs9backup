#ifndef RANK_ANALYSIS_H
#define RANK_ANALYSIS_H
// Author: stoero
// Date: 28/09/2013




























//#include <vector>
//#include <algorithm>
//#include <iostream>
//
//#include "genericDataflowCommon.h"
//#include "VirtualCFGIterator.h"
//#include "cfgUtils.h"
//#include "analysisCommon.h"
//#include "analysis.h"
//#include "dataflow.h"
//#include "latticeFull.h"

//class PSFWDataflow  : virtual public IntraProceduralDataflow
//{
//  /// Set of pCFGNodes that the analysis has already visited. Useful for determining
//  /// when the analysis visits a given pCFG node for the first time.
//  set<pCFGNode> visitedPCFGNodes;
//
//  public:
//  ///
//  PSFWDataflow()
//  { }
//
//  ///
//  PSFWDataflow(const PSFWDataflow& that)
//  { }
//
//  /// Generates the initial lattice state for the given Data-flow node,
//  /// in the given function, with the given NodeState. Does nothing in this analysis
//  /// since the rank analysis does not use Lattice elements.
//  void genInitState(const Function& func,
//                    const DataflowNode& n,
//                    const NodeState& state,
//                    vector<Lattice*>& initLattices,
//                    vector<NodeFact*>& initFacts) {}
//
//  /// Generates the initial lattice state for the given Data-flow node,
//  /// in the given function, with the given NodeState. Does nothing in this analysis
//  /// since the rank analysis does not use Lattice elements.
//  virtual void genInitState(const Function& func,
//                            const pCFGNode& n,
//                            const NodeState& state,
//                            vector<Lattice*>& initLattices,
//                            vector<NodeFact*>& initFacts) = 0;
//
//
//
//
//
//
//
//
//
//
//
//
//
//  // Runs the intra-procedural analysis on the given function. Returns true if
//  // the function's NodeState gets modified as a result and false otherwise.
//  // state - the function's NodeState
//  // NOTE: !!!There is currently no support for connecting this analysis to inter-procedural analyses!!!
//  bool runAnalysis(const Function& func, NodeState* state, bool analyzeDueToCallers, set<Function> calleesUpdated);
//
//  // Runs the dataflow analysis on the given partition, resuming from the given checkpoint.
//  // Specifically runAnalysis_pCFG() starts the pCFG dataflow analysis at some pCFG node and continues for
//  //    as long as it can make progress.
//  bool runAnalysis_pCFG(pCFG_Checkpoint* chkpt);
//
//  bool runAnalysis_pCFG(const Function& func, NodeState* fState, pCFG_Checkpoint* chkpt);
//
//  // Merge any process sets that are at equivalent dataflow states.
//  // Return true if any process sets were merged, false otherwise.
//  // If no processes are merged, mergePCFGNodes does not modify mergedN, dfInfo, activePSets, blockedPSets or releasedPSets.
//  bool mergePCFGNodes(const pCFGNode& n, pCFGNode& mergedN,
//                      const Function& func, NodeState& state, vector<Lattice*>& dfInfo,
//                      set<unsigned int>& activePSets, set<unsigned int>& blockedPSets, set<unsigned int>& releasedPSets, string indent="");
//
////  // Adds the set of checkpoints to the overall set of checkpoints currently active
////  // in this analysis. The caller is required to return immediately to the surrounding
////  // runAnalysisResume() call.
////  void split(const set<pCFG_Checkpoint*>& splitChkpts);
//
//  // Fills tgtLat with copies of the lattices in srcLat. tgtLat is assumed to be an empty vector
//  void latticeCopyFill(vector<Lattice*>& tgtLat, const vector<Lattice*>& srcLat);
//
//  // Deallocates all the lattices in lats and empties it out
//  void deleteLattices(vector<Lattice*>& lats);
//
//
//
//
//  // Copies the dataflow information from the srcPSet to the tgtPSet and updates the copy with the
//  // partitionCond (presumably using initPSetDFfromPartCond). Adds the new info directly to lattices and facts.
//  // It is assumed that pCFGNode n contains both srcPSet and tgtPSet.
//  // If omitRankSet==true, does not copy the constraints on srcPSet's bounds variables but
//  // instead just adds them with no non-trivial constraints.
//  virtual void copyPSetState(const Function& func, const pCFGNode& n,
//                        unsigned int srcPSet, unsigned int tgtPSet, NodeState& state,
//                        vector<Lattice*>& lattices, vector<NodeFact*>& facts,
//                        ConstrGraph* partitionCond, bool omitRankSet)=0;
//
//  // The transfer function that is applied to every node
//  // n - the pCFG node that is being processed
//  // pSet - the process set that is currently transitioning
//  // func - the function that is currently being analyzed
//  // state - the NodeState object that describes the state of the node, as established by earlier
//  //         analysis passes
//  // dfInfo - the Lattices that this transfer function operates on. The function takes these lattices
//  //          as input and overwrites them with the result of the transfer.
//  // deadPSet - may be set by the call to true to indicate that the dataflow state at this node constains an
//  //            inconsistency that makes this an impossible execution state
//  // splitPSet - set by the call to indicate if the current process set must be broken up into two process sets,
//  //             each of which takes a different path as a result of a conditional or a loop.
//  // splitPSetNodes - if splitPNode, splitNodes is filled with the nodes along which each new process set must advance
//  // splitPNode - set by the call to indicate if the current process set must take two different paths as a result
//  //             of a conditional or a loop, causing the analysis to advance along two different pCFGNodes
//  // splitConditions - if splitPNode==true or splitPSet==true, splitConditions is filled with the information
//  //             about the state along both sides of the split of pCFGNode or pSet split
//  // blockPSet - set to true by the call if progress along the given dataflow node needs to be blocked
//  //             until the next send-receive matching point. If all process sets become blocked, we
//  //             perform send-receive matching.
//  // Returns true if any of the input lattices changed as a result of the transfer function and
//  //    false otherwise.
//  virtual bool transfer(const pCFGNode& n, unsigned int pSet, const Function& func,
//                        NodeState& state, const vector<Lattice*>&  dfInfo,
//                        bool& deadPSet, bool& splitPSet, vector<DataflowNode>& splitPSetNodes,
//                   bool& splitPNode, vector<ConstrGraph*>& splitConditions, bool& blockPSet)=0;
//
//  // Called when a partition is created to allow a specific analysis to initialize
//  // its dataflow information from the partition condition
//  /*virtual void initDFfromPartCond(const Function& func, const pCFGNode& n, int pSet, NodeState& state,
//                                  const vector<Lattice*>& dfInfo, const vector<NodeFact*>& facts,
//                                  ConstrGraph* partitionCond)=0;*/
//
//  // Called when a process set is partitioned to allow the specific analysis to update the
//  // dataflow state for that process set with the set's specific condition.
//  // Returns true if this causes the dataflow state to change and false otherwise
//  virtual bool initPSetDFfromPartCond(const Function& func, const pCFGNode& n, unsigned int pSet,
//                                     const vector<Lattice*>& dfInfo, const vector<NodeFact*>& facts,
//                                     ConstrGraph* partitionCond)=0;
//
//  // Merge the dataflow information of two process sets. The space of process set IDs will be
//  // compressed to remove the holes left by the removal.
//  // pSetMigrations (initially assumed empty) is set to indicate which process sets have moved
//  //    to new ids, with the key representing the process set's original id and the value entry
//  //    representing the new id.
//  virtual void mergePCFGStates(const list<unsigned int>& pSetsToMerge, const pCFGNode& n, const Function& func,
//                               NodeState& state, const vector<Lattice*>& dfInfo, map<unsigned int, unsigned int>& pSetMigrations)=0;
//
//
//  // Performs send-receive matching on a fully-blocked analysis partition.
//  // If some process sets need to be split, returns the set of checkpoints that corresponds to this pCFG node's descendants.
//  // Otherwise, returns an empty set.
//  // Even if no split is required, matchSendsRecvs may modify activePSets and blockedPSets to release some process sets
//  //    that participated in a successful send-receive matching.
//  /*virtual set<pCFG_Checkpoint*> matchSendsRecvs(const pCFGNode& n, NodeState* state,
//                                                set<int>& activePSets, set<int>& blockedPSets, set<int>& releasedPSets,
//                                                const Function& func, NodeState* fState)=0;*/
//  virtual void matchSendsRecvs(const pCFGNode& n, const vector<Lattice*>& dfInfo, NodeState* state,
//                       // Set by analysis to identify the process set that was split
//                       unsigned int& splitPSet,
//                       vector<ConstrGraph*>& splitConditions,
//                       vector<DataflowNode>& splitPSetNodes,
//                       // for each split process set, true if its active and false if it is blocked
//                       vector<bool>&         splitPSetActive,
//                       // for each process set that was not split, true if becomes active as a result of the match,
//                       // false if its status doesn't change
//                       vector<bool>&         pSetActive,
//                       const Function& func, NodeState* fState)=0;
//
//  //typedef enum {retFalse, cont, normal} partitionTranferRet;
//
//  // Call the analysis transfer function.
//  // If the transfer function wants to block on the current DataflowNode, sets blockPSet to true.
//  // If the transfer function wants to split across multiple descendants, partitionTranfer() generates
//  //    the resulting checkpoints, performs the split and sets splitPSet to true.
//  // Otherwise, sets neither to true.
//  bool partitionTranfer(const pCFGNode& n, pCFGNode& descN, unsigned int curPSet, const Function& func, NodeState* fState,
//                        const DataflowNode& dfNode, NodeState* state,
//                        vector<Lattice*>& dfInfoBelow, vector<Lattice*>& dfInfoNewBelow,
//                        bool& deadPSet, bool& splitPSet, bool& splitPNode, bool& blockPSet,
//                        set<unsigned int>& activePSets, set<unsigned int>& blockedPSets, set<unsigned int>& releasedPSets,
//                   const DataflowNode& funcCFGEnd);
//
//  // Updates the lattices in tgtLattices from the lattices in srcLattices. For a given lattice pair <t, s>
//  //    (t from tgtLattices, s from srcLattices), t = t widen (t union s).
//  // If delSrcLattices==true, deletes all the lattices in srcLattices.
//  // Returns true if this causes the target lattices to change, false otherwise.
//  bool updateLattices(vector<Lattice*>& tgtLattices, vector<Lattice*>& srcLattices, bool delSrcLattices, string indent="");
//
//  // Split the process set pSet in pCFGNOde n, resulting in a new pCFGNode that contains more
//  //    process sets. Advances descN to be this new pCFGNode and updates activePSets to make
//  //    the newly-created process sets active.
//  // splitConditions - set of logical conditions that apply to each split process set
//  // splitPSetNodes - the DataflowNodes that each split process set starts at.
//  // if freshPSet==true, each partition will get a completely new process set and thus
//  //    must be set fresh. If freshPSet==false, each partition's process set is simply
//  //    an updated version of the old process set (i.e. an extra condition is applied).
//  void performPSetSplit(const pCFGNode& n, pCFGNode& descN, unsigned int pSet,
//                 vector<Lattice*>& dfInfo,
//                 vector<ConstrGraph*> splitConditions, vector<DataflowNode> splitPSetNodes,
//                 vector<bool>& splitPSetActive,
//                 const Function& func, NodeState* fState, NodeState* state,
//                 set<unsigned int>& activePSets, set<unsigned int>& blockedPSets, set<unsigned int>& releasedPSets,
//                 bool freshPSet, string indent);
//
//  // Removes all known bounds on pSet's process set in dfInfo and replaces them with the default
//  // constraints on process set bounds.
//  virtual void resetPSet(unsigned int pSet, vector<Lattice*>& dfInfo)=0;
//
//  // propagates the forward dataflow info from the current node's NodeState (curNodeState) to the next node's
//  // NodeState (nextNodeState)
//  /*bool */void propagateFWStateToNextNode(const Function& func,
//                const vector<Lattice*>& curNodeLattices, const pCFGNode& curNode, const NodeState& curNodeState,
//                vector<Lattice*>& nextNodeLattices, const pCFGNode& nextNode,
//                ConstrGraph* partitionCond, unsigned int pSet, string indent="");
//
//  // Moves the given process set from srcPSets to destPSets. If noSrcOk==false, does nothing
//  // if pSet is not inside srcPSets. If noSrcOk==true, adds pSet to destPSets regardless.
//  // Returns true if the process set was in srcPSets and false otherwise.
//static bool movePSet(unsigned int pSet, set<unsigned int>& destPSets, set<unsigned int>& srcPSets, bool noSrcOk=false);
//};
//
//
//
//
//
//
//
//
//










//#include "dataflow.h"
//#include "latticeFull.h"
//#include "pSet.h"

//extern int rankAnalysisDebugLevel;
//
//typedef std::map<int, PSet> cont_psets;
//typedef std::map<int, cont_psets> lattice_psets;
//
//class pSet
//{
//public:
//  enum LatticeSourceType
//  {
//    Standard,
//    IfTrue, //! Normal, unconditional edge
//    IfFalse,
//  };
//private:
////  typedef std::pair<int,bool> bound;
////  typedef std::pair<bound, bound> range;
////  std::set<range>;
//
//  // PSets are represented wit lb = lb/x + c
//  // c is used as key in cont_psets
//  // x is used as key in the pSets with x = 0 for const P sets ( don't divide !!)
//
////  typedef std::set<PSet*, compoffset> cont_psets;    //sorted with rising c
////  std::set<cont_psets*, compldiv> psets;      //sorted from back to front
//
//
//  //All the process sets of  this node
//  lattice_psets psets_;
//  //contains the psets_ of all predecessor nodes and separate true and false
//  //sets for conditional nodes.
//  std::vector< std::pair<lattice_psets, std::pair<SgNode*, LatticeSourceType> > > psets_vec_;
//
//  SgNode* rank_call_;
//  SgNode* size_call_;
//
//  int scope_depth_;
//  bool check_pred_;
//
//public:
//
//  // Do we need a default constructor?
//  RankLattice();
//
//  // This constructor builds a constant value lattice.
//  RankLattice( int v );
//
//  // Do we need th copy constructor?
//  RankLattice(const RankLattice & X);
//
//  // Access functions.
////  int getValue() const;
////  bool setValue(int x);
//  bool getScopeStmt() const;
//  bool setScopeStmt(bool stmt);
//  int getNodeCount() const;
//  bool setNodeCount(int x);
//  bool incrementNodeCount();
//  lattice_psets getPSets();
//  lattice_psets getPsets(RankLattice::LatticeSourceType source_type, SgNode* n);
//
//  SgNode* getRankCall();
//  SgNode* getSizeCall();
//  bool setRankCall(SgNode* node);
//  bool setSizeCall(SgNode* node);
//
//  //clears the whole psets_ map
//  void clearPsets();
//  bool clearPsetsMap();
//
//  // **********************************************
//  // Process Set related functionality
//  // **********************************************
//  // typedef std::map<int, PSet> cont_psets;
//  // std::map<int, cont_psets> psets_;
//  // **********************************************
//
//  bool doInterleave(PSet p_set1, PSet p_set2);
//  bool contains(PSet p_set, bool abs, int num, int den, int off);
//  bool isUnder(PSet p_set, bool abs, int num, int den, int off);
//  bool isOver(PSet p_set, bool abs, int num, int den, int off);
//
//  void addHardPSet(PSet p_set);
//  void addHardPSet(lattice_psets& psets, PSet p_set);
//
//  PSet combinePSets(PSet p_set1, PSet p_set2);
//
//  bool pushbackUniquePSets(RankLattice& X);
//  bool pushbackPSets(RankLattice& X);
//  bool pushbackPSets(lattice_psets sets,
//                     RankLattice::LatticeSourceType edgetype,
//                     SgNode* n);
//
//  bool mergePSets(RankLattice& lattice);
//  bool mergePSets(lattice_psets psets);
//  bool mergePSets(PSet p_set);
//  lattice_psets combinePSets(lattice_psets sets1, lattice_psets sets2);
//
//  lattice_psets splitSetBound(bool lowbound, bool abs, int num, int den, int off);
//  lattice_psets splitSetValue(bool val_set, bool abs, int num, int den, int off);
//
//
//  lattice_psets computeTrueSet(const DataflowNode& n);
//  lattice_psets computeFalseSet(const DataflowNode& n);
//  lattice_psets getTrueSet(SgNode* n);
//  lattice_psets getFalseSet(SgNode* n);
//  bool existsTrueSet(SgNode* n);
//  bool existsFalseSet(SgNode* n);
//
//  // Adds one process set range to psets_
//  // returns true if changes happened
//  // returns if added PSet was already in the psets_ (no changes)
//  bool addPSet(PSet p_set);
//
//  // Creates and adds one process set range to psets_
//  // returns true if changes happened
//  // returns if added PSet was already in the psets_ (no changes)
//  bool addPSet(bool empty_range,
//       bool lb_abs, bool hb_abs,
//       int lnum, int hnum,
//       int lden, int hden,
//       int loff, int hoff);
//
//
//
//
//  // **********************************************
//  // Required definition of pure virtual functions.
//  // **********************************************
//  void initialize();
//
//  // returns a copy of this lattice
//  Lattice* copy() const;
//
//  // overwrites the state of "this" Lattice with "that" Lattice
//  void copy(Lattice* that);
//
//  bool operator==(Lattice* that) /*const*/;
//
//  // computes the meet of this and that and saves the result in this
//  // returns true if this causes this to change and false otherwise
//  bool meetUpdate(Lattice* that);
//
//
//  std::string procSetsStr(std::string indent="");
//  std::string procSetsMapStr(std::string indent="");
//  std::string str(std::string indent="");
//};
//
//
//
////=======================================================================================
//class RankAnalysis : public IntraFWDataflow
//{
//protected:
////  static std::map<varID, Lattice*> constVars;
////  static bool constVars_init;
//  int start_value_;
//
//  //SgProject in order to perform NodeQuery
//  SgProject* project_;
////  //pointer to the analysis to find nodes
////  IntraProceduralDataflow* rank_analysis_;
//  varID rankVar_;
//  varID sizeVar_;
//
//
//public:
//  //RankAnalysis(sart_value_);
//  RankAnalysis(int x);
//
//  //RankAnalysis(project_);
//  RankAnalysis(SgProject* project) {project_ = project;}
//
//  // Generate the initial lattice state for the given dataflow node, in the
//  // given function, with the given NodeState
//  void genInitState(const Function& func, const DataflowNode& n,
//                    const NodeState& state, std::vector<Lattice*>& initLattices,
//                    std::vector<NodeFact*>& initFacts);
//
//  // The transfer function that is applied to every node in the CFG
//  // n - The dataflow node that is being processed
//  // state - The NodeState  object that describes the state of the node, as
//  //         established by earlier analysis passes
//  // dfInfo - The Lattices that this transfer function operates on. The
//  //            funtion takes theses lattices as input and overwrites the with
//  //
//  bool transfer(const Function& func, const DataflowNode& n,
//                NodeState& state, const std::vector<Lattice*>& dfInfo);
//
//
//  //=======================================================================================
//  bool isIfRankDep(SgNode* node);
//  bool isSupportedStmt(SgNode* node);
//  bool containsRankVar(SgNode* node);
//  bool onlyConstValues(SgNode* node);
//
//
//  //=======================================================================================
//  //beter move this functions to some mpiUtils.h file!!
//  bool isMPICommRank(SgNode* node);
//  bool isMPICommSize(SgNode* node);
//  void setProject(SgProject* project) { project_ = project; }
////  void setRankAnalysis(IntraProceduralDataflow* ra) { rank_analysis_ = ra; }
////  bool setSizeAndRank(SgProject* project);
//
////  boost::shared_ptr<IntraDFTransferVisitor> getTransferVisitor(const Function& func, const DataflowNode& n, NodeState& state, const std::vector<Lattice*>& dfInfo);
//};
#endif
