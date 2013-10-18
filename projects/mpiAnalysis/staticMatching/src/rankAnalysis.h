#ifndef RANK_ANALYSIS_H
#define RANK_ANALYSIS_H
// Author: Robert Stoegbuchner
// Date: 24/10/2012

#include "rose.h"
#include "dataflow.h"
#include "latticeFull.h"
#include "pSet.h"

#if 0
extern int rankAnalysisDebugLevel;
#endif
#if 1
//! Type holding all PSets with the same denominator value
typedef std::map<int, PSet> cont_psets;
//! Type for holding all process sets of a lattice.
typedef std::map<int, cont_psets> lattice_psets;
#endif

typedef std::vector<PSet> psets_vec;

//! TODO
//=======================================================================================
class RankLattice : public FiniteLattice
{
public:
  //! TODO
  enum LatticeSourceType
  {
    Standard,    //!< Normal, unconditional edge
    IfTrue,    //!<
    IfFalse,    //!<
  };
private:

  //! All process sets of this node
  //
  //! PSets are represented wit b = n/d + o
  //! o is used as key in cont_psets
  //! n/d is used as key in the pSets with d = 0 for constant P sets ( don't divide !!)
  lattice_psets psets_;

  //! Contains psets_ of all predecessor nodes and separate true and false sets
  //! for conditional nodes.
  std::vector< std::pair<lattice_psets,
                         std::pair<SgNode*, LatticeSourceType> > > psets_vec_;

#if 0
  //!
  SgNode* rank_call_node_;

  //!
  SgNode* size_call_node_;
#endif

#if 0
//  //TODO:
//  ///Never used .... To be deleted
//  int scope_depth_;
#endif

  bool check_pred_;    //!< In case of being a direct successor node of a conditional
                       //!< statement, the check predecessor statement flag is set to
                       //!< true. In this case find out if the true or the true or false
                       //!< pSet has to be merged with the nodes pSet.

public:
  //! Do we need a default constructor?
  RankLattice();

  //! This constructor builds a constant value lattice.
  RankLattice( int v );

  //! Do we need the copy constructor?
  RankLattice(const RankLattice & X);

//=======================================================================================
  //!
  bool getScopeStmt() const;
  bool setScopeStmt(bool stmt);
  int getNodeCount() const;
  bool setNodeCount(int x);
  bool incrementNodeCount();
  lattice_psets getPSets() const;
  lattice_psets getPsets(RankLattice::LatticeSourceType source_type, SgNode* n);

#if 0
  SgNode* getRankCall();
  SgNode* getSizeCall();
  bool setRankCall(SgNode* node);
  bool setSizeCall(SgNode* node);
#endif

  //! Empties the whole psets_ map
  void clearPsets();

  //! Empties the whole psets_ map
  bool clearPsetsMap();

  // **********************************************
  // Process Set related functionality
  // **********************************************
  // typedef std::map<int, PSet> cont_psets;
  // std::map<int, cont_psets> psets_;
  // **********************************************

  //! Adds one process set range to psets_
  //! returns true if changes happened
  //! returns if added PSet was already in the psets_ (no changes)
  bool addPSet(PSet p_set);

  //!
  void addHardPSet(PSet p_set);
  //!
  void addHardPSet(lattice_psets& psets, PSet p_set);

  bool pushbackUniquePSets(RankLattice& X);
  bool pushbackPSets(RankLattice& X);
  bool pushbackPSets(lattice_psets sets,
                     RankLattice::LatticeSourceType edgetype,
                     SgNode* n);

  bool mergePSets(RankLattice& lattice);
  bool mergePSets(lattice_psets psets);
  bool mergePSets(PSet p_set);
  lattice_psets combinePSets(const lattice_psets sets1, const lattice_psets sets2);
  //PSet combinePSets(const PSet p_set1, const PSet p_set2);

  lattice_psets splitPSetsAtBound(bool lowbound, bound b);
  lattice_psets splitPSetsAtValue(bool val_set, bound b);

  lattice_psets computeTrueSet(const DataflowNode& n);
  lattice_psets computeFalseSet(const DataflowNode& n);
  lattice_psets getTrueSet(SgNode* n);
  lattice_psets getFalseSet(SgNode* n);

  bool existsTrueSet(SgNode* n);
  bool existsFalseSet(SgNode* n);

  //=======================================================================================
  // **********************************************
  // Required definition of pure virtual functions.
  // **********************************************
  //!
  //!
  void initialize();

  //! returns a hard copy of this lattice
  Lattice* copy() const;

  //! overwrites the state of "this" Lattice with "that" Lattice
  void copy(Lattice* that);

  //! compares values from "this" Lattice to "that" Lattice
  bool operator==(Lattice* that) /*const*/;

  //! computes the meet of this and that and saves the result in this
  //! returns true if this causes this to change and false otherwise
  bool meetUpdate(Lattice* that);

  //!
  std::string procSetsStr(std::string indent="");
  //!
  std::string procSetsMapStr(std::string indent="");
  //!
  std::string str(std::string indent="");
};








//=======================================================================================
class RankAnalysis : public IntraFWDataflow
{
protected:
  int start_value_;    //!< Value to avoid Segmentation fault from InterDataflowAnalysis.
                       //!< Seems to be required by the Lattice Constructor.
  SgProject* project_;    //!< SgProject in order to perform NodeQuery.

  varID rank_var_;    //!< The variable which contains the information about the Rank
                      //!< of the executing process, to be used in the transfer function
                      //!< to know what variables to check for.
  varID size_var_;    //!< The variable which contains the information about the Size
                      //!< of all executing processes, to be used at the loop reasoning
                      //!< step.

public:
  //! Constructor that takes the whole project as argument.
  //
  //! ...
  //! ...
  RankAnalysis(SgProject* project);

  //! Init function to initialize member variables
  //
  //! varID rank_var_
  //! varID size_var_
  bool InitRankAnalysis();

  //! Ast Query for MPI_Comm_Size function and for MPI_Comm_Rank function
  //! to set the rank_var_ and size_var_ member varID variables.
  //
  //! Detailed Description ....
  //! @return: true if varID's for rank and size are found.
  bool SetMPICommunicationInfo();

#if 0
  //!
  void setProject(SgProject* project) { project_ = project; }
#endif

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
  bool isIfRankDep(SgNode* node);
  bool isSupportedStmt(SgNode* node);
  bool containsRankVar(SgNode* node);
  bool onlyConstValues(SgNode* node);

//=======================================================================================
  //beter move this functions to some mpiUtils.h file!!
  bool isMPICommRank(SgNode* node);
  bool isMPICommSize(SgNode* node);
//  void setRankAnalysis(IntraProceduralDataflow* ra) { rank_analysis_ = ra; }
//  bool setSizeAndRank(SgProject* project);
//  boost::shared_ptr<IntraDFTransferVisitor> getTransferVisitor(const Function& func, const DataflowNode& n, NodeState& state, const std::vector<Lattice*>& dfInfo);
};

















//template <class LatticeType>
//class ProcSetStateTransfer : public IntraDFTransferVisitor
//{
//protected:
//  bool modified;
//  void updateModified(bool latModified) { modified = latModified || modified; }
//
//  const int debugLevel;
//
//  FiniteVarsExprsProductLattice* prodLat;
//
//  LatticeType *getLattice(const SgExpression *sgn) {
//    return sgn ? getLattice(SgExpr2Var(sgn)) : NULL;
//  }
//  LatticeType *getLattice(varID var) {
//    return dynamic_cast<LatticeType *>(prodLat->getVarLattice(var));
//  }
//
//  //! create three lattices from a binary operation: lhs, rhs, and result lattices
//  bool getLattices(const SgBinaryOp *sgn, LatticeType* &arg1Lat, LatticeType* &arg2Lat, LatticeType* &resLat) {
//    arg1Lat = getLattice(sgn->get_lhs_operand());
//    arg2Lat = getLattice(sgn->get_rhs_operand());
//    resLat = getLattice(sgn);
//
//    if(isSgCompoundAssignOp(sgn)) {
//      if(resLat==NULL && arg1Lat != NULL)
//        resLat = arg1Lat;
//    }
//    //Dbg::dbg << "transfer B, resLat="<<resLat<<"\n";
//
//    return (arg1Lat && arg2Lat && resLat);
//  }
//
//  bool getLattices(const SgUnaryOp *sgn,  LatticeType* &arg1Lat, LatticeType* &arg2Lat, LatticeType* &resLat) {
//    arg1Lat = getLattice(sgn->get_operand());
//    resLat = getLattice(sgn);
//
//    // Unary Update
//    if(isSgMinusMinusOp(sgn) || isSgPlusPlusOp(sgn)) {
//      arg2Lat = new LatticeType(1);
//    }
//    //Dbg::dbg << "res="<<res.str()<<" arg1="<<arg1.str()<<" arg1Lat="<<arg1Lat<<", arg2Lat="<<arg2Lat<<"\n";
//    //Dbg::dbg << "transfer B, resLat="<<resLat<<"\n";
//
//    return (arg1Lat && arg2Lat && resLat);
//  }
//
//public:
//  ProcSetStateTransfer(const Function& func, const DataflowNode& n, NodeState& state, const std::vector<Lattice*>& dfInfo, const int &debugLevel_)
//    : IntraDFTransferVisitor(func, n, state, dfInfo), modified(false), debugLevel(debugLevel_), prodLat(dynamic_cast<FiniteVarsExprsProductLattice*>(*(dfInfo.begin())))
//  {
//    //Dbg::dbg << "transfer A prodLat="<<prodLat<<"="<<prodLat->str("    ")<<"\n";
//    // Make sure that all the lattices are initialized
//    //prodLat->initialize();
//    const std::vector<Lattice*>& lattices = prodLat->getLattices();
//    for(std::vector<Lattice*>::const_iterator it = lattices.begin(); it!=lattices.end(); it++)
//      (dynamic_cast<LatticeType *>(*it))->initialize();
//  }
//
//  void visit(SgAssignOp *sgn)
//  {
//    LatticeType *lhsLat, *rhsLat, *resLat;
//    getLattices(sgn, lhsLat, rhsLat, resLat);
//
//    if(debugLevel>=1) {
//      if(resLat) Dbg::dbg << "resLat=\n    "<<resLat->str("    ")<<"\n";
//      if(lhsLat) Dbg::dbg << "lhsLat=\n    "<<lhsLat->str("    ")<<"\n";
//      if(rhsLat) Dbg::dbg << "rhsLat=\n    "<<rhsLat->str("    ")<<"\n";
//    }
//
//    // Copy the lattice of the right-hand-side to both the left-hand-side variable and to the assignment expression itself
//    if(resLat) // If the left-hand-side contains a live expression or variable
//      { resLat->copy(rhsLat); modified = true; }
//    if(lhsLat) // If the left-hand-side contains a live expression or variable
//      { lhsLat->copy(rhsLat); modified = true; }
//  }
//
//  void visit(SgAssignInitializer *sgn)
//  {
//    LatticeType* asgnLat = getLattice(sgn->get_operand());
//    LatticeType* resLat = getLattice(sgn);
//
//    if(debugLevel>=1) {
//      if(asgnLat) Dbg::dbg << "asgnLat=    "<<asgnLat->str("    ")<<"\n";
//      if(resLat) Dbg::dbg << "resLat=    "<<resLat->str("    ")<<"\n";
//    }
//
//    // If the result expression is live
//    if(resLat) { resLat->copy(asgnLat); modified = true; }
//  }
//
//  // XX: Right now, we take the meet of all of the elements of the
//  // initializer. This could be enhanced with an improved memory
//  // abstraction to treat each element individually.
//  void visit(SgAggregateInitializer *sgn)
//  {
//    LatticeType *res = getLattice(sgn);
//    SgExpressionPtrList &inits = sgn->get_initializers()->get_expressions();
//    if (inits.size() > 0) {
//      res->copy(getLattice(inits[0]));
//      modified = true;
//      for (size_t i = 1; i < inits.size(); ++i)
//        res->meetUpdate(getLattice(inits[i]));
//    }
//  }
//
//  // XX: This needs to be handled by an inter-procedural analysis
//  void visit(SgConstructorInitializer *sgn)
//  { }
//
//  // XX: I don't even know what this is - Phil
//  void visit(SgDesignatedInitializer *sgn)
//  { }
//
//  void visit(SgInitializedName *initName)
//  {
//    LatticeType* varLat = getLattice(initName);
//
//    if(varLat) {
//      LatticeType* initLat = getLattice(initName->get_initializer());
//      // If there was no initializer, leave this in its default 'bottom' state
//      if(initLat) {
//        varLat->copy(initLat);
//        modified = true;
//      }
//    }
//  }
//
//  void visit(SgBinaryOp *sgn) {
//    LatticeType *lhs, *rhs, *res;
//    getLattices(sgn, lhs, rhs, res);
//    if (res) {
//      res->copy(lhs);
//      res->meetUpdate(rhs);
//      modified = true;
//    }
//  }
//
//  void visit(SgCompoundAssignOp *sgn) {
//    LatticeType *lhs, *rhs, *res;
//    getLattices(sgn, lhs, rhs, res);
//    if (lhs)
//      updateModified(lhs->meetUpdate(rhs));
//    // Liveness of the result implies liveness of LHS
//    if (res) {
//      res->copy(lhs);
//      modified = true;
//    }
//  }
//
//  void visit(SgCommaOpExp *sgn)
//  {
//    LatticeType *lhsLat, *rhsLat, *resLat;
//    getLattices(sgn, lhsLat, rhsLat, resLat);
//
//    if (resLat) {
//      resLat->copy(rhsLat);
//      modified = true;
//    }
//  }
//
//  void visit(SgConditionalExp *sgn)
//  {
//    LatticeType *condLat = getLattice(sgn->get_conditional_exp()),
//                *trueLat = getLattice(sgn->get_true_exp()),
//                *falseLat = getLattice(sgn->get_false_exp()),
//                *resLat = getLattice(sgn);
//
//    // Liveness of the result implies liveness of the input expressions
//    if (resLat) {
//      resLat->copy(condLat);
//      resLat->meetUpdate(trueLat);
//      resLat->meetUpdate(falseLat);
//      modified = true;
//    }
//  }
//
//  void visit(SgScopeOp *)
//  {
//    // Documentation says this is no longer used, so explicitly fail if we see it
//    assert(0);
//  }
//
//  void visit(SgBitComplementOp *sgn)
//  {
//    LatticeType *res = getLattice(sgn);
//    if (res) {
//      res->copy(getLattice(sgn->get_operand()));
//      modified = true;
//    }
//  }
//};

   

//class RankAnalysisTransfer : public ProcSetStateTransfer<RankLattice>
//{
//private:
//  typedef void (RankAnalysisTransfer::*TransferOp)(RankLattice *, RankLattice *, RankLattice *);
////  template <typename T> void transferArith(SgBinaryOp *sgn, T transferOp);
//  template <typename T> void transferScope(SgIfStmt *sgn, T transferOp);
//
//  template <class T> void visitIntegerValue(T *sgn);
//
////  void transferArith(SgBinaryOp *sgn, TransferOp transferOp);
//  void transferScope(SgIfStmt *sgn, TransferOp transferOp);
//
//  void transferIf(SgIfStmt *sgn);
////  void transferIncrement(SgUnaryOp *sgn);
////  void transferCompoundAdd(SgBinaryOp *sgn);
////  void transferAdditive(RankLattice *arg1Lat, RankLattice *arg2Lat, RankLattice *resLat, bool isAddition);
////  void transferMultiplicative(RankLattice *arg1Lat, RankLattice *arg2Lat, RankLattice *resLat);
////  void transferDivision(RankLattice *arg1Lat, RankLattice *arg2Lat, RankLattice *resLat);
////  void transferMod(RankLattice *arg1Lat, RankLattice *arg2Lat, RankLattice *resLat);
//
//public:
////  //  void visit(SgNode *);
////  void visit(SgLongLongIntVal *sgn);
////  void visit(SgLongIntVal *sgn);
//  void visit(SgIntVal *sgn);
////  void visit(SgShortVal *sgn);
////  void visit(SgUnsignedLongLongIntVal *sgn);
////  void visit(SgUnsignedLongVal *sgn);
////  void visit(SgUnsignedIntVal *sgn);
////  void visit(SgUnsignedShortVal *sgn);
////  void visit(SgValueExp *sgn);
////  void visit(SgPlusAssignOp *sgn);
////  void visit(SgMinusAssignOp *sgn);
////  void visit(SgMultAssignOp *sgn);
////  void visit(SgDivAssignOp *sgn);
////  void visit(SgModAssignOp *sgn);
////  void visit(SgAddOp *sgn);
////  void visit(SgSubtractOp *sgn);
////  void visit(SgMultiplyOp *sgn);
////  void visit(SgDivideOp *sgn);
////  void visit(SgModOp *sgn);
////  void visit(SgPlusPlusOp *sgn);
////  void visit(SgMinusMinusOp *sgn);
////  void visit(SgUnaryAddOp *sgn);
////  void visit(SgMinusOp *sgn);
//
//  bool finish();
//
//  RankAnalysisTransfer(const Function& func, const DataflowNode& n, NodeState& state, const std::vector<Lattice*>& dfInfo);
//};


#endif
