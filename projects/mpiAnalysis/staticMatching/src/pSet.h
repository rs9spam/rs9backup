#ifndef P_SET_H
#define P_SET_H

//#include "genericDataflowCommon.h"
//#include "cfgUtils.h"
//#include "variables.h"
//#include "ConstrGraph.h"

#include "nodeState.h"
#include <string>


//=======================================================================================
class PSet : public NodeFact
{
private:
  bool empty_range;  // true if the set is empty
  bool lb_abs;
  bool hb_abs;   // true if it is an absolute higher bound on the set
  int lnum;
  int hnum;     // absolute value for high bound numerator
  int lden;
  int hden;   // denominator of relative higher bound
  int loff;
  int hoff;   // offset of relative higher bound  (hb = hnum * rank / hden + hoff)

public:

  PSet(bool empty_range,
       bool lb_abs, bool hb_abs,
       int lnum, int hnum,
       int lden, int hden,
       int loff, int hoff);

  PSet();


  bool isEmpty();
  bool isLbAbs();
  bool isHbAbs();
  bool isAbsProcNumber();
  int getNumProc();

  //=======================================================================================
  int getLnum();
  int getHnum();
  int getLden();
  int getHden();
  int getLoff();
  int getHoff();

  void setLnum( int lnum);
  void setHnum( int hnum);
  void setLden( int lden);
  void setHden( int hden);
  void setLoff( int loff);
  void setHoff( int hoff);
  void setLow(bool abs, int num, int den, int off);
  void setHigh(bool abs, int num, int den, int off);

  //=======================================================================================
  bool compareHbSmallerLb(const PSet& that ) const;


  //=======================================================================================
  bool contains(bool abs, int num, int den, int off);
  bool isUnder(bool abs, int num, int den, int off);
  bool isOver(bool abs, int num, int den, int off);

  //=======================================================================================
  //checks if this->lb < that->lb
  bool operator<(const PSet& that) const;
  //checks if this->hb > that->hb
  bool operator>(const PSet& that) const;

  bool copy(const PSet& that);

  // ************************
  // *** NodeFact methods ***
  // ************************
  //=======================================================================================
  NodeFact* copy() const;

  // Returns a string representation of this set
  string str(string indent="") const;

  // Returns a string representation of this set
  string str(string indent="");





















//  static const int infinity = -1;
//
//  // Returns a heap-allocated reference to the intersection of this and that
//  pSet& intersect(const pSet& that) const;
//
//  // Updates this with the intersection of this and that
//  // returns true if this causes the pSet to change, false otherwise
//  bool intersectUpd(const pSet& that);
//
//  // Returns a heap-allocated reference to this - that
//  pSet& rem(const pSet& that) const;
//
//  // Updates this with the result of this - that
//  // returns true if this causes the pSet to change, false otherwise
//  bool remUpd(const pSet& that);
//
//  // Returns true if this set is empty, false otherwise
//  bool emptySet() const;
//
//  // Returns true if this is a representable set, false otherwise
//  bool validSet() const;
//
//  // The size of this process set, either a specific integer or infinity
//  int size() const;
//
//  // Assigns this to that
//  //virtual pSet& operator=(const pSet& that)=0;
//
//  // Comparison
//  bool operator==(const pSet& that) const;
//
//  bool operator!=(const pSet& that) const
//  { return !(*this == that); }
//
//  bool operator<(const pSet& that) const;

};

//class contPSet : public virtual pSet
//{
//
//};

//class emptyPSet : public virtual pSet
//{
//        public:
//
//        // Returns a heap-allocated reference to the intersection of this and that
//        pSet& intersect(const pSet& that) const
//        { return *(new emptyPSet()); }
//
//        // Updates this with the intersection of this and that
//        // returns true if this causes the pSet to change, false otherwise
//        bool intersectUpd(const pSet& that)
//        { return false; }
//
//        // Returns a heap-allocated reference to this - that
//        pSet& rem(const pSet& that) const
//        { return *(new emptyPSet()); }
//
//        // Updates this with the result of this - that
//        // returns true if this causes the pSet to change, false otherwise
//        bool remUpd(const pSet& that)
//        { return false; }
//
//        // Returns true if this set is empty, false otherwise
//        bool emptySet() const
//        { return true; }
//
//        // Returns true if this is a representable set, false otherwise
//        bool validSet() const
//        { return true; }
//
//        // The size of this process set, either a specific integer or infinity
//        int size() const
//        { return 0; }
//
//        // Assigns this to that
//        pSet& operator=(const pSet& that)
//        { ROSE_ASSERT(0); }
//
//        // Comparison
//        bool operator==(const pSet& that) const
//        { ROSE_ASSERT(0); }
//
//        bool operator<(const pSet& that) const
//        { return false; }
//
//        // Returns a string representation of this set
//        string str(string indent)
//        {
//                ostringstream outs;
//                outs << indent << "[emptyPSet]";
//                return outs.str();
//        }
//
//        string str(string indent) const
//        {
//                ostringstream outs;
//                outs << indent << "[emptyPSet]";
//                return outs.str();
//        }
//
//        // NodeFact methods
//
//        // returns a copy of this node fact
//        NodeFact* copy() const
//        { return new emptyPSet(); }
//};
//
//class invalidPSet : public virtual pSet
//{
//        public:
//        // Returns a heap-allocated reference to the intersection of this and that
//        pSet& intersect(const pSet& that) const
//        { return *(new invalidPSet()); }
//
//        // Updates this with the intersection of this and that
//        // returns true if this causes the pSet to change, false otherwise
//        bool intersectUpd(const pSet& that)
//        { return false; }
//
//        // Returns a heap-allocated reference to this - that
//        pSet& rem(const pSet& that) const
//        { return *(new invalidPSet()); }
//
//        // Updates this with the result of this - that
//        // returns true if this causes the pSet to change, false otherwise
//        bool remUpd(const pSet& that)
//        { return false; }
//
//        // Returns true if this set is empty, false otherwise
//        bool emptySet() const
//        { return false; }
//
//        // Returns true if this is a representable set, false otherwise
//        bool validSet() const
//        { return false; }
//
//        // The size of this process set, either a specific integer or infinity
//        int size() const
//        { return 0; }
//
//        // Assigns this to that
//        pSet& operator=(const pSet& that)
//        { ROSE_ASSERT(0); }
//
//        // Comparison
//        bool operator==(const pSet& that) const
//        { ROSE_ASSERT(0); }
//
//        bool operator<(const pSet& that) const
//        { return false; }
//
//        // Returns a string representation of this set
//        string str(string indent) //const
//        {
//                ostringstream outs;
//                outs << indent << "[invalidPSet]";
//                return outs.str();
//        }
//
//        string str(string indent) const
//        {
//                ostringstream outs;
//                outs << indent << "[invalidPSet]";
//                return outs.str();
//        }
//
//        // NodeFact methods
//
//        // returns a copy of this node fact
//        NodeFact* copy() const
//        { return new invalidPSet(); }
//};
//
//// Process sets that can be represented as contiguous ranges of ranks
//class contRangePSet : public virtual pSet
//{
//        // The variables that represent the lower and upper bounds of the range of ranks
//        // The range is [lb ... ub], inclusive.
//        varID lb;
//        varID ub;
//        // nodeConstant constraints on lb and ub
//        ConstrGraph* cg;
//        // true if this represents an empty range
//        bool emptyRange;
//        // true if this is a representable set, false otherwise
//        bool valid;
//
//        // The number of variables that have been generated as range lower and upper bounds
//        static int varCount;
//
//        varID genFreshVar(string prefix="");
//
//        public:
//        // Creates an empty set
//        contRangePSet(bool emptyRange=true);
//
//        contRangePSet(varID lb, varID ub, ConstrGraph* cg);
//
//        // this->lb*lbA = lb*lbB + lbC
//        // this->ub*ubA = ub*ubB + ubC
//        contRangePSet(varID lb, int lbA, int lbB, int lbC,
//                         varID ub, int ubA, int ubB, int ubC,
//                         ConstrGraph* cg);
//        contRangePSet(varID lb, int lbA, int lbB, int lbC,
//                         varID ub, int ubA, int ubB, int ubC,
//                         string annotName, void* annot,
//                         ConstrGraph* cg);
//
//        // if freshenVars==true, calls genFreshBounds() to make this set use different lower and upper
//        // bound variables from that set, while ensuring that the bound variables themselves are
//        // equal to each other in cg
//        contRangePSet(const contRangePSet& that, bool freshenVars=false);
//
//        //~contRangePSet();
//
//        // copies that to this, returning true if this is not changed and false otherwise
//        bool copy(const contRangePSet& that);
//
//        // Return the current value of lb, ub and cg
//        const varID& getLB() const;
//        const varID& getUB() const;
//        ConstrGraph* getConstr() const;
//
//        // Set lb, ub and cg, returning true if this causes this process set to change
//        // and false otherwise
//        bool setLB(const varID& lb);
//        bool setUB(const varID& ub);
//        bool setConstr(ConstrGraph* cg);
//
//        // Do not modify the upper or lower bound but add the constraint that it is equal lb/ub:
//        // lb*a = newLB*b + c, ub*a = newUB*b + c
//        bool assignLB(const varID& lb, int a=1, int b=1, int c=0);
//        bool assignUB(const varID& ub, int a=1, int b=1, int c=0);
//
//        // Asserts all the set-specific invariants in this set's constraint graph
//        void refreshInvariants();
//
//        // Generates new lower and upper bound variables that are set to be equal to the original
//        // lower and upper bound variables in this pSet's constraint graph as:
//        // newLB*lbA = oldLB*lbB + lbC and newUB*ubA = oldUB*ubB + ubC
//        // If annotName!="", the bounds variables will have the annotation annotName->annot
//        void genFreshBounds(int lbA=1, int lbB=1, int lbC=0,
//                            int ubA=1, int ubB=1, int ubC=0,
//                            string annotName="", void* annot=NULL);
//
//        // Transition from using the current constraint graph to using newCG, while annotating
//        // the lower and upper bound variables with the given annotation annotName->annot.
//        // Return true if this causes this set to change, false otherwise.
//        bool setConstrAnnot(ConstrGraph* newCG, string annotName, void* annot);
//
//        // Add the given annotation to the lower and upper bound variables
//        // Return true if this causes this set to change, false otherwise.
//        bool addAnnotation(string annotName, void* annot);
//
//        // Remove the given annotation from this set's lower and upper bound variables
//        // Return true if this causes this set to change, false otherwise.
//        bool removeVarAnnot(string annotName);
//
//        // Make this set invalid, return true if this causes the set to change, false otherwise
//        bool invalidate();
//
//        // Make this set valid, return true if this causes the set to change, false otherwise
//        bool makeValid();
//
//        // Make this set empty, return true if this causes the set to change, false otherwise.
//        // Also, remove the set's lower and upper bounds from its associated constraint graph (if any).
//        bool emptify();
//
//        // Make this set non-empty, return true if this causes the set to change, false otherwise
//        bool makeNonEmpty();
//
//        // Returns true if this and that represent equal ranges
//        bool rangeEq(const contRangePSet& that) const;
//
//        // Returns true if that is at the top of but not equal to this's range
//        bool rangeTop(const contRangePSet& that) const;
//
//        // Returns true if that is at the bottom of but not equal to this's range
//        bool rangeBottom(const contRangePSet& that) const;
//
//        // Returns true if the ranges of this and that must be disjoint
//        bool rangeDisjoint(const contRangePSet& that) const;
//
//        // Returns true if this must overlap, with this Below that: lb <= that.lb <= ub <= that.ub
//        bool overlapAbove(const contRangePSet& that) const;
//
//        // Returns true if this must overlap, with this Above that: that.lb <= lb <= that.ub <= ub
//        bool overlapBelow(const contRangePSet& that) const;
//
//        // Copies the given variable's constraints from srcCG to cg.
//        // !!! May want to label the inserted variables with this pSet object to avoid collisions
//        // !!! with other pSet objects.
//        void copyVar(ConstrGraph* srcCG, const varID& var);
//
//        // returns a heap-allocated reference to the intersection of this and that
//        // if transClosure==true, performs the appropriate (possibly optimized) transitive closure on cg
//        pSet& intersect(const pSet& that) const;
//        pSet& intersect(const contRangePSet& that, bool transClosure=false) const;
//
//        // updates this with the intersection of this and that
//        // returns true if this causes the pSet to change, false otherwise
//        // if transClosure==true, performs the appropriate (possibly optimized) transitive closure on cg
//        bool intersectUpd(const pSet& that);
//        bool intersectUpd(const contRangePSet& that, bool transClosure=false);
//
//        // Returns a heap-allocated reference to this - that
//        // if transClosure==true, performs the appropriate (possibly optimized) transitive closure on cg
//        pSet& rem(const pSet& that) const;
//        pSet& rem(const contRangePSet& that , bool transClosure=false) const;
//
//        // Updates this with the result of this - that
//        // returns true if this causes the pSet to change, false otherwise
//        // if transClosure==true, performs the appropriate (possibly optimized) transitive closure on cg
//        bool remUpd(const pSet& that);
//        bool remUpd(const contRangePSet& that, bool transClosure=false);
//
//        // Returns true if this set is empty, false otherwise
//        bool emptySet() const;
//
//        // Returns true if this is a representable set, false otherwise
//        bool validSet() const;
//
//        // The size of this process set, either a specific integer or infinity
//        int size() const;
//
//        // Assigns this to that
//        pSet& operator=(const pSet& that);
//
//        // Comparison
//        bool operator==(const pSet& that) const;
//
//        bool operator<(const pSet& that) const;
//
//        // Returns a string representation of this set
//        string str(string indent="") /*const*/;
//        string str(string indent="") const;
//
//        // NodeFact methods
//
//        // returns a copy of this node fact
//        NodeFact* copy() const;
//
//        // Removes the upper and lower bounds of this set from its associated constraint graph
//        // or the provided constraint graph (if it is not ==NULL) and sets cg to NULL if tgtCG==NULL.
//        // Returns true if this causes the constraint graph to change, false otherwise.
//        bool cgDisconnect();
//        bool cgDisconnect(ConstrGraph* tgtCG) const;
//};

#endif
