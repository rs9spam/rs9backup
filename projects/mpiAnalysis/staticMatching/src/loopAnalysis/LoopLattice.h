/*
 * RankLattice.h
 *
 *  Created on: Oct 19, 2013
 *      Author: stoero
 */

#ifndef MPILOOPLATTICE_H_
#define MPILOOPLATTICE_H_

#include "rose.h"
#include "latticeFull.h"
#include "rankAnalysis/pSet.h"

//=======================================================================================
//=======================================================================================
//=======================================================================================
//!
//
//!
struct loopCountStruct
{
  //! True if no or only over approximate information about the loop is available
  //
  //! Only over approximate information means, that the loop can be executed from
  //! 0 to infinity. So in this case the bound does not strike.
  bool over_approx_;
  //! The loop count is either an integer value or a fraction of the process number.
  //! This Count is represented by the struct bound.
  const bound count_;

  //! default + parameterized constructor
  loopCountStruct(const bool oa = false, const bound b = bound(true,1,1,1))
  {
    this->over_approx_ = oa;
    this->count_ = b;
  }

  //! constructor
  loopCountStruct(const loopCountStruct& that)
  {
    this->over_approx_ = that.over_approx_;
    this->count_ = that.count_;
  }

  //! assignment operator
  loopCountStruct& operator=(const loopCountStruct& that)
  {
    this->over_approx_ = that.over_approx_;
    this->count_ = that.count_;
    return *this;
  }

  //! equality comparison. doesn't modify object. therefore constant.
  bool operator==(const loopCountStruct& that) const
  {
    return(this->over_approx_ == that.over_approx_
           && this->count_ == that.count_);
  }

  //! equality comparison. doesn't modify object. therefore const.
  bool operator!=(const loopCountStruct& that) const
  {
    return !(*this == that);
  }

  loopCountStruct operator*(const loopCountStruct& that) const
  {
    if(this->over_approx_
       || that.over_approx_
       ||(!this->count_.abs && !that.count_.abs))
      return loopCountStruct(true);
    if(this->count_.abs && that.count_.abs)
      return loopCountStruct(false, bound(true,1,1, this->count_.o * that.count_.o));
    if(this->count_.abs)
      return loopCountStruct(false, bound(true,
                                          this->count_.n * that.count_.o,
                                          this->count_.d,
                                          this->count_.o * that.count_.o));
    if(that.count_.abs)
          return loopCountStruct(false, bound(true,
                                              that.count_.n * this->count_.o,
                                              that.count_.d,
                                              that.count_.o * this->count_.o));
    return loopCountStruct();
  }

  //! copy constructor
  loopCountStruct& operator()(const loopCountStruct& that)
  {
    *this = that;
    return *this;
  }

  //! copy constructor
  loopCountStruct& copy(const loopCountStruct& that)
  {
    *this = that;
    return *this;
  }

  //! outputs a string representation of the loop count structure.
  string toStr() const
  {
    ostringstream outs;
    outs << "[";
    outs << (this->over_approx_) ? "Z+" : this->count_.toStr();
    outs << "]";
    return outs.str();
  }
};

//=======================================================================================
//=======================================================================================
//=======================================================================================
//!
//!
class LoopLattice : public FiniteLattice
{
protected:
  //! ..
  //
  //!
  std::list<std::pair<CFGNode*, loopCountStruct> > loop_count_list_;

//=======================================================================================
public:
  //!
  LoopLattice();

  //!
  LoopLattice( int v );

  //!
  //
  //!
  LoopLattice(const LoopLattice & that);

  //!
  ~LoopLattice() {};

//=======================================================================================
public:
  //!
  void pushBack(const CFGNode* cfgn, const loopCountStruct lcs);
  //!
  void popBack();
  //!
  std::pair<CFGNode*, loopCountStruct> getCountBack() const;
  //!
  loopCountStruct getCountProduct() const;

//=======================================================================================
public:
  // **********************************************
  // Required definition of pure virtual functions.
  // **********************************************
  //! computes the meet of this and that and saves the result in this
  //! returns true if this causes this to change and false otherwise
  bool meetUpdate(Lattice* X);

  //!
  void initialize();

  //! returns a hard copy of this lattice
  Lattice* copy() const;

  //! overwrites the state of "this" Lattice with "that" Lattice
  void copy(Lattice* that);

  //! compares values from "this" Lattice to "that" Lattice
  bool operator==(Lattice* that) /*const*/;
  //!
  std::string str(std::string indent="");

//=======================================================================================
public:
  //!
  std::string toString() const;
  //!
  string toStringForDebugging() const;
};

#endif /* MPILOOPLATTICE_H_ */
