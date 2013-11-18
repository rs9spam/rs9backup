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
struct _Loop_Count_
{
  //! True if no or only over approximate information about the loop is available
  //
  //! Only over approximate information means, that the loop can be executed from
  //! 0 to infinity. So in this case the bound does not strike.
  bool over_approx_;
  //! The loop count is either an integer value or a fraction of the process number.
  //! This Count is represented by the struct bound.
  _Bound_ count_;

  _Loop_Count_()
  {
    this->over_approx_ = false;
    this->count_ = _Bound_(true,1,1,1);
  }

  //! default + parameterized constructor
  _Loop_Count_(bool oa)
  {
    this->over_approx_ = oa;
    this->count_ = _Bound_(true,1,1,1);;
  }

  //! default + parameterized constructor
  _Loop_Count_(bool oa, const _Bound_ b)
  {
    this->over_approx_ = oa;
    this->count_(b);
  }

  //! constructor
  _Loop_Count_(const _Loop_Count_& that)
  {
    this->over_approx_ = that.over_approx_;
    this->count_ = that.count_;
  }

  //! assignment operator
  _Loop_Count_& operator=(const _Loop_Count_& that)
  {
    this->over_approx_ = that.over_approx_;
    this->count_ = that.count_;
    return *this;
  }

  //! equality comparison. doesn't modify object. therefore constant.
  bool operator==(const _Loop_Count_& that) const
  {
    return(this->over_approx_ == that.over_approx_
           && this->count_ == that.count_);
  }

  //! equality comparison. doesn't modify object. therefore const.
  bool operator!=(const _Loop_Count_& that) const
  {
    return !(*this == that);
  }

  _Loop_Count_ operator*(const _Loop_Count_& that) const
  {
    if(this->over_approx_
       || that.over_approx_
       ||(!this->count_.abs && !that.count_.abs))
      return _Loop_Count_(true);
    if(this->count_.abs && that.count_.abs)
      return _Loop_Count_(false, _Bound_(true,1,1, this->count_.o * that.count_.o));
    if(this->count_.abs)
      return _Loop_Count_(false, _Bound_(false,
                                         that.count_.n * this->count_.o,
                                         that.count_.d,
                                         that.count_.o * this->count_.o));
    if(that.count_.abs)
          return _Loop_Count_(false, _Bound_(false,
                                             this->count_.n * that.count_.o,
                                             this->count_.d,
                                             this->count_.o * that.count_.o));
    return _Loop_Count_();
  }

  //! copy constructor
  _Loop_Count_& operator()(const _Loop_Count_& that)
  {
    *this = that;
    return *this;
  }

  //! copy constructor
  _Loop_Count_& copy(const _Loop_Count_& that)
  {
    *this = that;
    return *this;
  }

  /**
   * @brief Outputs a string representation of the loop count structure.
   */
  string toStr() const
  {
    ostringstream outs;
    outs << "[";
    (over_approx_) ? outs << "Z+" : outs << count_.toStr();
    //TODO: remove debug output
//    std::cerr << "ERROR OUTPUT: {";
//    (over_approx_) ? std::cerr << "TRUE " : std::cerr <<"FALSE ";
//    (over_approx_) ? std::cerr << "Z+" : std::cerr << count_.toStr();
//    std::cerr << "}";
    outs << "]";
    return outs.str();
  }
};

//=======================================================================================
//=======================================================================================
//=======================================================================================
/**
 *
 */
class LoopLattice : public FiniteLattice
{
protected:
  /**
   *
   */
  std::list<std::pair<SgNode*, _Loop_Count_> > loop_count_list_;

  /////////////////////////////////////////////////////////////////////////////////////
  ///////////////    to be initialized by init function   /////////////////////////////
  /**
   *
   */
  bool is_handled_loop_;
  /**
   *
   */
  SgNode* node_;
  /**
   *
   */
  SgNode* false_successor_;
  /**
   *
   */
  _Loop_Count_ lcs_;

//=======================================================================================
public:
  /**
   *
   */
  LoopLattice();

  /**
   *
   */
  LoopLattice( int v );

  /**
   *
   */
  LoopLattice(const LoopLattice & that);

  /**
   *
   */
  ~LoopLattice() {};

//=======================================================================================
public:
  /**
   * @brief Adds lcs to the list of _Loop_Count_s if it is not already there.
   * @param node SgNode* wich is a loop node.
   * @param lcs The loop count of the loop node @node.
   * @return True if _Loop_Count_ vector was modified, else false.
   *
   * Returns true, if the _Loop_Count_ list was modified.
   * Return false, if nothing modified because _Loop_Count_ already contained lcs.
   */
  bool addLoopCount(SgNode* node, _Loop_Count_ lcs);

  /**
   *
   */
  void pushBack(SgNode* node, _Loop_Count_ lcs);

  /**
   *
   */
  void popBack();

  /**
   *
   */
  std::pair<SgNode*, _Loop_Count_> getCountBack() const;

  /**
   * @brief Computes the product of all nested loops at a certain cfg node.
   */
  _Loop_Count_ getCountProduct() const;

  //=======================================================================================
  /**
   *
   */
  bool hasEqualList(const std::list<std::pair<SgNode*, _Loop_Count_> > l) const;
  /**
   *
   */
  bool hasEqualList(const LoopLattice* that) const;
  /**
   *
   */
  void copyList(const LoopLattice* that);
  /**
   *
   */
  void copyList(const std::list<std::pair<SgNode*, _Loop_Count_> > l);
  /**
   *
   */
  std::list<std::pair<SgNode*, _Loop_Count_> > getLoopCountList() const;
  /**
   *
   */
  std::list<std::pair<SgNode*, _Loop_Count_> > getFalseList() const;

//=======================================================================================
  /**
   *
   */
  bool isHandledLoop() const {return is_handled_loop_;};
  /**
   *
   */
  void isHandledLoop(bool handle) {is_handled_loop_ = handle;};
  /**
   *
   */
  SgNode* getNode() const {return node_;};
  /**
   *
   */
  void setNode(SgNode* node) {node_ = node;};
  /**
   *
   */
  SgNode* getFalseSuccessor() const {return false_successor_;};
  /**
   *
   */
  void setFalseSuccessor(SgNode* node) {false_successor_ = node;};

  /**
   * @brief Returns the nodes _Loop_Count_.
   * @return lcs_ is in case of a loop CFGNode the number of iterations of this loop.
   */
  _Loop_Count_ getLoopCount() const {return lcs_;};

  /**
   *
   */
  void setLoopCount(_Loop_Count_ lcs) {lcs_ = lcs;};

//=======================================================================================
public:
  // **********************************************
  // Required definition of pure virtual functions.
  // **********************************************
  /**
   * @brief Computes the meet of this and that and saves the result in this.
   * @returns True if this causes this to change and false otherwise.
   */
  bool meetUpdate(Lattice* X);

  /**
   *
   */
  void initialize();

  /**
   * @brief Returns a hard copy of this lattice.
   */
  Lattice* copy() const;

  /**
   * @brief Overwrites the state of "this" Lattice with "that" Lattice.
   */
  void copy(Lattice* that);

  /**
   * @brief Compares values from "this" Lattice to "that" Lattice.
   */
  bool operator==(Lattice* that) /*const*/;

  /**
   *
   */
  std::string str(std::string indent="");

//=======================================================================================
public:
  /**
   *
   */
  std::string toString() const;

  /**
   *
   */
  string toStringForDebugging() const;
};

#endif /* MPILOOPLATTICE_H_ */
