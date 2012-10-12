#ifndef VIRTUAL_MPICFG_H
#define VIRTUAL_MPICFG_H

#include "virtualCFG.h"

//class SgNode;
//class SgExpression;
//class SgInitializedName;
//class SgLabelSymbol;
//class SgLabelRefExp;
//class SgStatement;
//class SgOmpClause;
//
//#ifndef _MSC_VER
//SgStatement* isSgStatement(SgNode* node);
//const SgStatement* isSgStatement(const SgNode* node);
//SgExpression* isSgExpression(SgNode* node);
//const SgExpression* isSgExpression(const SgNode* node);
//SgInitializedName* isSgInitializedName(SgNode* node);
//const SgInitializedName* isSgInitializedName(const SgNode* node);
//SgOmpClause* isSgOmpClause(SgNode* node);
//const SgOmpClause* isSgOmpClause(const SgNode* node );
//#endif

namespace VirtualMPICFG {

  class MPICFGEdge;

  //! The conditions attached to edges are marked to determine which conditions
  //! trigger control to flow along that edge (as opposed to other edges out of
  //! the same source node).  For conditional branches (except eckCaseLabel and
  //! eckDefault), the conditions are implicit and depend on knowledge of the
  //! particular control structure.  Fortran support for this is underdeveloped;
  //! single AST nodes representing variable-length loops was not part of the
  //! original design of the CFG code.
  enum EdgeConditionKind
  {
    eckUnconditional, //! Normal, unconditional edge
    eckTrue,          //! True case of a two-way branch
    eckFalse,         //! False case of a two-way branch
    eckCaseLabel,     //! Case label (constant is given by caseLabel())
    eckDefault,       //! Default label
    eckDoConditionPassed, //! Enter Fortran do loop body
    eckDoConditionFailed, //! Fortran do loop finished
    eckForallIndicesInRange, //! Start testing forall mask
    eckForallIndicesNotInRange, //! End of forall loop
    eckComputedGotoCaseLabel, //! Case in computed goto -- number needs to be computed separately
    eckArithmeticIfLess, //! Edge for the arithmetic if expression being less than zero
    eckArithmeticIfEqual, //! Edge for the arithmetic if expression being equal to zero
    eckArithmeticIfGreater, //! Edge for the arithmetic if expression being greater than zero
    eckInterprocedural, //! Edge spanning two procedures
    eckMPI //! Edge spanning between MPI calls
  };

  //! A node in the control flow graph.  Each CFG node corresponds to an AST
  //! node, but there can be several CFG nodes for a given AST node.
  class CFGNode : public VirtualCFG::CFGNode {

    //! The AST node from this CFG node
    //SgNode* node; // Must be either a SgStatement, SgExpression, or SgInitializedName (FIXME: change this to just SgLocatedNode if SgInitializedName becomes a subclass of that)

    //! An index to differentiate control flow points within a single AST node
    //! (for example, before and after the test of an if statement)
    //unsigned int index;

    public:

//    //! Pretty string for Dot node labels, etc.
//    std::string toString() const;
//    //! String for debugging graphs
//    std::string toStringForDebugging() const;
//    //! ID to use for Dot, etc.
//    std::string id() const;
//    //! The underlying AST node
//    SgNode* getNode() const {return node;}
//    //! An identifying index within the AST node given by getNode()
//    unsigned int getIndex() const {return index;}
//    //! Outgoing control flow edges from this node
//    std::vector<CFGEdge> outEdges() const;
//    //! Incoming control flow edges to this node
//    std::vector<CFGEdge> inEdges() const;
//    //! Test whether this node satisfies a (fairly arbitrary) standard for
//    //! "interestingness".  There are many administrative nodes in the raw CFG
//    //! (nodes that do not correspond to operations in the program), and this
//    //! function filters them out.
//    bool isInteresting() const;
//    //! Equality operator
//    bool operator==(const CFGNode& o) const {return node == o.node && index == o.index;}
//    //! Disequality operator
//    bool operator!=(const CFGNode& o) const {return !(*this == o);}
//    //! Less-than operator
//    bool operator<(const CFGNode& o) const {return node < o.node || (node == o.node && index < o.index);}
  }; // end class CFGNode



  class CFGEdge : public VirtualCFG::CFGEdge {

    public:

    //! Constructor
    //CFGEdge(CFGNode src, CFGNode tgt): src(src), tgt(tgt) { assert(src.getNode() != NULL && tgt.getNode() != NULL); }

    //! Default constructor. Used for compatibility with containers
    //CFGEdge() {}

//    //! Pretty string for Dot node labels, etc.
//    std::string toString() const;
//    //! String for debugging graphs
//    std::string toStringForDebugging() const;
//    //! ID to use for Dot, etc.
//    std::string id() const;
//    //! The source (beginning) CFG node
//    CFGNode source() const {return src;}
//    //! The target (ending) CFG node
//    CFGNode target() const {return tgt;}
//    //! The control flow condition that enables this edge
//    EdgeConditionKind condition() const;
//    //! The label of the case represented by an eckCaseLabel edge
//    SgExpression* caseLabel() const;
//    //! The expression of the computed goto represented by the eckArithmeticIf* conditions
//    unsigned int computedGotoCaseIndex() const;
//    //! The test or case key that is tested as a condition of this control flow edge
//    SgExpression* conditionBasedOn() const;
//    //! Variables going out of scope across this edge (not extensively tested)
//    std::vector<SgInitializedName*> scopesBeingExited() const;
//    //! Variables coming into scope across this edge (not extensively tested)
//    std::vector<SgInitializedName*> scopesBeingEntered() const;
//    //! Compare equality of edges
//    bool operator==(const CFGEdge& o) const {return src == o.src && tgt == o.tgt;}
//    //! Compare disequality of edges
//    bool operator!=(const CFGEdge& o) const {return src != o.src || tgt != o.tgt;}
//
//    //! operator<() has an arbitrary ordering, but allows these objects to be used with std::set and std::map
//    bool operator<(const CFGEdge& o) const {return src < o.src || (src == o.src && tgt < o.tgt);}

  }; // end CFGEdge




//
//
//  //! The first CFG node for a construct (before the construct starts to
//  //! execute)
//  inline CFGNode cfgBeginningOfConstruct(SgNode* c) {
//    return CFGNode(c, 0);
//  }
//
//  //! \internal Non-member wrapper for SgNode::cfgIndexForEnd since that has
//  //! not yet been defined
//  unsigned int cfgIndexForEndWrapper(SgNode* n);
//
//  //! The last CFG node for a construct (after the entire construct has finished
//  //! executing).  This node may not actually be reached if, for example, a goto
//  //! causes a loop to be exited in the middle
//  inline CFGNode cfgEndOfConstruct(SgNode* c) {
//    return CFGNode(c, cfgIndexForEndWrapper(c));
//  }
//
//  //! Returns CFG node for just before start
//  inline CFGNode makeCfg(SgNode* start) {
//    return cfgBeginningOfConstruct(start);
//  }
//
//
//
//  //! Utility function to make MPICFG Edges
//  template <class Node1T, class Node2T, class EdgeT>
//  void makeMPIEdge(Node1T from, Node2T to, std::vector<EdgeT>& result);

} // end namespace VirtualMPICFG

//#define SGFUNCTIONCALLEXP_INTERPROCEDURAL_INDEX 2
//#define SGCONSTRUCTORINITIALIZER_INTERPROCEDURAL_INDEX 1
//#define SGFUNCTIONDEFINITION_INTERPROCEDURAL_INDEX 2
//
//#define SGFUNCTIONCALLEXP_INTERPROCEDURAL_INDEX 2
//#define SGCONSTRUCTORINITIALIZER_INTERPROCEDURAL_INDEX 1
//#define SGFUNCTIONDEFINITION_INTERPROCEDURAL_INDEX 2
//
////! Utility function to make CFG Edges
//template <class NodeT1, class NodeT2, class EdgeT>
//void makeEdge(NodeT1 from, NodeT2 to, std::vector<EdgeT>& result);

#endif // VIRTUAL_MPICFG_H
