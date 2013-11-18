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
struct _Bound_
{
  bool abs;  //!< true if it is an absolute bound on the set
  int n;     //!< absolute value for bound numerator
  int d;     //!< denominator of relative bound
  int o;     //!< offset of relative higher bound or absolute bound value

  //! default + parameterized constructor
  _Bound_(bool abs = true, int n = 1, int d = 1, int o = 0)
      : abs(abs), n(n), d(d), o(o)
  {
  }

  //!
  _Bound_(int off)
  {
    abs = true;
    n = 1;
    d = 1;
    o = off;
  }

  //! assignment operator
  _Bound_& operator=(const _Bound_& b)
  {
      abs=b.abs;
      n=b.n;
      d=b.d;
      o=b.o;
      return *this;
  }

  //! add off to old offset
  _Bound_ operator+(const int& off) const
  {
    return _Bound_(abs, n, d, o + off);
  }

  //! subtract off from old offset
  _Bound_ operator-(const int& off) const
  {
    return _Bound_(abs, n, d, o - off);
  }

  //!
  _Bound_ operator-(const _Bound_& that) const
  {
    if(that.abs)
      return _Bound_(abs, n, d, o - that.o);
    return _Bound_(abs, n, d, o);
  }

  //! add off to old offset
  _Bound_& operator+=(const int& off)
  {
    o += off;
    return *this;
  }

  //! subtract off from old offset
  _Bound_& operator-=(const int& off)
  {
    o -= off;
    return *this;
  }

  //! equality comparison. doesn't modify object. therefore const.
  bool operator==(const _Bound_& b) const
  {
    if(abs == true && b.abs == true)
      return (o == b.o);
    else
      return (abs == b.abs && n == b.n && d == b.d && o == b.o);
  }

  //! equality comparison. doesn't modify object. therefore const.
  bool operator!=(const _Bound_& b) const
  {
      return !(*this == b);
  }

  //! less comparison.
  //
  //! Returns only true if 100% sure about the less fact.
  bool operator<(const _Bound_& b) const
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
  bool operator>(const _Bound_& b) const
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
  bool operator>=(const _Bound_& b) const
  {
    return (*this > b || *this == b);
  }

  //! less comparison.
  //
  //! Returns only true if 100% sure about the less fact.
  bool operator<=(const _Bound_& b) const
  {
    return (*this < b || *this == b);
  }

  //! copy constructor
  _Bound_& operator()(const _Bound_& b)
  {
    abs = b.abs;
    n = b.n;
    d = b.d;
    o = b.o;
    return *this;
  }

  //! copy constructor
  _Bound_& copy(const _Bound_& b)
  {
    abs = b.abs;
    n = b.n;
    d = b.d;
    o = b.o;
    return *this;
  }

  //! escape lower _Bound_ to String
  string toStr() const
  {
    ostringstream out;
    if(abs)
      out << o;
    else
    {
      (n != d)? out << n << "/" << d << "*p" : out << "p";
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
  _Bound_ lb_;    //!< represents the lower _Bound_ of the process set
  _Bound_ hb_;    //!< represents the higher _Bound_ of the proccess set
  bool empty_;  //!< true if the set is empty

public:
  /**
   * @brief PSet Constructor with empty flag, lower and upper _Bound_ as arguments.
   */
  PSet(bool empty, _Bound_ lb, _Bound_ hb);

  /**
   * @brief Default PSet Constructor which sets the empty flag and sets
   *        default bound values.
   */
  PSet();

  /**
   * @brief More intuitive Constructor for generating a single element PSet.
   * @param empty True if the PSet should represent an empty set.
   * @param value The absolute value for the lower and for the upper bound.
   *
   * This Constructor is provided because it is more intuitive than creating a PSet
   * with a lower and an upper bound which are the same. But it basically does only that.
   */
  PSet(bool empty, int value);
  /**
   * @brief Copy constructor.
   */
  PSet(const PSet& that);
//=======================================================================================
  /**
   * @brief Returns True if PSet is empty.
   */
  bool isEmpty() const;

  /**
   * @brief Returns true if lower bound is an absolute value.
   */
  bool isLbAbs() const;

  /**
   * @brief Returns true if upper bound is an absolute value.
   */
  bool isHbAbs() const;

  /**
   * @brief Returns true if the process set contains an absolute number of Processes.
   */
  bool isAbsProcNumber() const;

  /**
   * @brief Returns absolute number of processes.
   * @return 0 if empty, absolute value if the process set contains an absolute number
   *         of Processes, -1 if not.
   */
  int getAbsNumProc() const;

  /**
   * @brief Returns true if the minimum bound of the Set is 0.
   */
  bool isMinBound() const;

  /**
   * @brief Returns true if the maximum bound of the Set is number of processes - 1.
   */
  bool isMaxBound() const;

  /**
   * @brief Returns true if 100% sure about the decision that bound is within the PSet.
   */
  bool contains(const _Bound_& b) const;

  /**
   * @brief Returns true if this PSet contains the elements of that PSet.
   */
  bool contains(const PSet& that) const;

  /**
   * @brief Return true if the two sets intersect or touch.
   */
  bool interleavesOrTouches(const PSet& that) const;
//=======================================================================================
  /**
   * @brief Returns the Union of PSet this and p in case they intersect or touch or
   *        returns PSet this otherwise.
   * @param that
   * @return
   */
  PSet combineWith(const PSet& that) const;

  /**
   * @brief Returns PSet this intersected with PSet p.
   */
  PSet intersectWith(const PSet& that) const;

  /**
   * @brief Removes that PSet from this PSet and returns the resulting PSet or PSets.
   * @param that PSet to be removed from this PSet.
   * @return One or two PSets that represent this without that.
   *
   * If this or that is empty, the result is that.
   * May return one or two process sets in the return vector.
   * Two Sets are possible, if that is a subset of A and not reaching the lower or
   * the upper bound.
   */
  std::vector<PSet> remove(const PSet& that) const;
//=======================================================================================
  /**
   * @brief Get function for upper bound.
   */
  _Bound_ getHBound() const;

  /**
   * @brief Get function for lower bound of process set.
   */
  _Bound_ getLBound() const;

  /**
   * @brief Sets lower bound to bound b.
   * @param b New value for the lower bound.
   */
  void setLBound(const _Bound_& b);

  /**
   * @brief Sets upper bound to bound b.
   * @param b New value for the upper bound of the process set.
   */
  void setHBound(const _Bound_& b);

  //=======================================================================================
  /**
   * @brief
   */
  bool operator==(const PSet& that) const;

  /**
   * @brief
   */
  bool operator!=(const PSet& that) const;

  /**
   * @brief Checks if this->lb_ < that->lb_.
   */
  bool operator<(const PSet& that) const;

  /**
   * @brief Checks if this->hb_ > that->hb_.
   */
  bool operator>(const PSet& that) const;

  /**
   * @brief
   */
  bool operator<=(const PSet& that) const;

  /**
   * @brief
   */
  bool operator>=(const PSet& that) const;

  /**
   * @brief Copy from that to this.
   */
  bool operator=(const PSet& that);

  /**
   * @brief Hard copy from that to this.
   */
  bool copy(const PSet& that);

//=======================================================================================
  // ************************
  // *** NodeFact methods ***
  // ************************

  /**
   * @brief
   */
  NodeFact* copy() const;

  /**
   * @brief Returns a string representation of this set.
   */
  string toString() const;

  /**
   * @brief Returns a string representation of this set.
   */
  string str(string indent="");
};
#endif
