/*
 * CallLattice.cpp
 *
 *  Created on: Nov 10, 2013
 *      Author: stoero
 */


#include "rankMpiCallList/CallLattice.h"

extern int mpiCallAnalysisDebugLevel;

// **********************************************************************
//                      CallLattice
// **********************************************************************
//=======================================================================================
CallLattice::CallLattice()
{
//  std::cerr << "\nCallLattice: Empty constructor. THIS: " << this;
//  this->pset_lists_.clear();
//  this->pset_lists1_.clear();
//  this->predecessors_list_.clear();
}

//=======================================================================================
CallLattice::CallLattice( int v )
{
//  std::cerr << "\nCallLattice: Constructor with Argument. THIS: " << this;
//  this->pset_lists_.clear();
//  this->pset_lists1_.clear();
//  this->predecessors_list_.clear();
}

//=======================================================================================
CallLattice::CallLattice (const CallLattice& that)
{
//  std::cerr << "\nCallLattice: Copy Constructor. THIS: " << this << " That: " << &that;

  this->pset_lists_.clear();
  this->pset_lists_.insert(that.pset_lists_.begin(),
                           that.pset_lists_.end());

//  this->pset_lists1_.clear();
//  this->predecessors_list_.clear();

  //TODO: do I need the predecessor lists? //////////////////////////////////////////////////////////////////////////////////////////
}

//=======================================================================================
void CallLattice::initPSets(const std::vector<PSet> ps)
{
  std::vector<PSet>::const_iterator it;
  for(it = ps.begin(); it != ps.end(); ++it)
    pset_lists_[*it] = _Call_List_();
}

//=======================================================================================
bool CallLattice::isSubsetOf(const _Call_List_& this_list,
                             const _Call_List_& that_list) const
{
  std::vector< std::pair<CFGNode, _Loop_Count_> >::const_iterator this_it;
  std::vector< std::pair<CFGNode, _Loop_Count_> >::const_iterator that_it;
  this_it = this_list.calls_.begin();
  that_it = that_list.calls_.begin();
  while(this_it != this_list.calls_.end() && that_it != that_list.calls_.end())
  {
    if(this_it->first != that_it->first || this_it->second != that_it->second)
      return false;
    ++this_it;
    ++that_it;
  }

  // if this list is not at the end and that list is,
  // then that_list is a subset of this_list
  if(this_it != this_list.calls_.end() && that_it == that_list.calls_.end())
    return false;
  return true;
}

//=======================================================================================
bool CallLattice::haveEqualCalls(const _Call_List_& this_list,
                                 const _Call_List_& that_list) const
{
  if(this_list.calls_.size() != that_list.calls_.size())
    return false;
  if(!isSubsetOf(this_list, that_list))
    return false;
  return this_list.unease_middle_ == that_list.unease_middle_;
}

//======================================================================================= TODO: DEBUG OUTPUT (remove)
int CallLattice::commonStartLength(const _Call_List_& this_list,
                                   const _Call_List_& that_list) const
{
  int count = 0;
  std::vector< std::pair<CFGNode, _Loop_Count_> >::const_iterator this_it;
  std::vector< std::pair<CFGNode, _Loop_Count_> >::const_iterator that_it;
  this_it = this_list.calls_.begin();
  that_it = that_list.calls_.begin();
  while(this_it != this_list.calls_.end() && that_it != that_list.calls_.end())
  {
    if(this_it->first != that_it->first || this_it->second != that_it->second)
    {
      std::cerr << "\nCallLattice::commonStartLenght: "
                 << "Two Elements in list are not equal at "
                 << count + 1;
      return count;
    }
    ++this_it;
    ++that_it;
    ++count;
  }
  return count;
}

//=======================================================================================
std::vector<std::pair< CFGNode, _Loop_Count_> >
  CallLattice::getElementsFromBeginning(const _Call_List_& list, int num) const
{

  std::vector<std::pair< CFGNode, _Loop_Count_> > ret;
  if(num > 0)
    ret.assign(list.calls_.begin(), list.calls_.begin()+(num-1));
  else
    ret.clear();
  return ret;
}

//=======================================================================================
bool CallLattice::add(const std::vector<PSet> psets,
                      const CFGNode cfg,
                      const _Loop_Count_ lcs)
{
  bool modified = false;
  for(std::vector<PSet>::const_iterator it = psets.begin(); it != psets.end(); ++it)
    modified = add(*it, cfg, lcs) || modified;
  return modified;
}

//=======================================================================================
bool CallLattice::add(const PSet pset,
                      const CFGNode cfg,
                      const _Loop_Count_ lcs)
{
  bool modified = false;
  std::map<PSet, _Call_List_>::iterator map_it;
  for(map_it = pset_lists_.begin(); map_it != pset_lists_.end(); ++map_it)
  {
    if(pset.contains(map_it->first))
    {
      bool found_cfg_node = false;
      std::vector<std::pair<CFGNode, _Loop_Count_> >::const_iterator calls_it;
      for(calls_it = map_it->second.calls_.begin();
          calls_it != map_it->second.calls_.end();
          ++calls_it)
      {
        if(calls_it->first == cfg)
          found_cfg_node = true;
      }
      if(!found_cfg_node)
      {
        map_it->second.calls_.push_back(std::make_pair(cfg, lcs));
        modified = true;
      }
    }
  }
  return modified;
}

//=======================================================================================
bool CallLattice::comparePSetLists(const std::map<PSet, _Call_List_>& m1,
                                   const std::map<PSet, _Call_List_>& m2) const
{
  std::map<PSet, _Call_List_>::const_iterator it1 = m1.begin();
  std::map<PSet, _Call_List_>::const_iterator it2 = m2.begin();
  while(it1 != m1.end() && it2 != m2.end())
  {
    if(it1->first != it2->first || it1->second != it2->second)
      return false;
    ++it1;
    ++it2;
  }
  if(it1 != m1.end() || it2 != m2.end())
    return false;

  return true;
}

//=======================================================================================
void CallLattice::mergeCallLists(_Call_List_& l1, const _Call_List_& l2) const
{
  if(!haveEqualCalls(l1, l2))
  {
    if(l1.calls_.empty() && l1.unease_middle_ == -1)//most likely case.
    {
//      if(!l2.calls_.empty() || l2.unease_middle_ != -1)
        l1 = l2;
    }
    else
    {
      //TODO: maybe also check for loops here, since it is not so clear if this is
      //      the case, if the last element of that is not a loop....
      if(isSubsetOf(l1, l2))
      {
        l1 = _Call_List_(l2.calls_, std::max(l1.unease_middle_, l2.unease_middle_));
      }
      //TODO, should actually cut the whole thing here.
      else if(isSubsetOf(l2, l1))
      {
        if(l1.unease_middle_ != l2.unease_middle_)
          l1.unease_middle_ = l2.unease_middle_;
      }
      else
      {
        int min = commonStartLength(l1, l2);
        if(l1.unease_middle_ != -1)
          min = std::min(min, l1.unease_middle_);
        if(l2.unease_middle_ != -1)
          min = std::min(min, l2.unease_middle_);
        l1 = _Call_List_(getElementsFromBeginning(l1, min), min);
      }
    }
  }
  else
  {
    //TODO:
    //Is it important to check for the unease_middle_?
  }
}

//=======================================================================================
void CallLattice::emptyPSetsCallLists()
{
  std::map<PSet, _Call_List_>::iterator it;
  for(it = pset_lists_.begin(); it != pset_lists_.end(); ++it)
    it->second = _Call_List_();
}

// **********************************************
// Required definition of pure virtual functions.
// **********************************************
// computes the meet of this lattice and that lattice and saves the result in this lattice
// returns true if this causes this lattice to change and false otherwise
//=======================================================================================
bool CallLattice::meetUpdate(Lattice* X)  //TODO: check this function
{
  CallLattice* that = dynamic_cast<CallLattice*>(X);

  if(!comparePSetLists(that->pset_lists_, predecessors_list_[X]))
  {
    predecessors_list_[X] = that->pset_lists_;

    std::map<PSet, _Call_List_> temp_pset_lists;
    temp_pset_lists.insert(pset_lists_.begin(), pset_lists_.end());
    emptyPSetsCallLists();

    std::map<Lattice*, std::map<PSet, _Call_List_> >::const_iterator l_it;
    for(l_it = predecessors_list_.begin(); l_it != predecessors_list_.end(); ++l_it)
    {
      std::map<PSet, _Call_List_>::iterator it;
      for(it = this->pset_lists_.begin(); it != this->pset_lists_.end(); ++it)
        mergeCallLists(it->second, (l_it->second).at(it->first));
    }
    if(comparePSetLists(temp_pset_lists, pset_lists_))
      return false;
    else
      return true;
  }

  return false;

#if 0
  if(mpiCallAnalysisDebugLevel >= 1)
    std::cerr << "\n\n\n\n MEEET UPDATE Call Lattice:";

  CallLattice* that = dynamic_cast<CallLattice*>(X);

//  if(mpiCallAnalysisDebugLevel >= 3)
//    std::cerr << "\nOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO"
//              << "\nTHIS:\n" << this->toStringForDebugging()
//              << "\nTHAT:\n" << that->toStringForDebugging()
//              << "\nPred: " << printPsets(predecessors_list_[X]);

//  std::map<PSet, _Call_List_>::iterator temp_it;
//  bool that_is_not_empty = false;
//  for(temp_it = that->pset_lists_.begin(); temp_it != that->pset_lists_.end(); ++temp_it)
//  {
//    if(!(temp_it->second.calls_.empty()))
//      that_is_not_empty = true;
//  }

  if(!comparePSetLists(that->pset_lists_, predecessors_list_[X]))
  {
    if(mpiCallAnalysisDebugLevel >= 3)
      std::cerr <<  "   THIS != THAT";

//    if(mpiCallAnalysisDebugLevel >= 3)
//      std::cerr << "\nOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO"
//                << "\nTHIS:\n" << this->toStringForDebugging()
//                << "\nTHAT:\n" << that->toStringForDebugging();

    //add that->pset_list_ to pred_list_[X]
    predecessors_list_[X] = that->pset_lists_;

    if(mpiCallAnalysisDebugLevel >= 3)
      std::cerr << "\n new Pred: " << printPsets(predecessors_list_[X]);

    //create temp copy of existing pset_
    std::map<PSet, _Call_List_> temp_pset_lists;
    temp_pset_lists.insert(pset_lists_.begin(), pset_lists_.end());

//    pset_lists_.clear(); //////THAT WAS A BIG PROBLEM....
    emptyPSetsCallLists();

    //merge the whole predecessor list to a cleared pset_lists_
    std::map<Lattice*, std::map<PSet, _Call_List_> >::const_iterator l_it;

//    if(that_is_not_empty)
//      std::cerr << "\n8888  PREDECESSORS_LIST SIZE: " << predecessors_list_.size();

    for(l_it = predecessors_list_.begin(); l_it != predecessors_list_.end(); ++l_it)
    {
      std::map<PSet, _Call_List_>::iterator it;

//      if(that_is_not_empty)
//        std::cerr << "\n99999 Process set List from loop: " << printPsets(l_it->second);

      for(it = this->pset_lists_.begin(); it != this->pset_lists_.end(); ++it)
        mergeCallLists(it->second, (l_it->second).at(it->first));
    }
    //check with the temp copy of the pset_lists_ if changes happened
    if(comparePSetLists(temp_pset_lists, pset_lists_))
      return false;
    else
//    {
//      std::cerr << "                       RETURN True!";
//      std::cerr << "\n new THIS:\n" << this->toStringForDebugging();
      return true;
//    }
  }

//  std::cerr << "                       RETURN FALSE Default!";
  return false;
#endif

#if 0
  if(mpiCallAnalysisDebugLevel >= 1)
    std::cerr << "\n MEEET UPDATE Call Lattice:";
  bool modified = false;

  CallLattice* that = dynamic_cast<CallLattice*>(X);
  std::map<PSet, _Call_List_>::iterator it;

  if(mpiCallAnalysisDebugLevel >= 3)
    std::cerr << "\nOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO"
              << "\nTHIS:\n" << this->toStringForDebugging()
              << "\nTHAT:\n" << that->toStringForDebugging();

  for(it = this->pset_lists_.begin(); it != this->pset_lists_.end(); ++it)
  {
    _Call_List_ that_cl = that->pset_lists_[it->first];
    if(!haveEqualCalls(it->second, that_cl))
    {
      if(it->second.calls_.empty() && it->second.unease_middle_ == -1)//most likely case.
      {
        if(!that_cl.calls_.empty() || that_cl.unease_middle_ != -1)
        {
          it->second = that_cl;
          modified = true;
        }
      }
      else
      {
        //TODO: maybe also check for loops here, since it is not so clear if this is
        //      the case, if the last element of that is not a loop....
        if(isSubsetOf(it->second, that_cl))
        {
          int min = std::max(it->second.unease_middle_, that_cl.unease_middle_);
          it->second = _Call_List_(that_cl.calls_, min);
          modified = true;
        }
        //TODO, should actually cut the whole thing here.
        else if(isSubsetOf(that_cl, it->second))
        {
          if(it->second.unease_middle_ != that_cl.unease_middle_)
          {
            it->second.unease_middle_ = that_cl.unease_middle_;
            modified = true;
          }
        }
        else
        {
          int min = commonStartLength(it->second, that_cl);
          if(it->second.unease_middle_ != -1)
            min = std::min(min, it->second.unease_middle_);
          if(that_cl.unease_middle_ != -1)
            min = std::min(min, that_cl.unease_middle_);
          it->second = _Call_List_(getElementsFromBeginning(it->second, min), min);
          modified = true;
        }
      }
    }
    else
    { //TODO:
      //Is it important to check for the unease_middle_?
    }
  }

  if(mpiCallAnalysisDebugLevel >= 3)
    std::cerr << "\nNEW THIS: modified = " << (modified? "true" : "false")
              << this->toStringForDebugging();

  return modified;
#endif
}

//=======================================================================================
void CallLattice::initialize()
{
  this->pset_lists_.clear();
}

// returns a copy of this lattice
//=======================================================================================
Lattice* CallLattice::copy() const
{
  return new CallLattice(*this);
}

// overwrites the state of "this" Lattice with "that" Lattice
//=======================================================================================
void CallLattice::copy(/*const*/ Lattice* X)
{
  CallLattice* that = dynamic_cast<CallLattice*>(X);
  std::map<PSet,_Call_List_>::const_iterator it;
  for(it = that->pset_lists_.begin(); it != that->pset_lists_.end(); ++it)
    this->pset_lists_[it->first] = it->second;
}

//=======================================================================================
bool CallLattice::operator==(Lattice* X) /*const*/  //TODO
{
  CallLattice* that = dynamic_cast<CallLattice*>(X);
  std::map<PSet,_Call_List_>::const_iterator this_it = this->pset_lists_.begin();
  std::map<PSet,_Call_List_>::const_iterator that_it = that->pset_lists_.begin();
  while(this_it != this->pset_lists_.end() && that_it != that->pset_lists_.end())
  {
    if(this_it->first != that_it->first || this_it->second != that_it->second)
      return false;
    else
    {
      ++this_it;
      ++that_it;
    }
  }
  if(this_it == this->pset_lists_.end() && that_it == that->pset_lists_.end())
    return true;
  return false;
//  return this->pset_lists_ == that->pset_lists_;
}

//=======================================================================================
string CallLattice::str(string indent)
{
  ostringstream outs;
  outs << indent << toString();
  return outs.str();
}

//=======================================================================================
string CallLattice::toString() const
{
  ostringstream outs;
  outs << toStringForDebugging();
  return outs.str();
}

//=======================================================================================
string CallLattice::toStringForDebugging() const
{
  ostringstream outs;
  outs << "\n######################################################################"
       << "\n###############      MPI CALL LIST LATTICE     #######################"
       << "\n######################################################################";
  outs << printPsets(pset_lists_);
//  std::map<PSet,_Call_List_>::const_iterator it;
//  for(it = pset_lists_.begin(); it != pset_lists_.end(); ++it)
//  {
//    outs << "\n======================================================================"
//         << "\nPSET: " << it->first.toString()
//         << it->second.toStr();
//  }
//  outs << "\n======================================================================";
  outs << "\n######################################################################";
  return outs.str();
}

//=======================================================================================
string CallLattice::printPsets(const std::map<PSet, _Call_List_>& m) const
{
  ostringstream outs;
  std::map<PSet,_Call_List_>::const_iterator it;
  for(it = m.begin(); it != m.end(); ++it)
  {
    outs << "\n======================================================================"
         << "\nPSET: " << it->first.toString()
         << it->second.toStr();
  }
  outs << "\n======================================================================";
  return outs.str();
}
