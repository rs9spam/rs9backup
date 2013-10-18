#ifndef P_SET_H
#define P_SET_H

//#include "genericDataflowCommon.h"
//#include "cfgUtils.h"
//#include "variables.h"
//#include "ConstrGraph.h"

#include "nodeState.h"
#include <string>

//! Bound structure to represent absolute and relative bounds.
//
//! Bound is represented either as an absolute bound. In this case the value for
//! abs is true and the absolute bound value is stored in o.
//! In the case of a relative bound, the bound is represented as a fraction of
//! the maximum number of processes plus a value for an offset.
//! bound = number_of_processes * n / d + o.
struct bound
{
  bool abs;  //!< true if it is an absolute bound on the set
  int n;     //!< absolute value for bound numerator
  int d;     //!< denominator of relative bound
  int o;     //!< offset of relative higher bound or absolute bound value

  //! default + parameterized constructor
  bound(bool abs = true, int n = 1, int d = 1, int o = 0)
      : abs(abs), n(n), d(d), o(o)
  {
  }

  //! assignment operator
  bound& operator=(const bound& b)
  {
      abs=b.abs;
      n=b.n;
      d=b.d;
      o=b.o;
      return *this;
  }

  //! add off to old offset
  bound operator+(const int& off) const
  {
    return bound(abs, n, d, o + off);
  }

  //! subtract off from old offset
  bound operator-(const int& off) const
  {
    return bound(abs, n, d, o - off);
  }

  //! add off to old offset
  bound& operator+=(const int& off)
  {
    o += off;
    return *this;
  }

  //! subtract off from old offset
  bound& operator-=(const int& off)
  {
    o -= off;
    return *this;
  }

  //! equality comparison. doesn't modify object. therefore const.
  bool operator==(const bound& b) const
  {
      return (abs == b.abs && n == b.n && d == b.d && o == b.o);
  }

  //! equality comparison. doesn't modify object. therefore const.
  bool operator!=(const bound& b) const
  {
      return !(*this == b);
  }

  //! less comparison.
  //
  //! Returns only true if 100% sure about the less fact.
  bool operator<(const bound& b) const
  {
    if(*this ==b)
      return false;
    if(abs && b.abs)
      return (o < b.o);
    if(abs)
      return true;
    if(b.abs)
      return false;

    if(o == b.o)
    {
      if( ((float)n/(float)d) < ((float)b.n/(float)b.d) )
        return true;
      if( ((float)n/(float)d) > ((float)b.n/(float)b.d) )
        return false;
    }
    if( ((float)n/(float)d) == ((float)b.n/(float)b.d) )
    {
      if(o < b.o)
        return true;
    }
    //true if this is Min Bound
    if(abs && o == 0)
      return true;
    if(!abs && n == 0 && o == 0)  //this case should not exist but can be handled
      return true;
    //true if that is Max Bound
    if(!b.abs && b.d != 0 && b.n == b.d && b.o == -1)
      return true;
    return false;
  }

  //! less comparison.
  //
  //! Returns only true if 100% sure about the less fact.
  bool operator>(const bound& b) const
  {
    return !(*this < b || *this == b);
//    if(*this == b)
//      return false;
//    if(abs && b.abs)
//      return (o > b.o);
//    if(abs)
//      return false;
//    if(b.abs)
//      return true;
//
//    if(o == b.o)
//    {
//      if( ((float)n/(float)d) > ((float)b.n/(float)b.d) )
//        return true;
//      if( ((float)n/(float)d) < ((float)b.n/(float)b.d) )
//        return false;
//    }
//    if( ((float)n/(float)d) == ((float)b.n/(float)b.d) )
//    {
//      if(o > b.o)
//        return true;
//    }
//    //true if that is Min Bound
//    if(b.abs && b.o == 0)
//      return true;
//    if(!b.abs && b.n == 0 && b.o == 0)  //this case should not exist but can be handled
//      return true;
//    //true if that is Max Bound
//    if(!abs && d != 0 && n == d && o == -1)
//      return true;
//    return false;
  }

  //! less comparison.
  //
  //! Returns only true if 100% sure about the less fact.
  bool operator>=(const bound& b) const
  {
    return (*this > b || *this == b);
  }

  //! less comparison.
  //
  //! Returns only true if 100% sure about the less fact.
  bool operator<=(const bound& b) const
  {
    return (*this < b || *this == b);
  }

  //! copy constructor
  bound& operator()(const bound& b)
  {
    abs = b.abs;
    n = b.n;
    d = b.d;
    o = b.o;
    return *this;
  }

  //! copy constructor
  bound& copy(const bound& b)
  {
    abs = b.abs;
    n = b.n;
    d = b.d;
    o = b.o;
    return *this;
  }
};

//=======================================================================================
class PSet : public NodeFact
{
protected:
  bound lb_;    //!< represents the lower bound of the process set
  bound hb_;    //!< represents the higher bound of the proccess set
  bool empty_;  //!< true if the set is empty

public:
  //! PSet Constructor with empty flag and all bound values as single arguments.
  PSet(bool empty,
       bool lb_abs, bool hb_abs,
       int lnum, int hnum,
       int lden, int hden,
       int loff, int hoff);
  //! PSet Constructor with empty flag, lower and upper bound as arguments .
  PSet(bool empty, bound lb, bound hb);
  //! Default PSet Constructor which sets the empty flag and sets default bound values.
  PSet();
//=======================================================================================
  //! Returns True if PSet is empty.
  bool isEmpty();
  //! Returns true if lower bound is an absolute value.
  bool isLbAbs();
  //! Returns true if upper bound is an absolute value.
  bool isHbAbs();
  //!
  bool isAbsProcNumber();
  //!
  int getAbsNumProc();
  //! returns true if the minimum bound of the Set is 0
  bool isMinBound();
  //! returns true if the maximum bound of the Set is number of processes - 1
  bool isMaxBound();
  //! Returns true if 100% sure about the decision that bound is within the PSet
  bool contains(bool abs, int num, int den, int off);
  //! Returns true if 100% sure about the decision that bound is within the PSet
  bool contains(bound b);
  //! Return true if the two sets intersect or touch.
  bool interleavesOrTouches(const PSet& that) const;
//=======================================================================================
  //! Returns the Union of PSet this and p in case they intersect or touch or returns
  //! PSet this otherwise.
  //
  //!
  PSet combineWith(const PSet& p) const;
  //! Returns PSet this intersected with PSet p.
  PSet intersectWith(const PSet& p) const;
//=======================================================================================
  int getLnum() const;
  int getHnum() const;
  int getLden() const;
  int getHden() const;
  int getLoff() const;
  int getHoff() const;
  bound getHBound() const;
  bound getLBound() const;
  void setLnum(const int& lnum);
  void setHnum(const int& hnum);
  void setLden(const int& lden);
  void setHden(const int& hden);
  void setLoff(const int& loff);
  void setHoff(const int& hoff);
  void setLow(bool abs, int num, int den, int off);
  void setHigh(bool abs, int num, int den, int off);
  void setLBound(const bound& b);
  void setHBound(const bound& b);
//=======================================================================================
  //!
  bool operator==(const PSet& that) const;
  //!
  bool operator!=(const PSet& that) const;
  //! checks if this->lb_ < that->lb_
  bool operator<(const PSet& that) const;
  //! checks if this->hb_ > that->hb_
  bool operator>(const PSet& that) const;
  //!
  bool operator<=(const PSet& that) const;
  //!
  bool operator>=(const PSet& that) const;
  //! copy from that to this
  bool operator=(const PSet& that);
  //! hard copy from that to this
  bool copy(const PSet& that);

  // ************************
  // *** NodeFact methods ***
  // ************************
//=======================================================================================
  //!
  NodeFact* copy() const;
  //! Returns a string representation of this set
  string str(string indent="") const;
  //! Returns a string representation of this set
  string str(string indent="");
};

#endif

#if 0
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

//};

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
