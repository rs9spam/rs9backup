/*
 * CallLattice.h
 *
 *  Created on: Nov 10, 2013
 *      Author: stoero
 */

#ifndef MPICALLLATTICE_H_
#define MPICALLLATTICE_H_

#include "rose.h"
#include "latticeFull.h"
#include "rankAnalysis/pSet.h"
#include "loopAnalysis/LoopLattice.h"

//=======================================================================================
//=======================================================================================
//=======================================================================================
//!
//
//!
struct _Call_List_
{
  /*bool over_approx_;*/
  //!< point after which possible branching occured.
  int unease_middle_;
  //!< calls list. //maybe we want SgNode* stored here.
  std::vector<std::pair<CFGNode, _Loop_Count_> > calls_;

  /**
   * @brief create empty call list.
   */
  _Call_List_()
  {
    unease_middle_ = -1;
    calls_.clear();
  }

  //! default + parameterized constructor
  _Call_List_(std::vector<std::pair<CFGNode, _Loop_Count_> > calls, int um = -1)
      /*, bool oa = false*/
  {
//    over_approx_ = oa;
    unease_middle_ = um;
    calls_.assign(calls.begin(), calls.end());
  }

  //! assignment operator
  _Call_List_& operator=(const _Call_List_& that)
  {
//    this->over_approx_ = that.over_approx_;
    this->unease_middle_ = that.unease_middle_;
    this->calls_.assign(that.calls_.begin(), that.calls_.end());
    return *this;
  }

  //! equality comparison. doesn't modify object. therefore const.
  bool operator==(const _Call_List_& that) const
  {
      return (/*this->over_approx_ == that.over_approx_
              &&*/ this->unease_middle_ == that.unease_middle_
              && this->calls_ == that.calls_);
  }

  //! equality comparison. doesn't modify object. therefore const.
  bool operator!=(const _Call_List_& that) const
  {
      return !(*this == that);
  }

  //! copy constructor
  _Call_List_& operator()(const _Call_List_& that)
  {
    *this = that;
    return *this;
  }

  //! copy constructor
  _Call_List_& copy(const _Call_List_& that)
  {
    *this = that;
    return *this;
  }

  //! escape _Call_List_ to String
  //
  //! create something like CFGNode -> CFGNode < ... > CFGNode -> CFGNode
  //!   or in case of u_m = -1   CFGNode -> CFGNode -> CFGNode -> CFGNode
  string toStr() const
  {
    ostringstream out;
    out << "\n\n\nTOOD:   TOOD:    !!!!\n\n\n";
    return out.str();
  }
};


//=======================================================================================
//=======================================================================================
//=======================================================================================
//!
//!
class CallLattice : public FiniteLattice
{
protected:
  //! ..
  //
  //!
  std::map<PSet, _Call_List_> pset_lists_;

//=======================================================================================
public:
  /**
   *
   */
  CallLattice();

  /**
   *
   */
  CallLattice( int v );

  /**
   *
   */
  CallLattice(const CallLattice & that);

  /**
   *
   */
  ~CallLattice() {};

  /**
   *
   */
  void initPSets(const std::vector<PSet> ps);

//=======================================================================================
public:
  /**
   * @brief Checks if this call list is a subset of that call list.
   * @param this_list
   * @param that_list
   * @return Returns true if this_list is in that_list.
   *
   */
  bool isSubsetOf(const _Call_List_& this_list, const _Call_List_& that_list) const;

  /**
   * @brief Checks if this and that _Call_List_.calls_ is equal.
   * @param this_list
   * @param that_list
   * @return Returns true if equal.
   *
   */
  bool haveEqualCalls(const _Call_List_& this_list, const _Call_List_& that_list) const;

  /**
   * @brief Find the number of MPI function calls in this and that that are the same.
   * @param this_list
   * @param that_list
   * @return Returns number of common start sequence.
   *
   */
  int commonStartLength(const _Call_List_& this_list,
                        const _Call_List_& that_list) const;

  /**
   * @brief Returns the first num elements from list.
   * @param list
   * @param num
   * @return Returns a vector with num number of elements.
   *
   */
  std::vector<std::pair< CFGNode, _Loop_Count_> >
    getElementsFromBeginning(const _Call_List_& list, int num) const;

  /**
   * @brief adds the CFGNode to every PSSet which is covered by @ps.
   * @param pset
   * @param cfg
   * @param lc
   * @return False if CFGNode was already contained by all relevant process set lists.
   */
  bool add(const std::vector<PSet> psets, const CFGNode cfg, const _Loop_Count_ lcs);
  bool add(PSet ps, const CFGNode cfg, const _Loop_Count_ lcs);

//=======================================================================================
  //!
  _Call_List_ getCallList(const PSet& ps) { return pset_lists_[ps];};
  //!
  void setCallList(_Call_List_ cl, const PSet& ps) {pset_lists_[ps] = cl;};

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

#endif /* MPICALLLATTICE_H_ */
