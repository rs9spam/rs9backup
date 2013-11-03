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
}

//=======================================================================================
LoopLattice::LoopLattice( int v )
{
  this->loop_count_list_.clear();
}

//=======================================================================================
LoopLattice::LoopLattice (const LoopLattice& that)
{
  this->loop_count_list_.assign(that.loop_count_list_.begin(),
                                that.loop_count_list_.end());
}

//=======================================================================================
void LoopLattice::pushBack(const CFGNode* cfgn, const loopCountStruct lcs)
{
  std::pair<CFGNode*, loopCountStruct> p(cfgn,lcs);
  loop_count_list_.push_back(p);
}

//=======================================================================================
void LoopLattice::popBack()
{
  loop_count_list_.pop_back();
}

//=======================================================================================
std::pair<CFGNode*, loopCountStruct> LoopLattice::getCountBack() const
{
  return loop_count_list_.back();
}

//=======================================================================================
loopCountStruct LoopLattice::getCountProduct() const
{
  loopCountStruct lcs = loopCountStruct();

  std::list<std::pair<CFGNode*, loopCountStruct> >::iterator it;
  for(it = this->loop_count_list_.begin(); it != this->loop_count_list_.end(); ++it)
    lcs = lcs * it->second;

  return lcs;
}

// **********************************************
// Required definition of pure virtual functions.
// **********************************************
// computes the meet of this lattice and that lattice and saves the result in this lattice
// returns true if this causes this lattice to change and false otherwise
//=======================================================================================
bool LoopLattice::meetUpdate(Lattice* X)
{
  if(loopAnalysisDebugLevel >= 1)
    std::cerr << "\n MEEET UPDATE";
  LoopLattice* that = dynamic_cast<LoopLattice*>(X);

  if(*this == that)
    return false;
  this->copy(X);
  return true;
}

//=======================================================================================
void LoopLattice::initialize()
{
  this->loop_count_list_.clear();
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
}

//=======================================================================================
bool LoopLattice::operator==(Lattice* X) /*const*/
{
  LoopLattice* that = dynamic_cast<LoopLattice*>(X);
  return this->loop_count_list_ == that->loop_count_list_;
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
  loopCountStruct lcs = this->getCountProduct();
  outs << "\nLoop Lattice Count = " << lcs.toStr() << " {";
  std::list<std::pair<CFGNode*, loopCountStruct> >::iterator it;
  for(it = this->loop_count_list_.begin(); it != this->loop_count_list_.end(); ++it)
    outs << it->second.toStr();
  outs << "}";
  return outs.str();
}

//=======================================================================================
string LoopLattice::toStringForDebugging() const
{
  ostringstream outs;
  loopCountStruct lcs = this->getCountProduct();
    outs << "\nLoop Lattice Count = " << lcs.toStr() << " {";
    std::list<std::pair<CFGNode*, loopCountStruct> >::iterator it;
    for(it = this->loop_count_list_.begin(); it != this->loop_count_list_.end(); ++it)
    {
      outs << "NODE: " << it->first->toString()
           << "Loop Count" << it->second.toStr();
    }
    outs << "}";
  return outs.str();
}
