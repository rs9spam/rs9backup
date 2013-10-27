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
    if(abs == true && b.abs == true)
      return (o == b.o);
    else
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
    if(*this == b)
      return false;
    if(abs && b.abs)
      return (o < b.o);
    if(abs)
      return true;
    if(b.abs)
      return false;

#if 0
    //both abs flags are false at this point
    if(o == b.o)
    {
      if( ((float)n/(float)d) < ((float)b.n/(float)b.d) )
        return true;
      if( ((float)n/(float)d) >= ((float)b.n/(float)b.d) )
        return false;
    }
    if( ((float)n/(float)d) == ((float)b.n/(float)b.d) )
    {
      if(o < b.o)
        return true;
    }
//    //true if this is Min Bound
//    if(abs && o == 0)
//      return true;
    if(!abs && n == 0 && o == 0)  //this case should not exist but can be handled
      return true;
    //true if that is Max Bound
    if(!b.abs && b.d != 0 && b.n == b.d && b.o == -1)
      return true;
    return false;
#else
    //both abs flags are false at this point
    if( ((float)n/(float)d) < ((float)b.n/(float)b.d) )
      return true;

    if( ((float)n/(float)d) == ((float)b.n/(float)b.d) )
      if(o < b.o)
        return true;

    return false;
#endif
  }

  //! less comparison.
  //
  //! Returns only true if 100% sure about the less fact.
  bool operator>(const bound& b) const
  {
    return !(*this < b || *this == b);
#if 0
    if(*this == b)
      return false;
    if(abs && b.abs)
      return (o > b.o);
    if(abs)
      return false;
    if(b.abs)
      return true;

    if(o == b.o)
    {
      if( ((float)n/(float)d) > ((float)b.n/(float)b.d) )
        return true;
      if( ((float)n/(float)d) < ((float)b.n/(float)b.d) )
        return false;
    }
    if( ((float)n/(float)d) == ((float)b.n/(float)b.d) )
    {
      if(o > b.o)
        return true;
    }
    //true if that is Min Bound
    if(b.abs && b.o == 0)
      return true;
    if(!b.abs && b.n == 0 && b.o == 0)  //this case should not exist but can be handled
      return true;
    //true if that is Max Bound
    if(!abs && d != 0 && n == d && o == -1)
      return true;
    return false;
#endif
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

  //! escape lower bound to String
  string toStr() const
  {
    ostringstream out;
    if(abs)
      out << o;
    else
    {
      out << n << "/" << d << "*p";
      (o >= 0) ? out << "+" : out << "";
      out << o;
    }
    return out.str();
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
  //!
  PSet(bool empty, int value);
  //! Copy constructor
  PSet(const PSet& that);
//=======================================================================================
  //! Returns True if PSet is empty.
  bool isEmpty() const;
  //! Returns true if lower bound is an absolute value.
  bool isLbAbs() const;
  //! Returns true if upper bound is an absolute value.
  bool isHbAbs() const;
  //!
  bool isAbsProcNumber() const;
  //!
  int getAbsNumProc() const;
  //! returns true if the minimum bound of the Set is 0
  bool isMinBound() const;
  //! returns true if the maximum bound of the Set is number of processes - 1
  bool isMaxBound() const;
  //! Returns true if 100% sure about the decision that bound is within the PSet
  bool contains(bool abs, int num, int den, int off) const;
  //! Returns true if 100% sure about the decision that bound is within the PSet
  bool contains(const bound& b) const;
  //! Return true if the two sets intersect or touch.
  bool interleavesOrTouches(const PSet& that) const;
//=======================================================================================
  //! Returns the Union of PSet this and p in case they intersect or touch or returns
  //! PSet this otherwise.
  //
  //!
  PSet combineWith(const PSet& that) const;
  //! Returns PSet this intersected with PSet p.
  //
  //! Returns
  PSet intersectWith(const PSet& that) const;
  //! Removes that PSet from this PSet and returns the resulting PSet or PSets.
  //
  //! If this or that is empty, the result is that.
  //! May return one or two process sets in the return vector.
  //! Two Sets are possible, if that is a subset of A and not reaching the lower or
  //! the upper bound.
  std::vector<PSet> remove(const PSet& that) const;
//=======================================================================================
#if 0
  int getLnum() const;
  int getHnum() const;
  int getLden() const;
  int getHden() const;
  int getLoff() const;
  int getHoff() const;
#endif
  bound getHBound() const;
  bound getLBound() const;
#if 0
  void setLnum(const int& lnum);
  void setHnum(const int& hnum);
  void setLden(const int& lden);
  void setHden(const int& hden);
  void setLoff(const int& loff);
  void setHoff(const int& hoff);
#endif
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
  string toString() const;
  //! Returns a string representation of this set
  string str(string indent="");
};
#endif
