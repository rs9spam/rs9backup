#include "rankAnalysis.h"



















////#include <boost/bind.hpp>
////#include <boost/mem_fn.hpp>
//
//int rankAnalysisDebugLevel = 2;
//
//
//// **********************************************************************
////                      RankLattice
//// **********************************************************************
//
//////=======================================================================================
////struct compoffset {
////  bool operator() (const PSet* lhs, const PSet* rhs) const
////  {return lhs<rhs;}
////};
////
//////=======================================================================================
////struct compldiv {
////  bool operator() (const int& lhs, const int& rhs) const
////  {return lhs<rhs;}
////};
//
////=======================================================================================
//RankLattice::RankLattice()
//{
//  this->scope_depth_ = 0;
//  this->check_pred_ = false;
//
////  cont_psets range;
////  range[0] = PSet();
////  psets_[0] = range;
//
//  psets_[0][0] = PSet();
//  this->size_call_ = NULL;
//  this->rank_call_ = NULL;
//
////  cont_psets range;
////  range.insert(*PSet());
////  this->psets.insert(range, 0);
//}
//
////=======================================================================================
//RankLattice::RankLattice( int v )
//{
//  this->scope_depth_ = 0;
//  this->check_pred_ = false;
//
////  cont_psets range;
////  range[0] = PSet();
////  psets_[0] = range;
//  psets_[0][0] = PSet();
//  this->size_call_ = NULL;
//  this->rank_call_ = NULL;
//}
//
//// This is the same as the implicit definition, so it might not be required to be defined explicitly.
//// I am searching for the minimal example of the use of the data flow classes.
////=======================================================================================
//RankLattice::RankLattice (const RankLattice & X)
//{
//  this->scope_depth_ = X.scope_depth_;
//  this->check_pred_ = X.check_pred_;
//  this->psets_ = X.psets_;
//  this->psets_vec_ = X.psets_vec_;
//  this->size_call_ = X.size_call_;
//  this->rank_call_ = X.rank_call_;
//}
//
////=======================================================================================
//bool RankLattice::getScopeStmt() const
//{
//  return check_pred_;
//}
//
////=======================================================================================
//bool RankLattice::setScopeStmt(bool stmt)
//{
//  bool modified = this->check_pred_ != stmt;
//  this->check_pred_ = stmt;
//  return modified;
//}
//
////=======================================================================================
//lattice_psets RankLattice::getPSets()
//{
//  return (this->psets_);
//}
//
////=======================================================================================
//SgNode* RankLattice::getRankCall()
//{
//  return this->rank_call_;
//}
////=======================================================================================
//SgNode* RankLattice::getSizeCall()
//{
//  return this->size_call_;
//}
////=======================================================================================
//bool RankLattice::setRankCall(SgNode* node)
//{
//  if(this->rank_call_ == node)
//    return false;
//  else
//    this->rank_call_ = node;
//  return true;
//}
////=======================================================================================
//bool RankLattice::setSizeCall(SgNode* node)
//{
//  if(this->size_call_ == node)
//    return false;
//  else
//    this->size_call_ = node;
//  return true;
//}
//
//
//
//
//// **********************************************
//// Process Set related functionality
//// **********************************************
//// typedef std::map<int, PSet> cont_psets;
//// std::map<int, cont_psets> psets_;
//// **********************************************
//
//
////=======================================================================================
////return ture if the two sets intersect or touch
//bool RankLattice::doInterleave(PSet p_set1, PSet p_set2)
//{
//  //false if
//  //either p1_hb < p2_lb - 1  or p2_hb < p1_lb
//  // compare if higher bound of p_set1 is smaller than lower bound -1 of p_set2
//  if(p_set1.compareHbSmallerLb(p_set2) ||
//     p_set2.compareHbSmallerLb(p_set1))
//    return false;
//
//  return true;
//}
//
////=======================================================================================
//bool RankLattice::contains(PSet p_set, bool abs, int num, int den, int off)
//{
//  return p_set.contains(abs, num, den, off);
//}
////=======================================================================================
//bool RankLattice::isUnder(PSet p_set, bool abs, int num, int den, int off)
//{
//  return p_set.isUnder(abs, num, den, off);
//}
////=======================================================================================
//bool RankLattice::isOver(PSet p_set, bool abs, int num, int den, int off)
//{
//  return p_set.isOver(abs, num, den, off);
//}
//
////=======================================================================================
////set the p_set on the according position in the psets_ map
//void RankLattice::addHardPSet(PSet p_set)
//{
//  (psets_[p_set.getLden()])[p_set.getLoff()] = p_set;
//}
//
////=======================================================================================
////set the p_set on the according position in the psets_ map
//void RankLattice::addHardPSet(lattice_psets& psets, PSet p_set)
//{
//  (psets[p_set.getLden()])[p_set.getLoff()] = p_set;
//  std::cerr << "ADD Hard a p_set!\n";
//}
//
////=======================================================================================
////combines the range of both p_sets and returns a new p_set
//PSet RankLattice::combinePSets(PSet p_set1, PSet p_set2)
//{
//  int lnum, hnum, loff, hoff, lden, hden;
//  bool empty, absl, absh;
//
//  //if both sets are empty, the remaining set is going to be empty
//  empty = p_set1.isEmpty() && p_set2.isEmpty();
//  //absolute bounds are smaller than relative ones so if one of them is abs, the result is abs
//  absl = p_set1.isLbAbs() || p_set2.isLbAbs();
//  //higher bounds: if one is not abs, the result is not abs
//  absh = p_set1.isHbAbs() && p_set2.isHbAbs();
//
//  if(p_set1 < p_set2)
//  {
//    //get low values from p_set1
//    lnum = p_set1.getLnum();
//    loff = p_set1.getLoff();
//    lden = p_set1.getLden();
//  }
//  else
//  {
//    //get low values from p_set2
//    lnum = p_set2.getLnum();
//    loff = p_set2.getLoff();
//    lden = p_set2.getLden();
//  }
//  if(p_set1 > p_set2)
//  {
//    //get high values from p_set1
//    hnum = p_set1.getHnum();
//    hoff = p_set1.getHoff();
//    hden = p_set1.getHden();
//  }
//  else
//  {
//    //get high values from p_set2
//    hnum = p_set2.getHnum();
//    hoff = p_set2.getHoff();
//    hden = p_set2.getHden();
//  }
//  return PSet(empty, absl, absh, lnum, hnum, lden, hden, loff, hoff);
//}
//
////=======================================================================================
//bool RankLattice::pushbackUniquePSets(RankLattice& X)
//{
//  //  std::cerr << "### RankAnalysis:RankLattice:pushbackPsets (rank_lattice) begin\n";
//    std::vector< std::pair<lattice_psets, std::pair<SgNode*, LatticeSourceType> > >::iterator it;
//    for(it = X.psets_vec_.begin(); it != X.psets_vec_.end(); it++)
//    {
//  //    std::cerr << "### RankAnalysis:RankLattice:pushbackPsets (rank_lattice) middle\n";
//      lattice_psets lp = it->first;
//      RankLattice::LatticeSourceType edgetype = it->second.second;
//      SgNode* n = it->second.first;
//      //TODO bevore inserting check if it already exists...
//      if(true)
//        pushbackPSets(lp, edgetype, n);
//    }
//  //  std::cerr << "### RankAnalysis:RankLattice:pushbackPsets (rank_lattice) end\n";
//    return (X.psets_vec_.size()>0) ? true : false;
//}
//
////=======================================================================================
//bool RankLattice::pushbackPSets(RankLattice& X)
//{
////  std::cerr << "### RankAnalysis:RankLattice:pushbackPsets (rank_lattice) begin\n";
//  std::vector< std::pair<lattice_psets, std::pair<SgNode*, LatticeSourceType> > >::iterator it;
//  for(it = X.psets_vec_.begin(); it != X.psets_vec_.end(); it++)
//  {
////    std::cerr << "### RankAnalysis:RankLattice:pushbackPsets (rank_lattice) middle\n";
//    lattice_psets lp = it->first;
//    RankLattice::LatticeSourceType edgetype = it->second.second;
//    SgNode* n = it->second.first;
//    pushbackPSets(lp, edgetype, n);
//  }
////  std::cerr << "### RankAnalysis:RankLattice:pushbackPsets (rank_lattice) end\n";
//  return (X.psets_vec_.size()>0) ? true : false;
//}
//
////=======================================================================================
//bool RankLattice::pushbackPSets(lattice_psets sets,
//                                RankLattice::LatticeSourceType edgetype,
//                                SgNode* n)
//{
////  std::cerr << "### RankAnalysis:RankLattice:pushbackPsets (sets, type, node) \n";
//  std::pair<SgNode*, LatticeSourceType> mypair(n,edgetype);
//  std::pair<lattice_psets, std::pair<SgNode*, LatticeSourceType> > pair2 (sets, mypair);
//  psets_vec_.push_back(pair2);
//  return true;
//}
//
////=======================================================================================
//lattice_psets RankLattice::getPsets(RankLattice::LatticeSourceType source_type,
//                                     SgNode* n)
//{
//  lattice_psets pset;
//  std::vector< std::pair<lattice_psets, std::pair<SgNode*, LatticeSourceType> > >::iterator it;
//  for(it = this->psets_vec_.begin(); it != this->psets_vec_.end(); it++)
//  {
//    if((it->second.first == n) && (it->second.second == source_type))
//    {
//      pset = (it->first);
//      it = this->psets_vec_.end();
//    }
//  }
//  return pset;
//}
//
////=======================================================================================
//// Adds all process set ranges from lattice to this->psets_
//// returns true if changes happened
//// returns false if added PSets were already in the psets_ (no changes)
//bool RankLattice::mergePSets(RankLattice& lattice)
//{
//  return mergePSets(lattice.getPSets());
//}
//
////=======================================================================================
//bool RankLattice::mergePSets(lattice_psets psets)
//{
//  bool modified = false;
//
//  lattice_psets::iterator s_iter;
//  cont_psets::iterator r_iter;
//
//  for(s_iter = psets.begin(); s_iter != psets.end(); s_iter++)
//    for( r_iter = (s_iter->second).begin();
//         r_iter != (s_iter->second).end();
//         r_iter++)
//      modified = mergePSets(r_iter->second);
//
//  return modified;
//}
//
//lattice_psets RankLattice::combinePSets(lattice_psets sets1, lattice_psets sets2)
//{
//  lattice_psets psets;
//  PSet p_set;
//
//  std::map<int, cont_psets>::iterator s_iter;
//  std::map<int, PSet>::iterator r_iter;
//  for(s_iter = sets1.begin(); s_iter != sets1.end(); s_iter++)
//  {
//    for( r_iter = (s_iter->second).begin();
//         r_iter != (s_iter->second).end();
//         r_iter++)
//    {
//      p_set = r_iter->second;
//      addHardPSet(psets, p_set);
//    }
//  }
//  for(s_iter = sets2.begin(); s_iter != sets2.end(); s_iter++)
//  {
//    for( r_iter = (s_iter->second).begin();
//         r_iter != (s_iter->second).end();
//         r_iter++)
//    {
//      p_set = r_iter->second;
//      addHardPSet(psets, p_set);
//    }
//  }
//
//  return psets;
//}
//
////=======================================================================================
//// Adds one process set range to psets_
//// returns true if changes happened
//// returns false if added PSet was already in the psets_ (no changes)
//bool RankLattice::mergePSets(PSet p_set)
//{
//  //find the lower bound of the p_set and add it at the appropriate cont_psets position
//  //merge one process set to the others.
//  bool modified = false;
//
//  std::map<int, cont_psets>::iterator s_iter;
//  std::map<int, PSet>::iterator r_iter;
//  for(s_iter = psets_.begin(); s_iter != psets_.end(); )//s_iter++)
//  {
//    for( r_iter = (s_iter->second).begin();
//         r_iter != (s_iter->second).end();
//         )// no iteration increment here!!
//    {
//      if((r_iter->second).isEmpty())
//      {
//        //make sure, the empty set is gone
//        (s_iter->second).erase(r_iter++);
//      }
//      else if(doInterleave(r_iter->second, p_set))
//      {
////        std::cerr << "ATTENTION the p_set does interleave with another one...\n";
//        modified = true;
//        //combine both sets in p_set and remove the r_iter set
//        p_set = combinePSets(r_iter->second, p_set);
//
//        (s_iter->second).erase(r_iter++);
//      }
//      else
//      {
//        ++r_iter;
//      }
//    }
//    // if s_iter is empty after the four loop, also remove the s_iter
//    if((s_iter->second).size()==0)
//    {
//      modified = true;
//      psets_.erase(s_iter++);
//    }
//    else
//    {
//      ++s_iter;
//    }
//  }
//  addHardPSet(p_set);
////  std::cerr << "ADDHardPSet: " << p_set.str() << "\n";
//
//  return modified;
//}
//
////=======================================================================================
//lattice_psets RankLattice::splitSetBound(bool lowbound, bool abs, int num, int den, int off)
//{
//  lattice_psets sub_sets;
//  PSet pset;
//  bool empty = true;
//
//  lattice_psets::iterator s_iter;
//  cont_psets::iterator r_iter;
//
//  for(s_iter = psets_.begin(); s_iter != psets_.end(); s_iter++)
//    for( r_iter = (s_iter->second).begin();
//         r_iter != (s_iter->second).end();
//         r_iter++)
//    {
//      if(lowbound)
//      {
//        if(isOver(r_iter->second, abs, num, den, off))
//        {
//          pset = r_iter->second;
//          addHardPSet(sub_sets, pset);
//          empty = false;
//        }
//        if(contains(r_iter->second, abs, num, den, off))
//        {
//          pset = r_iter->second;
//          pset.setLow(abs, num, den, off);
//          addHardPSet(sub_sets, pset);
//          empty = false;
//        }
//      }
//      else
//      {
//        if(isUnder(r_iter->second, abs, num, den, off))
//        {
//          pset = r_iter->second;
//          addHardPSet(sub_sets, pset);
//          empty = false;
//        }
//        if(contains(r_iter->second, abs, num, den, off))
//        {
//          pset = r_iter->second;
//          pset.setHigh(abs, num, den, off);
//          addHardPSet(sub_sets, pset);
//          empty = false;
//        }
//      }
//
//    }
//  if(empty)
//  {
//    pset = PSet();
//    addHardPSet(sub_sets, pset);
//  }
//
//  return sub_sets;
//}
//
////=======================================================================================
//lattice_psets RankLattice::splitSetValue(bool val_set, bool abs, int num, int den, int off)
//{
//  lattice_psets sub_sets;
//  PSet pset;
//  bool empty = true;
//
//  if(val_set)
//  {
//    lattice_psets::iterator s_iter;
//    cont_psets::iterator r_iter;
//    for(s_iter = psets_.begin(); s_iter != psets_.end(); s_iter++)
//      for( r_iter = (s_iter->second).begin();
//           r_iter != (s_iter->second).end();
//           r_iter++)
//      {
//        //find the set that contains the value
//        //  if it exists, return a set with the value
//        //  else        , return the empty set
//
//          if(contains(r_iter->second, abs, num, den, off))
//          {
//            pset = r_iter->second;
//            pset.setLow(abs, num, den, off);
//            pset.setHigh(abs, num, den, off);
//            addHardPSet(sub_sets, pset);
//            empty = false;
//          }
//      }
//  }
//  else
//  {
//    lattice_psets sub_sets1, sub_sets2;
//    //call split with < and >
//    sub_sets1 = splitSetBound(false, abs, num, den, off-1);
//    sub_sets2 = splitSetBound(false, abs, num, den, off+1);
//    sub_sets = combinePSets(sub_sets1, sub_sets2);
//    empty = false;
//  }
//
//  if(empty)
//  {
//    pset = PSet();
//    addHardPSet(sub_sets, pset);
//  }
//
//  return sub_sets;
//}
//
//
////=======================================================================================
//// Use this function only at the if function node.
//// Determine how to split the set and return this part of psets_.
//lattice_psets RankLattice::computeTrueSet(const DataflowNode& n)
//{
//  lattice_psets t_set;
//  //TODO:
//  //find out the bound for the true set and if its a lower or a upper bound
//  std::vector<DataflowEdge> in_edges = n.inEdges();
////  if(in_edges.size() != 1)
////    ROSE_ASSERT(!"SgIfStmt should only have one in Edge");
//  in_edges = (in_edges.front().source()).inEdges();
//  DataflowNode op_node = in_edges.front().source();
//  if(isSgLessThanOp(op_node.getNode()))
//  {
//
//  }
//  if(isSgGreaterThanOp(op_node.getNode()))
//  {
//
//  }
//  if(isSgEqualityOp(op_node.getNode()))
//  {
//
//  }
//
//  isSgIfStmt(n.getNode());
//  //if(isInequality())
//
//
//  //      splitSet(bool islowerbound, bool isabs, int num, int den, int off)
//  t_set = splitSetBound(     false,              true,      0,       0,       40);
//
//  return t_set;
//}
//
////=======================================================================================
//// Use this function only at the if function node.
//// Determine how to split the set and return this part of psets_.
//lattice_psets RankLattice::computeFalseSet(const DataflowNode& n)
//{
//  lattice_psets f_set;
//  //TODO: find the real bound
//
//  f_set = splitSetBound(     true,              true,      0,       0,       41);
//
//  return f_set;
//}
//
////=======================================================================================
//// Checks for psets entry with SgNode* n and Edge condition eckFalse
//bool RankLattice::existsTrueSet(SgNode* n)
//{
//  bool exists = false;
//  std::vector< std::pair<lattice_psets, std::pair<SgNode*, LatticeSourceType> > >::iterator it;
//  for(it = psets_vec_.begin(); it != psets_vec_.end(); it++)
//  {
//    if((it->second.first == n) && (it->second.second == RankLattice::IfTrue))
//    {
//      exists = true;
//      break;
//    }
//  }
//  return exists;
//}
//
////=======================================================================================
//// Find the psets entry for SgNode* n and Edge condition eckTrue.
//lattice_psets RankLattice::getTrueSet(SgNode* n)
//{
//  lattice_psets f_set;
//  std::vector< std::pair<lattice_psets, std::pair<SgNode*, LatticeSourceType> > >::iterator it;
//  for(it = psets_vec_.begin(); it != psets_vec_.end(); it++)
//  {
//    if((it->second.first == n) && (it->second.second == RankLattice::IfTrue))
//    {
//      f_set = it->first;
//      break;
//    }
//  }
//  return f_set;
//}
//
////=======================================================================================
//// Checks for psets entry with SgNode* n and Edge condition eckFalse
//bool RankLattice::existsFalseSet(SgNode* n)
//{
//  bool exists = false;
//  std::vector< std::pair<lattice_psets, std::pair<SgNode*, LatticeSourceType> > >::iterator it;
//  for(it = psets_vec_.begin(); it != psets_vec_.end(); it++)
//  {
//    if((it->second.first == n) && (it->second.second == RankLattice::IfFalse))
//    {
//      exists = true;
//      break;
//    }
//  }
//  return exists;
//}
//
////=======================================================================================
//// Find the psets entry for SgNode* n and Edge condition eckFalse
//lattice_psets RankLattice::getFalseSet(SgNode* n)
//{
//  lattice_psets f_set;
//  std::vector< std::pair<lattice_psets, std::pair<SgNode*, LatticeSourceType> > >::iterator it;
//  for(it = psets_vec_.begin(); it != psets_vec_.end(); it++)
//  {
//    if((it->second.first == n) && (it->second.second == RankLattice::IfFalse))
//    {
//      f_set = it->first;
//      break;
//    }
//  }
//  return f_set;
//}
//
////=======================================================================================
//// Adds one process set range to psets_
//// returns true if changes happened
//// returns if added PSet was already in the psets_ (no changes)
//bool RankLattice::addPSet(PSet p_set)
//{
//  //find the lower bound of the p_set and add it at the appropriate cont_psets position
//  //merge one process set to the others.
//  return mergePSets(p_set);
//}
//
////=======================================================================================
//// Creates and adds one process set range to psets_
//// returns true if changes happened
//// returns if added PSet was already in the psets_ (no changes)
//bool RankLattice::addPSet(bool empty_range,
//                          bool lb_abs, bool hb_abs,
//                          int lnum, int hnum,
//                          int lden, int hden,
//                          int loff, int hoff)
//{
//  PSet new_set = PSet(empty_range, lb_abs, hb_abs, lnum, hnum, lden, hden, loff, hoff);
//  return addPSet(new_set);
//}
//
//
//
//
//
//
//
//// **********************************************
//// Required definition of pure virtual functions.
//// **********************************************
////=======================================================================================
//void RankLattice::initialize()
//{
//  // Use the default constructor (implemented above).
//  // So nothing to do here.
//}
//
//// returns a copy of this lattice
////=======================================================================================
//Lattice* RankLattice::copy() const
//{
//  return new RankLattice(*this);
//}
//
//
//////=======================================================================================
//////produce a hard copy of the psets_
////lattice_psets RankLattice::getPsetsCopy(lattice_psets psets)
////{
////  lattice_psets new_psets;
////
////  std::map<int, cont_psets>::iterator s_iter;
////  std::map<int, PSet>::iterator r_iter;
////  std::cerr << "\n\n############+++ COPYING PSETS HERE!!!\n\n";
////  for(s_iter = psets_.begin(); s_iter != psets_.end(); s_iter++)
////  {
////  //    if(s_iter->second != NULL)
////      for(r_iter = (s_iter->second).begin();
////          r_iter != (s_iter->second).end();
////          r_iter ++)
////      {
////        this->m
////      }
////  }
////
////  return new_psets;
////}
//
////=======================================================================================
//void RankLattice::clearPsets()
//{
//  lattice_psets::iterator s_iter;
////  std::cerr << "\n\n############+++ CLEAR PSETS HERE!!!\n\n";
//  for(s_iter = psets_.begin(); s_iter != psets_.end(); s_iter++)
//  {
//    (s_iter->second).clear();
//  }
//  psets_.clear();
//}
//
////=======================================================================================
//bool RankLattice::clearPsetsMap()
//{
//  if(this->psets_vec_.empty())
//    return false;
//  psets_vec_.clear();
//  return true;
//}
//
//// overwrites the state of "this" Lattice with "that" Lattice
////=======================================================================================
//void RankLattice::copy(Lattice* X)
//{
//  RankLattice* that = dynamic_cast<RankLattice*>(X);
//
//  this->check_pred_ = that->check_pred_;
//  this->scope_depth_ = that->scope_depth_;
//  this->clearPsets();
//  this->mergePSets(*that);
//  this->psets_vec_.clear();
//  this->pushbackPSets(*that);
//  this->size_call_ = that->size_call_;
//  this->rank_call_ = that->rank_call_;
//}
//
////=======================================================================================
//bool RankLattice::operator==(Lattice* X) /*const*/
//{
//  // Implementation of equality operator.
//  RankLattice* that = dynamic_cast<RankLattice*>(X);
//  bool eq = true;
//  eq = (scope_depth_ == that->scope_depth_) && eq;
//  eq = (check_pred_ == that->check_pred_) && eq;
//  eq = (rank_call_ == that->rank_call_) && eq;
//  eq = (size_call_ == that->size_call_) && eq;
////  eq = (psets_ == that->psets_) && eq;
//  return eq;
//}
//
////=======================================================================================
//string RankLattice::procSetsStr(string indent)
//{
//  ostringstream outs;
//  outs << "[";
//  std::map<int, cont_psets>::iterator s_iter;
//  std::map<int, PSet>::iterator r_iter;
////  outs << "!stoero! PsetsSize: " << psets_.size() << " ";
//  for(s_iter = psets_.begin(); s_iter != psets_.end(); s_iter++)
//  {
////    if(s_iter->second != NULL)
////      outs<< "!stoero! ConstPsetsSize: " << (s_iter->second).size() << " ";
//      for(r_iter = (s_iter->second).begin();
//          r_iter != (s_iter->second).end();
//          r_iter ++)
//      {
////        if(r_iter->second != NULL)
//          outs << (r_iter->second).str();
////          outs << "!stoero!";
//      }
//  }
//
//  outs << "]";
//
//  return outs.str();
//}
//
////=======================================================================================
//string RankLattice::procSetsMapStr(string indent)
//{
//  ostringstream outs;
//  outs << "[";
//  std::vector< std::pair<lattice_psets, std::pair<SgNode*, LatticeSourceType> > >::iterator m_iter;
//  lattice_psets::iterator s_iter;
//  cont_psets::iterator r_iter;
////  outs << "!stoero! PsetsSize: " << psets_.size() << " ";
//  for(m_iter = psets_vec_.begin(); m_iter != psets_vec_.end(); m_iter++)
//  {
//    outs << "[";
//    lattice_psets lp = m_iter->first;
//    for(s_iter = lp.begin(); s_iter != lp.end(); s_iter++)
//    {
//    //    if(s_iter->second != NULL)
//    //      outs<< "!stoero! ConstPsetsSize: " << (s_iter->second).size() << " ";
//        for(r_iter = (s_iter->second).begin();
//            r_iter != (s_iter->second).end();
//            r_iter ++)
//        {
//    //        if(r_iter->second != NULL)
//            outs << (r_iter->second).str();
//    //          outs << "!stoero!";
//        }
//    }
//    outs << "]";
////    outs << " NODE(" << isSgNode((m_iter->second).first)->unparseToString() <<")";
//  }
//  outs << "]";
//
//  return outs.str();
//}
//
////=======================================================================================
//string RankLattice::str(string indent)
//{
//  ostringstream outs;
//  outs << indent << "RankLattice: { "
//                 << "check predecessor: " << (check_pred_?"true\n":"false\n")
//                 << "  ProcSets:" << procSetsStr()
//                 << "}\n psets_map_SIZE: " << this->psets_vec_.size()
//                 <<"  :" << procSetsMapStr() << "\n";
//
//  return outs.str();
//}
//
//// computes the meet of this lattice and that lattice and saves the result in this lattice
//// returns true if this causes this lattice to change and false otherwise
////=======================================================================================
//bool RankLattice::meetUpdate(Lattice* X)
//{
//  RankLattice* that = dynamic_cast<RankLattice*>(X);
//
//  bool modified = false;
//
//  if(that->check_pred_ != this->check_pred_)
//  {
//    this->setScopeStmt(that->getScopeStmt());
//    modified = true;
//  }
//
//  //merge the process sets into one
//  modified = this->mergePSets(*that) || modified;
//  //add psets to the psets_map (only the ones that are not there yet
//  modified = this->pushbackUniquePSets(*that) || modified;
//
//  if(this->size_call_ != that->size_call_)
//  {
//    this->size_call_ = that->size_call_;
//    modified = true;
//  }
//
//  if(this->rank_call_ != that->rank_call_)
//  {
//    this->rank_call_ = that->rank_call_;
//    modified = true;
//  }
//
//  return modified;
//}
//
//
//// **********************************************************************
////                     RankAnalysis
//// **********************************************************************
//
////=======================================================================================
//RankAnalysis::RankAnalysis(int x)
//{
//  start_value_ = x;
//}
//
//// generates the initial lattice state for the given dataflow node,
////in the given function, with the given NodeState
////=======================================================================================
//void RankAnalysis::genInitState(const Function& func, const DataflowNode& n,
//                           const NodeState& state, std::vector<Lattice*>& initLattices,
//                           std::vector<NodeFact*>& initFacts)
//{
//  RankLattice* l = new RankLattice(start_value_);
//  //l->addPSet(empty_range, lb_abs, hb_abs, lnum, hnum, lden, hden, loff, hoff)
//  //l->addPSet(  false,       false,   false,  1,    1,    2,    1,    0,    0);
//  l->addPSet(  true,       true,   true,  0,    0,    0,    0,    0,    0);
//  initLattices.push_back(l);
//}
//
////=======================================================================================
//bool RankAnalysis::transfer(const Function& func, const DataflowNode& n,
//                            NodeState& state, const std::vector<Lattice*>& dfInfo)
//{
//  bool modified=false;
//  //Get the lattice object
//  RankLattice* lattice = dynamic_cast<RankLattice*>(*(dfInfo.begin()));
//
////  if(isMPICommRank(n.getNode()))
////  {
////    //TOD O:
////    //find out information about the rank variable name and store it as VarID
////    lattice->setRankCall(n.getNode());
////    //TOD O: remove this debug process set!
////    modified = lattice->addPSet(PSet(false, true, false, 0, 1, 0,1,20,10)) || modified;
////
////    std::cerr << "#### RankAnalysis->Transfer: communication Rank node!! \n";
////  }
////  if(isMPICommSize(n.getNode()))
////  {
////    //TOD O:
////    //find out information about the size variable name and store it as VarID
////    lattice->setSizeCall(n.getNode());
////
////    //find out information about the MPI Size variable name and store it as VarID
////
////    // create initial Process set [0 .. mpi_size]
////    // set with absolute lower bound = 0/0 mpi_size + 0
////    //          relative top bound = 1/1 mpi_size + 0
////    // get mpi_size from this CommSize call
////    //=======================
////    //FOR NOW THIS LINE IS IN THE Initialization!
////    //modified = lattice->addPSet(false, true, false, 0, 1, 0, 1, 0, 0);
////    //debug set
////    //modified = lattice->addPSet(false, true, true, 0, 0, 0, 0, 7, 30);
////    //=======================
////
////    std::cerr << "#### RankAnalysis->Transfer: communication Size node!! \n";
////  }
//
//  if(isSgFunctionCallExp(n.getNode()))
//  {
//    //Dbg::dbg << "    isSgFunctionCallExp"<<endl;
//    SgFunctionCallExp* fnCall = isSgFunctionCallExp(n.getNode());
//    Function calledFunc(fnCall);
//
//    if(calledFunc.get_name().getString() == "MPI_Comm_rank")
//    {
//      //Dbg::dbg << "        MPI_Comm_rank"<<endl;
//      SgExpressionPtrList& args = fnCall->get_args()->get_expressions();
//      // args[1]
//      SgExpression* arg1 = *(++(args.begin()));
//      //printf("arg1 = [%s | %s]]\n", arg1->class_name().c_str(), arg1->unparseToString().c_str());
//
//      // Look at MPI_Comm_rank's second argument and record that it depends on the process' rank
//      if(isSgAddressOfOp(arg1) && varID::isValidVarExp(isSgAddressOfOp(arg1)->get_operand()))
//      {
//        //varID rankVar((isSgVarRefExp(arg1))->get_symbol()->get_declaration());
//        rankVar_ = SgExpr2Var(isSgAddressOfOp(arg1)->get_operand());
////        MPIRankNProcsDepLattice* rnLat = dynamic_cast<MPIRankNProcsDepLattice*>(prodLat->getVarLattice(rankVar));
////        Dbg::dbg << "rankVar="<<rankVar<<" rnLat="<<rnLat<<endl;
////        Dbg::dbg << "prodLat="<<prodLat->str("    ")<<endl;
////        modified = rnLat->setRankDep(true) || modified;
////        Dbg::dbg << "prodLat="<<prodLat->str("    ")<<endl;
//        modified = lattice->addPSet(PSet(false, true, false, 0, 1, 0,1,20,10)) || modified;
//      }
//      else
//      { fprintf(stderr, "[rankDepAnalysis] MPIRankDepAnalysis::transfer() ERROR: second argument \"%s\" of function call \"%s\" is not a reference to a variable!\n", arg1->unparseToString().c_str(), (n.getNode())->unparseToString().c_str()); exit(-1); }
//    }
//    else if(calledFunc.get_name().getString() == "MPI_Comm_size")
//    {
//      //Dbg::dbg << "        MPI_Comm_rank"<<endl;
//      SgExpressionPtrList& args = fnCall->get_args()->get_expressions();
//      // args[1]
//      SgExpression* arg1 = *(++(args.begin()));
//
//      // look at MPI_Comm_size's second argument and record that it depends on the number of processes
//      if(isSgAddressOfOp(arg1) && varID::isValidVarExp(isSgAddressOfOp(arg1)->get_operand()))
//      {
//        //varID rankVar((isSgVarRefExp(arg1))->get_symbol()->get_declaration());
//        sizeVar_ = SgExpr2Var(isSgAddressOfOp(arg1)->get_operand());
//
////        MPIRankNProcsDepLattice* rnLat = dynamic_cast<MPIRankNProcsDepLattice*>(prodLat->getVarLattice(nprocsVar));
////        modified = rnLat->setNprocsDep(true);
//      }
//      else
//      { fprintf(stderr, "[rankDepAnalysis] MPIRankDepAnalysis::transfer() ERROR: second argument \"%s\" of function call \"%s\" is not a reference to a variable!\n", arg1->unparseToString().c_str(), (n.getNode())->unparseToString().c_str()); exit(-1); }
//    }
//  }
//
//
//  // If this condition is true, the node is a direct successor of a conditional statement
//  // Therefore, clear the psets_ and build them new, with:
//  //    all p_sets from predecessors that are not conditional statements
//  //    the correct (true/false) psets_ from the conditional statements
//  if((lattice->getScopeStmt()))
//  {
//    std::cerr << "\n#### This is a CONDITIONAL SUCCESSOR NODE ###\n\n" << lattice->str()
////              << isSgNode(n.getNode())->class_name() << " at line nr: "
////              << isSgNode(n.getNode())->get_file_info()->get_line()
//              <<"\n";
//
//    lattice->clearPsets();
//
////    std::cerr << "Cleared Psets: " << lattice->str() << "\n";
//
//    vector<DataflowEdge> in_edges = n.inEdges();
//    vector<DataflowEdge>::iterator it;
//    for(it=in_edges.begin(); it !=in_edges.end(); it ++)
//    {
//      if((it->condition() == eckTrue || it->condition() == eckFalse)
//          && (isIfRankDep((it->source()).getNode())) )
//      {
//        std::cerr << "***********conditional merge ";
//        (it->condition() == eckFalse)? std::cerr<< "false\n" :std::cerr << "true\n";
//
//
//        //get the true or false set depending on the eckCondition and merge it to the state
//        if( (it->condition() == eckTrue) &&
//            (lattice->existsTrueSet(it->source().getNode())))
//        {
//          modified = lattice->mergePSets(lattice->getTrueSet(it->source().getNode())) || modified;
//          std::cerr << "###eckTrue merge: " << lattice->str() << "\n";
//        }
//        if( (it->condition() == eckFalse) &&
//            (lattice->existsFalseSet(it->source().getNode())))
//        {
//          modified = lattice->mergePSets(lattice->getFalseSet(it->source().getNode())) || modified;
//          std::cerr << "###eckFalse merge: " << lattice->str() << "\n";
//        }
//      }
//      else
//      {
//        //If it is an edge not from a conditional statement, just merge the source lattice
//        modified = lattice->mergePSets(
//                             lattice->getPsets(RankLattice::Standard, n.getNode()))
//                   || modified;
//
//        std::cerr << "Not conditional merge: " << lattice->str() << "\n";
//      }
////      std::cerr << "EDGE: " << it->toString() << "\n";
//    }
//    //Set the scope statement flag for the next node in order to check back on the process set range
//    modified = lattice->setScopeStmt(false) || modified;
//  }
//  //Clear psets_map after all sets are matched.
//  std::cerr << "#### This is the same CONDITIONAL SUCCESSOR NODE 1 ###\n" << lattice->str() << "\n";
//  modified = lattice->clearPsetsMap() || modified;
//  std::cerr << "#### This is the same CONDITIONAL SUCCESSOR NODE 2 ###\n" << lattice->str() << "\n";
//
//  //If we reached an If statement
//  if(isSgIfStmt(n.getNode()) && isIfRankDep(n.getNode()))
//  {
//    std::cerr << "\n#### This is an SgIfStmt ###\n\n" << lattice->str() << "\n\n";
//
//    lattice_psets fsets = lattice->computeFalseSet(n);
//    lattice->pushbackPSets(fsets, RankLattice::IfFalse, n.getNode());
//    lattice_psets tsets = lattice->computeTrueSet(n);
//    lattice->pushbackPSets(tsets, RankLattice::IfTrue, n.getNode());
//
//    modified = lattice->setScopeStmt(true) || modified;
//  }
//
//  //Every node, add the psets_ to the psets_map in order to access the psets_ at the successor node.
//  lattice->pushbackPSets(lattice->getPSets(),RankLattice::Standard,n.getNode());
//
//  return modified;
//}
//
//
//// **********************************************************************
////                   If related  Decisions
//// **********************************************************************
//
////=======================================================================================
////Return ture if: It is Rank Dependend and
////                a const value or value from constant propagation
//bool RankAnalysis::isIfRankDep(SgNode* node)
//{
//  if(!isSupportedStmt(node))
//    return false;
//
//  if(!containsRankVar(node))
//    return false;
//
//  if(!onlyConstValues(node))
//    return false;
//
//  return true;
//}
//
////=======================================================================================
////Check if it is a simple inequality that we can handle
//bool RankAnalysis::isSupportedStmt(SgNode* node)
//{
////TODO:
//
//  return true;
//}
//
////=======================================================================================
////Check if it is a simple inequality that we can handle
//bool RankAnalysis::containsRankVar(SgNode* node)
//{
////TODO:
//
//  return true;
//}
//
////=======================================================================================
////Check if it is a simple inequality that we can handle
//bool RankAnalysis::onlyConstValues(SgNode* node)
//{
////TODO:
//
//  return true;
//}
//
//
//// **********************************************************************
////                     Rank utils stuff
//// **********************************************************************
////=======================================================================================
//bool RankAnalysis::isMPICommRank(SgNode* node)
//{
//  string name;
//  if( isSgFunctionRefExp(node)) {
//    name = isSgFunctionRefExp(node)->get_symbol()->get_name();
//    if(name == "MPI_Comm_rank")
//      return true;
//  }
//  return false;
//}
//
////=======================================================================================
//bool RankAnalysis::isMPICommSize(SgNode* node)
//{
//  string name;
//  if( isSgFunctionRefExp(node)) {
//    name = isSgFunctionRefExp(node)->get_symbol()->get_name();
//    if(name == "MPI_Comm_size")
//      return true;
//  }
//  return false;
//}
//
////=======================================================================================
////TOD O this function doesn't do useful stuff
////bool RankAnalysis::setSizeAndRank(SgProject* project)
////{
////  bool found_size = false;
////  bool found_rank = false;
////  project_ = project;
////
////  //can't find the function where the computation before the actual analysis can be done.
////  //TOD O move it there when you know what I'm talking about :)
////  Rose_STL_Container<SgNode*> fCallExpList = NodeQuery::querySubTree(project_, V_SgFunctionRefExp);
////  Rose_STL_Container<SgNode*>::iterator iter;
////  for(iter = fCallExpList.begin(); iter < fCallExpList.end(); iter++)
////  {
////    //TOD O: move this to a separate Analysis in order to support MPI_Comm_split with multiple
////    //      MPI_Comm_rank and MPI_Comm_size functioncalls
////    if(isMPICommRank(*iter))
////    {
////      if(found_rank)
////        std::cerr << "MPI Rank Analysis: Warning, multiple MPI_Comm_rank function Calls.\n";
////      else
////      {
////        //find out information about the rank variable name and store it as VarID
////        std::cerr << "#### communication Rank node!! \n";
////        found_rank = true;
////      }
////    }
////    if(isMPICommSize(*iter))
////    {
////      if(found_rank)
////        std::cerr << "MPI Rank Analysis: Warning, multiple MPI_Comm_size function Calls.\n";
////      else
////      {
////        //find out information about the MPI Size variable name and store it as VarID
////        //also create initial Process set [0 .. mpi_size]
////        std::cerr << "#### communication Size node!! \n";
////        found_size = true;
////      }
////    }
////  }
////
////  return (found_size && found_rank);
////}
