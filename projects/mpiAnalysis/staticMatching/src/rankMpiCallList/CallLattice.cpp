/*
 * CallLattice.cpp
 *
 *  Created on: Nov 10, 2013
 *      Author: stoero
 */


#include "rankMpiCallList/CallLattice.h"

//extern int mpiCallAnalysisDebugLevel;

// **********************************************************************
//                      CallLattice
// **********************************************************************
//=======================================================================================
CallLattice::CallLattice()
{
  this->pset_lists_.clear();
}

//=======================================================================================
CallLattice::CallLattice( int v )
{
  this->pset_lists_.clear();
}

//=======================================================================================
CallLattice::CallLattice (const CallLattice& that)
{
  this->pset_lists_.clear();
  this->pset_lists_.insert(that.pset_lists_.begin(),
                           that.pset_lists_.end());
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
    if(this_it != that_it)
      return false;
  }
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
  return isSubsetOf(this_list, that_list);
}

//=======================================================================================
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
    if(this_it != that_it)
      return count;
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

// **********************************************
// Required definition of pure virtual functions.
// **********************************************
// computes the meet of this lattice and that lattice and saves the result in this lattice
// returns true if this causes this lattice to change and false otherwise
//=======================================================================================
bool CallLattice::meetUpdate(Lattice* X)  //TODO: check this function
{
  bool modified = false;

  CallLattice* that = dynamic_cast<CallLattice*>(X);
  std::map<PSet, _Call_List_>::iterator it;

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

  return modified;
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
  this->pset_lists_.insert(that->pset_lists_.begin(), that->pset_lists_.end());
}

//=======================================================================================
bool CallLattice::operator==(Lattice* X) /*const*/  //TODO
{
  CallLattice* that = dynamic_cast<CallLattice*>(X);
  return this->pset_lists_ == that->pset_lists_;
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
  //TODO:
//  _Loop_Count_ lcs = this->getCountProduct();
//  outs << "\nLoop Lattice Count = " << lcs.toStr() << " {";
//  std::list<std::pair<SgNode*, _Loop_Count_> >::const_iterator it;
//  for(it = this->loop_count_list_.begin(); it != this->loop_count_list_.end(); ++it)
//    outs << it->second.toStr();
//  outs << "}";
  return outs.str();
}

//=======================================================================================
string CallLattice::toStringForDebugging() const  //TODO some node info
{
  ostringstream outs;
  //TODO:
//  _Loop_Count_ lcs = this->getCountProduct();
//    outs << "\nLoop Lattice Count = " << lcs.toStr() << " {";
//    std::list<std::pair<SgNode*, _Loop_Count_> >::const_iterator it;
//    for(it = this->loop_count_list_.begin(); it != this->loop_count_list_.end(); ++it)
//    {
//      outs << "<" << it->first->class_name() << ">"
//           << " @line=" << it->first->get_startOfConstruct()->get_line() << "\n"
//           << "Loop Count" << it->second.toStr();
//    }
//    outs << "}";
  return outs.str();
}
