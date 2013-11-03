/*
 * RankLattice.cpp
 *
 *  Created on: Oct 19, 2013
 *      Author: stoero
 */

#include "rankAnalysis/RankLattice.h"

extern int rrankAnalysisDebugLevel;

// **********************************************************************
//                      RankLattice
// **********************************************************************
//=======================================================================================
RankLattice::RankLattice()
{
  this->check_pred_ = false;
  this->pset_cont_.clear();
  this->pset_cont_.push_back(PSet());
  this->out_pset_cont_.clear();
  this->out_pset_cont_.push_back(PSet());
  this->out_psets_vec_.clear();
}

//=======================================================================================
RankLattice::RankLattice( int v )
{
  this->check_pred_ = false;
  this->pset_cont_.clear();
  this->pset_cont_.push_back(PSet());
  this->out_pset_cont_.clear();
  this->out_pset_cont_.push_back(PSet());
  this->out_psets_vec_.clear();
}

//=======================================================================================
RankLattice::RankLattice (const RankLattice& X)
{
  this->check_pred_ = X.check_pred_;
  this->pset_cont_ = X.pset_cont_;
  this->out_pset_cont_ = X.out_pset_cont_;
  this->out_psets_vec_ = X.out_psets_vec_;
  this->node_ = X.node_;
}

//=======================================================================================
bool RankLattice::isDepCondScope() const
{
  return check_pred_;
}

//=======================================================================================
bool RankLattice::setDepCondScope(bool stmt)
{
  bool modified = this->check_pred_ != stmt;
  this->check_pred_ = stmt;
  return modified;
}

//=======================================================================================
SgNode* RankLattice::getNode() const
{
  return node_;
}

//=======================================================================================
bool RankLattice::setNode(SgNode* node)
{
  if(this->node_ == node)
    return false;
  this->node_ = node;
  return true;
}

//=======================================================================================
std::vector<PSet> RankLattice::getPSets() const
{
  return this->pset_cont_;
}

//=======================================================================================
std::vector<PSet> RankLattice::getPSets(const SgNode* n) const
{
  std::vector<psetStruct>::const_iterator it;
  for(it = this->out_psets_vec_.begin(); it != this->out_psets_vec_.end(); ++it)
  {
    psetStruct ps = *it;
    if(ps.n_source_ == n )
    {
      std::vector<PSet> v;
      v.assign(ps.psets_.begin(), ps.psets_.end());
      return v;
    }
  }
  std::cerr << "\nERROR: There should be a PSet for the if and else branch "
            << "at the conditional statement.\nReturning [empty] set as default.";
  std::cerr << "\n" << this->toString();
  std::cerr << "\nNODE: " << n;

  std::vector<PSet> v;
  v.push_back(PSet());
  return v;
}

//=======================================================================================
std::vector<PSet> RankLattice::getOutPSets() const
{
  return this->out_pset_cont_;
}

//=======================================================================================
std::vector<PSet> RankLattice::getOutPSets(const SgNode* n) const
{
  return getPSets(n);
}

//=======================================================================================
void RankLattice::setPset(const PSet& ps)
{
  this->pset_cont_.clear();
  this->pset_cont_.push_back(ps);
}

//=======================================================================================
void RankLattice::setPsets(const std::vector<PSet>& ps)
{
  this->pset_cont_.assign(ps.begin(), ps.end());
}

//=======================================================================================
void RankLattice::setOutPset(const PSet& ps)
{
  this->out_pset_cont_.clear();
  this->out_pset_cont_.push_back(ps);
}

//=======================================================================================
void RankLattice::setOutPsets(const std::vector<PSet>& ps)
{
  this->out_pset_cont_.assign(ps.begin(), ps.end());
}

#if 0
//=======================================================================================
// Checks for psets entry with SgNode* n and Edge condition eckFalse
bool RankLattice::existsTrueSet(SgNode* n)
{
  bool exists = false;
  std::vector< std::pair<lattice_psets, std::pair<SgNode*, LatticeSourceType> > >::iterator it;
  for(it = psets_vec_.begin(); it != psets_vec_.end(); it++)
  {
    if((it->second.first == n) && (it->second.second == RankLattice::IfTrue))
    {
      exists = true;
      break;
    }
  }
  return exists;
}

//=======================================================================================
// Find the psets entry for SgNode* n and Edge condition eckTrue.
lattice_psets RankLattice::getTrueSet(SgNode* n)
{
  lattice_psets f_set;
  std::vector< std::pair<lattice_psets, std::pair<SgNode*, LatticeSourceType> > >::iterator it;
  for(it = psets_vec_.begin(); it != psets_vec_.end(); it++)
  {
    if((it->second.first == n) && (it->second.second == RankLattice::IfTrue))
    {
      f_set = it->first;
      break;
    }
  }
  return f_set;
}

//=======================================================================================
// Checks for psets entry with SgNode* n and Edge condition eckFalse
bool RankLattice::existsFalseSet(SgNode* n)
{
  bool exists = false;
  std::vector< std::pair<lattice_psets, std::pair<SgNode*, LatticeSourceType> > >::iterator it;
  for(it = psets_vec_.begin(); it != psets_vec_.end(); it++)
  {
    if((it->second.first == n) && (it->second.second == RankLattice::IfFalse))
    {
      exists = true;
      break;
    }
  }
  return exists;
}

//=======================================================================================
// Find the psets entry for SgNode* n and Edge condition eckFalse
lattice_psets RankLattice::getFalseSet(SgNode* n)
{
  lattice_psets f_set;
  std::vector< std::pair<lattice_psets, std::pair<SgNode*, LatticeSourceType> > >::iterator it;
  for(it = psets_vec_.begin(); it != psets_vec_.end(); it++)
  {
    if((it->second.first == n) && (it->second.second == RankLattice::IfFalse))
    {
      f_set = it->first;
      break;
    }
  }
  return f_set;
}
#endif

//=======================================================================================
bool RankLattice::addPSet(PSet p_set)
{
  return mergePSets(p_set);
}

#if 0
//=======================================================================================
void RankLattice::addHardPSet(PSet pset)
{
  addHardPSet(this->pset_cont_, pset);
}

//=======================================================================================
void RankLattice::addHardPSet(std::vector<PSet>& psets, PSet pset)
{
  //use the insert function to insert the p_set on the proper position
  //what is the proper position?
  //search till you find the position where
  std::vector<PSet>::iterator it = psets.begin();
  while(it != psets.end() && pset > *it)
    ++it;
  psets.insert(it, pset);
  //TODO remove debug output
  std::cerr << "\nAdd hard a process set to the existing ones!";
}
#endif

#if 0
//=======================================================================================
//combines the range of both p_sets and returns a new p_set
PSet RankLattice::combinePSets(const PSet p_set1, const PSet p_set2)
{
  return PSet(p_set1.combineWith(p_set2));
}
#endif

//=======================================================================================
void RankLattice::attachPSets(const psetStruct& sets)
{
  psetStruct pss = psetStruct(sets);
  out_psets_vec_.push_back(pss);
}

#if 0
//=======================================================================================
lattice_psets RankLattice::getPsets(RankLattice::LatticeSourceType source_type,
                                     SgNode* n)
{
  lattice_psets pset;
  std::vector< std::pair<lattice_psets, std::pair<SgNode*, LatticeSourceType> > >::iterator it;
  for(it = this->psets_vec_.begin(); it != this->psets_vec_.end(); it++)
  {
    if((it->second.first == n) && (it->second.second == source_type))
    {
      pset = (it->first);
      it = this->psets_vec_.end();
    }
  }
  return pset;
}
#endif

//=======================================================================================
bool RankLattice::mergePSets(const RankLattice& lattice)
{
  return mergePSets(lattice.getOutPSets());
}

//=======================================================================================
bool RankLattice::mergePSets(const std::vector<PSet>& psets)
{
  bool modified = false;
  if(rrankAnalysisDebugLevel >= 2)
    std::cerr << "\nMERGE FUNCTION:  This: "
              << this->procSetsStr(this->pset_cont_)
              << "   That: "
              << this->procSetsStr(psets);

  std::vector<PSet>::const_iterator it;

  for(it = psets.begin(); it != psets.end(); ++it)
    modified = this->mergePSets(*it) || modified;

  if(rrankAnalysisDebugLevel >= 2)
  {
    std::cerr << "  TO: modified = ";
    if(modified)
      std::cerr << "true";
    else
      std::cerr << "false";
    std::cerr << "  Result: " << this->procSetsStr(this->pset_cont_);
  }

  return modified;
}

//=======================================================================================
bool RankLattice::mergePSets(const PSet& p_set)
{
  bool modified = false;
  bool covered = false;

  if(p_set.isEmpty())
    return false;

  if(this->pset_cont_.size() == 1)
    if(this->pset_cont_[0].isEmpty())
    {
      this->pset_cont_.pop_back();
      this->pset_cont_.push_back(p_set);
      return true;
    }

  std::vector<PSet> u_set;
  PSet in_set = p_set;
  std::vector<PSet>::iterator it;

  for(it = this->pset_cont_.begin(); it != this->pset_cont_.end(); ++it)
  {
    if(covered)
    {
      u_set.push_back(*it);
    }
    else
    {
      if(in_set == *it)
      {
        u_set.push_back(*it);
        covered = true;
      }
      if(in_set < *it && !in_set.interleavesOrTouches(*it))
      {
        u_set.push_back(in_set);
        u_set.push_back(*it);
        modified = true;
        covered = true;
      }
      if(*it < in_set && !in_set.interleavesOrTouches(*it))
      {
        u_set.push_back(*it);
        modified = true;
      }
      if(in_set.interleavesOrTouches(*it))
      {
        in_set = in_set.combineWith(*it);
        modified = true;
      }
    }
  }
  if(!covered)
  {
    u_set.push_back(in_set);
    modified = true;
  }

  this->pset_cont_.clear();
  for(it = u_set.begin(); it != u_set.end(); ++it)
  {
    this->pset_cont_.push_back(*it);
  }
  return modified;
}

#if 0
//=======================================================================================
lattice_psets RankLattice::combinePSets(lattice_psets sets1, lattice_psets sets2)
{
  lattice_psets psets;
  PSet p_set;

  std::map<int, cont_psets>::iterator s_iter;
  std::map<int, PSet>::iterator r_iter;
  for(s_iter = sets1.begin(); s_iter != sets1.end(); s_iter++)
  {
    for( r_iter = (s_iter->second).begin();
         r_iter != (s_iter->second).end();
         r_iter++)
    {
      p_set = r_iter->second;
      addHardPSet(psets, p_set);
    }
  }
  for(s_iter = sets2.begin(); s_iter != sets2.end(); s_iter++)
  {
    for( r_iter = (s_iter->second).begin();
         r_iter != (s_iter->second).end();
         r_iter++)
    {
      p_set = r_iter->second;
      addHardPSet(psets, p_set);
    }
  }

  return psets;
}
#endif

//=======================================================================================
bool RankLattice::pushPSetToOutSet()
{
  if(pset_cont_ == out_pset_cont_)
    return false;
  else
    out_pset_cont_.assign(pset_cont_.begin(), pset_cont_.end());
  return true;
}

//=======================================================================================
bool RankLattice::pushPSetToOutVec(const std::vector<PSet>& psv, const SgNode* n)
{
  //TODO: verify .... maybe remove the psetStruct ... it is not required.
//  std::cerr << "\npushPSetToOutVec: " << this->procSetsStr(psv);
  psetStruct ps = psetStruct(psv, n);
//  std::cerr << "\npsetStruct: " << this->procSetsStr(ps.psets_);
  std::vector<psetStruct>::iterator it = out_psets_vec_.begin();

  for(; it != out_psets_vec_.end(); ++it)
  {
    if(*it == ps)
      return false;
    if(it->n_source_ == n)
    {
      out_psets_vec_.erase(it);
      break;
    }
  }

  out_psets_vec_.push_back(ps);
//  std::cerr << "\nOUTVEC: " << this->procSetsVecStr();
  return true;
}

#if 0
//=======================================================================================
lattice_psets RankLattice::splitPSetsAtBound(bool lowbound, bound b)
{
  lattice_psets sub_sets;
  PSet pset;
  bool empty = true;

  lattice_psets::iterator s_iter;
  cont_psets::iterator r_iter;

  for(s_iter = psets_.begin(); s_iter != psets_.end(); s_iter++)
    for( r_iter = (s_iter->second).begin();
         r_iter != (s_iter->second).end();
         r_iter++)
    {
      if(lowbound)
      {
//        if(isOver(r_iter->second, abs, num, den, off))
        if(b > (r_iter->second).getHBound())
        {
          pset = r_iter->second;
          addHardPSet(sub_sets, pset);
          empty = false;
        }
//        if(contains(r_iter->second, abs, num, den, off))
        if((r_iter->second).contains(b))
        {
          pset = r_iter->second;
          pset.setLBound(b);
          addHardPSet(sub_sets, pset);
          empty = false;
        }
      }
      else
      {
//        if(isUnder(r_iter->second, abs, num, den, off))
        if(b < (r_iter->second).getLBound())
        {
          pset = r_iter->second;
          addHardPSet(sub_sets, pset);
          empty = false;
        }
//        if(contains(r_iter->second, abs, num, den, off))
        if((r_iter->second).contains(b))
        {
          pset = r_iter->second;
          pset.setHBound(b);
          addHardPSet(sub_sets, pset);
          empty = false;
        }
      }

    }
  if(empty)
  {
    pset = PSet();
    addHardPSet(sub_sets, pset);
  }

  return sub_sets;
}

//=======================================================================================
// Use this function only at the if function node.
// Determine how to split the set and return this part of psets_.
lattice_psets RankLattice::computeTrueSet(const DataflowNode& n)
{
  lattice_psets t_set;
  //TODO:
  //find out the bound for the true set and if its a lower or a upper bound
  std::vector<DataflowEdge> in_edges = n.inEdges();
//  if(in_edges.size() != 1)
//    ROSE_ASSERT(!"SgIfStmt should only have one in Edge");
  in_edges = (in_edges.front().source()).inEdges();
  DataflowNode op_node = in_edges.front().source();
  if(isSgLessThanOp(op_node.getNode()))
  {

  }
  if(isSgGreaterThanOp(op_node.getNode()))
  {

  }
  if(isSgEqualityOp(op_node.getNode()))
  {

  }

  isSgIfStmt(n.getNode());
  //if(isInequality())


  //      splitSet(bool islowerbound, bool isabs, int num, int den, int off)
  bound b_debug = bound(true,0,0,80);
  t_set = splitPSetsAtBound(false, b_debug);

  return t_set;
}

//=======================================================================================
// Use this function only at the if function node.
// Determine how to split the set and return this part of psets_.
lattice_psets RankLattice::computeFalseSet(const DataflowNode& n)
{
  lattice_psets f_set;
  //TODO: find the real bound

  bound b_debug = bound(true,0,0,60);
  f_set = splitPSetsAtBound(false, b_debug);

  return f_set;
}
#endif

#if 0
//=======================================================================================
void RankLattice::clearPsets()
{
  lattice_psets::iterator s_iter;
//  std::cerr << "\n\n############+++ CLEAR PSETS HERE!!!\n\n";
  for(s_iter = psets_.begin(); s_iter != psets_.end(); s_iter++)
  {
    (s_iter->second).clear();
  }
  psets_.clear();
}

//=======================================================================================
bool RankLattice::clearPsetsMap()
{
  if(this->psets_vec_.clear())
    return false;
  psets_vec_.clear();
  return true;
}
#endif

//=======================================================================================
std::vector<PSet> RankLattice::intersect(const PSet& ps) const
{
  std::vector<PSet> ps_vec;
  PSet result;
  std::vector<PSet>::const_iterator it = pset_cont_.begin();
  for(; it < pset_cont_.end(); ++it)
  {
    result = it->intersectWith(ps);
    if(!result.isEmpty())
      ps_vec.push_back(result);
  }
  if(ps_vec.empty())
    ps_vec.push_back(PSet());

  return ps_vec;
}

//=======================================================================================
std::vector<PSet> RankLattice::remove(const PSet& ps) const
{
  std::vector<PSet> ps_vec;
  std::vector<PSet> result;
  std::vector<PSet>::const_iterator it = pset_cont_.begin();
  for(; it < pset_cont_.end(); ++it)
  {
    result = it->remove(ps);
    std::vector<PSet>::const_iterator iter = result.begin();
    for(; iter < result.end(); ++iter)
      if(!iter->isEmpty())
        ps_vec.push_back(*iter);
  }
  if(ps_vec.empty())
    ps_vec.push_back(PSet());

  return ps_vec;
}

// **********************************************
// Required definition of pure virtual functions.
// **********************************************
// computes the meet of this lattice and that lattice and saves the result in this lattice
// returns true if this causes this lattice to change and false otherwise
//=======================================================================================
bool RankLattice::meetUpdate(Lattice* X)
{
  std::cerr << "\n MEEET UPDATE";
  RankLattice* that = dynamic_cast<RankLattice*>(X);

  if(that->isDepCondScope())
    return mergePSets(that->getOutPSets(node_));
  else
    return mergePSets(that->getOutPSets());

#if 0
  RankLattice* that = dynamic_cast<RankLattice*>(X);

  if(that->isDepCondScope())
  {
    return this->mergePSets(that->getOutPSets(this->node_));
  }
  else
    return this->mergePSets(that->getOutPSets());
#endif
}

//=======================================================================================
void RankLattice::initialize()
{
  this->check_pred_ = false;
  this->pset_cont_.clear();
  this->pset_cont_.push_back(PSet());
  this->out_pset_cont_.clear();
  this->out_pset_cont_.push_back(PSet());
  this->out_psets_vec_.clear();
}

// returns a copy of this lattice
//=======================================================================================
Lattice* RankLattice::copy() const
{
  return new RankLattice(*this);
}

// overwrites the state of "this" Lattice with "that" Lattice
//=======================================================================================
void RankLattice::copy(/*const*/ Lattice* X)
{
  RankLattice* that = dynamic_cast<RankLattice*>(X);
  this->check_pred_ = that->check_pred_;
  this->node_ = that->node_;
  this->pset_cont_.assign(that->pset_cont_.begin(), that->pset_cont_.end());
  this->out_pset_cont_.assign(that->out_pset_cont_.begin(), that->out_pset_cont_.end());
  this->out_psets_vec_.assign(that->out_psets_vec_.begin(), that->out_psets_vec_.end());
}

//=======================================================================================
bool RankLattice::operator==(Lattice* X) /*const*/
{
  RankLattice* that = dynamic_cast<RankLattice*>(X);
  if(this->check_pred_ != that->check_pred_)
    return false;
  if(this->node_ != that->node_)
    return false;

  if(this->pset_cont_ != that->pset_cont_)
    return false;

  if(this->out_pset_cont_ != that->out_pset_cont_)
    return false;

  if(this->out_psets_vec_ != that->out_psets_vec_)
    return false;

  return true;
}

//=======================================================================================
string RankLattice::str(string indent)
{
  ostringstream outs;
  outs << indent << toString();
  return outs.str();
}

//=======================================================================================
string RankLattice::toString() const
{
  ostringstream outs;
  outs << "\nRankLattice: { "
       << "check predecessor: " << (check_pred_?"true\n":"false\n")
       << "  ProcSets: " << procSetsStr(this->pset_cont_)
       << "  OUT PRoc Sets: " << procSetsStr(this->out_pset_cont_)
       << "}\n out_psets_vec_SIZE: " << this->out_psets_vec_.size()
       <<"  :" << this->procSetsVecStr();

  return outs.str();
}

//=======================================================================================
string RankLattice::psetsToString() const
{
  ostringstream outs;
  outs << (check_pred_ ? "ifStmt = true\n" : "")
       << "PSets: " << procSetsStr(this->pset_cont_)
       << (check_pred_ ? this->procSetsVecStr() : "");

  return outs.str();
}

//=======================================================================================
string RankLattice::toStringForDebugging() const
{
  ostringstream outs;
  outs << "\nRankLattice: { "
       << "check predecessor: " << (check_pred_?"true\n":"false\n")
       << "  ProcSets: " << procSetsStr(this->pset_cont_)
       << "  OUT PRoc Sets: " << procSetsStr(this->out_pset_cont_)
       << "}\n out_psets_vec_SIZE: " << this->out_psets_vec_.size()
       <<"  :" << this->procSetsVecStr();
  return outs.str();
}

//=======================================================================================
string RankLattice::procSetsStr(const std::vector<PSet>& ps) const
{
  ostringstream outs;
  outs << "[";
  std::vector<PSet>::const_iterator it;
  for(it = ps.begin(); it != ps.end(); ++it)
  {
    outs << it->toString();
  }
  outs << "]";

  return outs.str();
}

//=======================================================================================
string RankLattice::procSetsVecStr() const
{
  ostringstream outs;
  outs << "\n PROCESS SET VECTOR CONTENT:";

  std::vector<psetStruct>::const_iterator it;
  for(it = this->out_psets_vec_.begin(); it != this->out_psets_vec_.end(); ++it)
  {
    outs << "\no ";
    outs << this->procSetsStr(it->psets_);
    outs << " for NODE(" << isSgNode((it->n_source_))->unparseToString() <<")";
  }
  outs << "\n";

  return outs.str();
}
