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
/**
 * @brief List of calls for a certain
 */
struct _Call_List_
{
  /**
   * Point after which possible branching occurred.
   */
  int unease_middle_;

  /**
   * Calls list with CFGNode and associated _Loop_Count_.
   */
  std::vector<std::pair<CFGNode, _Loop_Count_> > calls_;

  /**
   * @brief create empty call list.
   */
  _Call_List_()
  {
    unease_middle_ = -1;
    calls_.clear();
  }

  /**
   * @brief Default + parameterized constructor.
   */
  _Call_List_(std::vector<std::pair<CFGNode, _Loop_Count_> > calls, int um = -1)
  {
    unease_middle_ = um;
    calls_.assign(calls.begin(), calls.end());
  }

  /**
   * @brief Assignment operator.
   */
  _Call_List_& operator=(const _Call_List_& that)
  {
#if 0
    std::cerr << "\nCallLattice.h: _Call_List_: Assign operator. This: "
              << this << " That: " << &that;
#endif
    this->unease_middle_ = that.unease_middle_;
    this->calls_.assign(that.calls_.begin(), that.calls_.end());
    return *this;
  }

  /**
   * @brief Equality comparison doesn't modify object and therefore constant.
   */
  bool operator==(const _Call_List_& that) const
  {
      return (this->unease_middle_ == that.unease_middle_
              && this->calls_ == that.calls_);
  }

  /**
   * @brief Not equal comparison.
   */
  bool operator!=(const _Call_List_& that) const
  {
      return !(*this == that);
  }

  /**
   * @brief Copy constructor.
   */
  _Call_List_& operator()(const _Call_List_& that)
  {
    *this = that;
    return *this;
  }

  /**
   * @brief Copy constructor.
   */
  _Call_List_& copy(const _Call_List_& that)
  {
    *this = that;
    return *this;
  }

  /**
   * @brief Escape _Call_List_ to String.
   *
   * Create something like CFGNode -> CFGNode < ... > CFGNode -> CFGNode
   * or in case of u_m = -1   CFGNode -> CFGNode -> CFGNode -> CFGNode
   */
  string toStr() const
  {
    //TODO:
    ostringstream out;
    std::vector<std::pair<CFGNode, _Loop_Count_> >::const_iterator it;
    int count = 1;
    for(it = calls_.begin(); it != calls_.end(); ++it)
    {
      int line = 0;
      if(isSgLocatedNode(it->first.getNode()))
        line = (isSgLocatedNode(it->first.getNode()))->get_file_info()->get_line();
      out << "\n  (" << count << ") "
          << "## MPI_CALL(...)"
          << " ## line: " << line
          << " ## loop count: " << it->second.toStr();
      count++;
    }
    return out.str();
  }
};


//=======================================================================================
//=======================================================================================
//=======================================================================================
/**
 * @brief
 *
 */
class CallLattice : public FiniteLattice
{
protected:

  /**
   * @brief
   * @param PSet
   * @param _Call_List_
   */
  std::map<PSet, _Call_List_> pset_lists_;

//  int i;
//  int j;
//  CallLattice* pointerthingi;
  /**
   *
   */
  std::map<Lattice*, std::map<PSet,_Call_List_ > > predecessors_list_;
//  int custom_map;
//  std::map<PSet, _Call_List_> pset_lists1_;

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
  ~CallLattice()
  {
#if 0
    std::cerr << "\nCallLattice: DESTRUCTOR: This: "<< this;
#endif
  };

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
   */
  bool isSubsetOf(const _Call_List_& this_list, const _Call_List_& that_list) const;

  /**
   * @brief Checks if this and that _Call_List_.calls_ is equal.
   * @param this_list
   * @param that_list
   * @return Returns true if equal.
   */
  bool haveEqualCalls(const _Call_List_& this_list, const _Call_List_& that_list) const;

  /**
   * @brief Find the number of MPI function calls in this and that that are the same.
   * @param this_list
   * @param that_list
   * @return Returns number of common start sequence.
   */
  int commonStartLength(const _Call_List_& this_list,
                        const _Call_List_& that_list) const;

  /**
   * @brief Returns the first num elements from list.
   * @param list
   * @param num
   * @return Returns a vector with num number of elements.
   */
  std::vector<std::pair< CFGNode, _Loop_Count_> >
    getElementsFromBeginning(const _Call_List_& list, int num) const;

  /**
   * @brief adds the CFGNode to every PSet which is covered by @ps.
   * @param pset
   * @param cfg
   * @param lc
   * @return False if CFGNode was already contained by all relevant process set lists.
   */
  bool add(const std::vector<PSet> psets, const CFGNode cfg, const _Loop_Count_ lcs);

  /**
   * @brief
   */
  bool add(PSet ps, const CFGNode cfg, const _Loop_Count_ lcs);

//=======================================================================================
  /**
   * @brief
   */
  _Call_List_ getCallList(const PSet& ps) { return pset_lists_[ps];};
  /**
   * @brief
   */
  void setCallList(_Call_List_ cl, const PSet& ps) {pset_lists_[ps] = cl;};

protected:
  /**
   * @brief compares two maps with process sets.
   */
  bool comparePSetLists(const std::map<PSet, _Call_List_>& m1,
                        const std::map<PSet, _Call_List_>& m2) const;

  /**
   * @brief Merges l2 with l1 and stores result in l1.
   * @param l1
   * @param l2
   */
  void mergeCallLists(_Call_List_& l1, const _Call_List_& l2) const;

  /**
   * @brief Iterates over all PSets in pset_lists_ and resets the _Call_List_
   */
  void emptyPSetsCallLists();

//=======================================================================================
public:
  // **********************************************
  // Required definition of pure virtual functions.
  // **********************************************

  /**
   * @brief Computes the meet of this and that and saves the result in this.
   * @return True if X causes this to change and false otherwise.
   */
  bool meetUpdate(Lattice* X);

  /**
   * @brief
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
   * @brief
   */
  std::string str(std::string indent="");

//=======================================================================================
public:
  /**
   * @brief
   */
  std::string toString() const;

  /**
   * @brief
   */
  string toStringForDebugging() const;

protected:
  /**
   * @brief
   */
  string printPsets(const std::map<PSet, _Call_List_>& m) const;
};

#endif /* MPICALLLATTICE_H_ */
