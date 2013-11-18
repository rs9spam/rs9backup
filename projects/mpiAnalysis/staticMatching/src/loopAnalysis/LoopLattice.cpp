/*
 * LoopLattice.cpp
 *
 *  Created on: Nov 01, 2013
 *      Author: stoero
 */

#include "loopAnalysis/LoopLattice.h"

extern int loopAnalysisDebugLevel;

// **********************************************************************
//                      LoopLattice
// **********************************************************************
//=======================================================================================
LoopLattice::LoopLattice()
{
  this->loop_count_list_.clear();
  this->node_ = NULL;
  this->false_successor_ = NULL;
  this->is_handled_loop_ = false;
  this->lcs_ = _Loop_Count_();
}

//=======================================================================================
LoopLattice::LoopLattice( int v )
{
  this->loop_count_list_.clear();
  this->node_ = NULL;
  this->false_successor_ = NULL;
  this->is_handled_loop_ = false;
  this->lcs_ = _Loop_Count_();
}

//=======================================================================================
LoopLattice::LoopLattice (const LoopLattice& that)
{
  this->loop_count_list_.assign(that.loop_count_list_.begin(),
                                that.loop_count_list_.end());
  this->node_ = that.node_;
  this->false_successor_ = that.false_successor_;
  this->is_handled_loop_ = that.is_handled_loop_;
  this->lcs_ = that.lcs_;
}

//=======================================================================================
bool LoopLattice::addLoopCount(SgNode* node, _Loop_Count_ lcs)
{
  std::list<std::pair<SgNode*, _Loop_Count_> >::iterator it;
  for(it = this->loop_count_list_.begin(); it != this->loop_count_list_.end(); ++it)
    if(it->first == node)
      return false;
  pushBack(node, lcs);
  return true;
}

//=======================================================================================
void LoopLattice::pushBack(SgNode* node, _Loop_Count_ lcs)
{
  std::pair<SgNode*, _Loop_Count_> p(node, lcs);
  loop_count_list_.push_back(p);
}

//=======================================================================================
void LoopLattice::popBack()
{
  loop_count_list_.pop_back();
}

//=======================================================================================
std::pair<SgNode*, _Loop_Count_> LoopLattice::getCountBack() const
{
  return loop_count_list_.back();
}

//=======================================================================================
_Loop_Count_ LoopLattice::getCountProduct() const
{
  _Loop_Count_ lcs = _Loop_Count_();

  std::list<std::pair<SgNode*, _Loop_Count_> >::const_iterator it;
  for(it = this->loop_count_list_.begin(); it != this->loop_count_list_.end(); ++it)
    lcs = lcs * it->second;

  return lcs;
}

//=======================================================================================
bool LoopLattice::hasEqualList(
                       const std::list<std::pair<SgNode*, _Loop_Count_> > l) const
{
  std::list<std::pair<SgNode*, _Loop_Count_> >::const_iterator this_it;
  std::list<std::pair<SgNode*, _Loop_Count_> >::const_iterator that_it;
  bool found;
  for(that_it = l.begin(); that_it != l.end(); ++ that_it)
  {
    found = false;
    for(this_it = this->loop_count_list_.begin();
        this_it != this->loop_count_list_.end();
        ++this_it)
    {
      if(this_it->first == that_it->first)
      {
        found = true;
        break;
      }
    }
    if(!found)
      return false;
  }
  return true;
}

bool LoopLattice::hasEqualList(const LoopLattice* that) const
{
  return hasEqualList(that->getLoopCountList());
}

//=======================================================================================
void LoopLattice::copyList(const LoopLattice* that)
{
  this->loop_count_list_ = that->getLoopCountList();
}

//=======================================================================================
void LoopLattice::copyList(const std::list<std::pair<SgNode*, _Loop_Count_> > l)
{
  this->loop_count_list_ = l;
}

//=======================================================================================
std::list<std::pair<SgNode*, _Loop_Count_> > LoopLattice::getLoopCountList() const
{
  return this->loop_count_list_;
}

//=======================================================================================
std::list<std::pair<SgNode*, _Loop_Count_> > LoopLattice::getFalseList() const
{
  if(this->node_ == NULL)
    return this->loop_count_list_;

  std::list<std::pair<SgNode*, _Loop_Count_> >::const_iterator it;
  std::list<std::pair<SgNode*, _Loop_Count_> > r_list;
  r_list.clear();
  for(it = loop_count_list_.begin(); it != loop_count_list_.end(); ++it)
  {
    if(it->first != this->node_)
      r_list.push_back(*it);
  }
  return r_list;
}

// **********************************************
// Required definition of pure virtual functions.
// **********************************************
//=======================================================================================
bool LoopLattice::meetUpdate(Lattice* X)
{
  if(loopAnalysisDebugLevel >= 1)
    std::cerr << "\n MEEET UPDATE LOOP LATTICE";
  LoopLattice* that = dynamic_cast<LoopLattice*>(X);

  if(loopAnalysisDebugLevel >= 3)
  {
    std::cerr << "\n   This LOOP: " << this->toStringForDebugging();
    std::cerr << "\n   That LOOP: " << that->toStringForDebugging();
  }

  if(that->isHandledLoop())
  {
    if(loopAnalysisDebugLevel >= 3)
      std::cerr << "\n    That is handled loop.";
    if(that->getFalseSuccessor() == this->getNode()) //MpiUtils functionalities
    {
      if(this->hasEqualList(that->getFalseList()))
      {
        if(loopAnalysisDebugLevel >= 3)
          std::cerr << "\n   Meet update Returns FALSE!";
        return false;
      }
      this->copyList(that->getFalseList());
      if(loopAnalysisDebugLevel >= 3)
        std::cerr << "\n This UPDATED: " << this->toStringForDebugging();
      return true;
    }
  }

  if(this->hasEqualList(that))
  {
    if(loopAnalysisDebugLevel >= 3)
      std::cerr << "\n Equal List, Return = False.";
    return false;
  }
  this->copyList(that);
  if(loopAnalysisDebugLevel >= 3)
    std::cerr << "\n This UPDATED: " << this->toStringForDebugging();
  return true;
}

//=======================================================================================
void LoopLattice::initialize()
{
  this->loop_count_list_.clear();
  this->node_ = NULL;
  this->false_successor_ = NULL;
  this->is_handled_loop_ = false;
  this->lcs_ = _Loop_Count_();
}

// returns a copy of this lattice
//=======================================================================================
Lattice* LoopLattice::copy() const
{
  return new LoopLattice(*this);
}

// overwrites the state of "this" Lattice with "that" Lattice
//=======================================================================================
void LoopLattice::copy(/*const*/ Lattice* X)
{
  LoopLattice* that = dynamic_cast<LoopLattice*>(X);
  this->loop_count_list_.assign(that->loop_count_list_.begin(),
                                that->loop_count_list_.end());
  this->node_ = that->node_;
  this->false_successor_ = that->false_successor_;
  this->is_handled_loop_ = that->is_handled_loop_;
  this->lcs_ = that->lcs_;
}

//=======================================================================================
bool LoopLattice::operator==(Lattice* X) /*const*/  //TODO
{
  LoopLattice* that = dynamic_cast<LoopLattice*>(X);
  return this->loop_count_list_ == that->loop_count_list_
         && this->node_ == that->node_
         && this->false_successor_ == that->false_successor_
         && this->is_handled_loop_ == that -> is_handled_loop_
         && this->lcs_ == that->lcs_;
}

//=======================================================================================
string LoopLattice::str(string indent)
{
  ostringstream outs;
  outs << indent << toString();
  return outs.str();
}

//=======================================================================================
string LoopLattice::toString() const
{
  ostringstream outs;
  _Loop_Count_ lcs = this->getCountProduct();
  outs << "Loop Lattice Count = " << lcs.toStr() << " {";
  std::list<std::pair<SgNode*, _Loop_Count_> >::const_iterator it;
  for(it = this->loop_count_list_.begin(); it != this->loop_count_list_.end(); ++it)
    outs << it->second.toStr();
  outs << "}";
  return outs.str();
}

//=======================================================================================
string LoopLattice::toStringForDebugging() const  //TODO some node info
{
  ostringstream outs;
  _Loop_Count_ lcs = this->getCountProduct();
    outs << "Loop Lattice Count = " << lcs.toStr() << " {";
    std::list<std::pair<SgNode*, _Loop_Count_> >::const_iterator it;
    for(it = this->loop_count_list_.begin(); it != this->loop_count_list_.end(); ++it)
    {
      outs << "[<" << it->first->class_name() << ">"
           << " @line=" << it->first->get_startOfConstruct()->get_line()
           << " Loop Count" << it->second.toStr() << "]";
    }
    outs << "}";
  return outs.str();
}
