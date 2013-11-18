#include "rose.h"
#include "rankAnalysis/FinestPSetGranularity.h"
#include "rankAnalysis/rankAnalysis.h"
#include "rankAnalysis/pSet.h"


//=======================================================================================
void FinestPSetGranularity::visit(const Function& func,
                                  const DataflowNode& n,
                                  NodeState& state)
{
  if(state.isInitialized(ra_))
  {
    std::vector<Lattice*> lv = state.getLatticeAbove(ra_);
    if(lv.empty())
      std::cerr << "ERROR lattice vector == NULL!\n";
    Lattice* lattice = *(lv.begin());
    if(lattice == NULL)
      std::cerr << "ERROR lattice == NULL\n";
    RankLattice* rank_lattice = dynamic_cast<RankLattice*>(lattice);

    if(rank_lattice != NULL)
    {
      if(rrankAnalysisDebugLevel > 1)
        std::cerr << "\n" << rank_lattice->toString();

      std::vector<PSet> ps = rank_lattice->getPSets();
      std::vector<PSet>::const_iterator it;
      for(it = ps.begin(); it != ps.end(); ++it)
      {
        if(!it->isEmpty())
        {
          this->all_bounds_.insert(_Directed_Bound_(it->getLBound(),true));
          this->all_bounds_.insert(_Directed_Bound_(it->getHBound(),false));
        }
      }
    }
  }
}

//=======================================================================================
void FinestPSetGranularity::buildPSets()
{
  all_psets_.clear();
  if(all_bounds_.empty())
  {
    std::cerr << "Run Analysis before optaining results.";
  }
  else if(all_bounds_.size() == 1)
  {
    std::set<_Directed_Bound_>::const_iterator db = all_bounds_.begin();
    if(db->is_lb_)
      all_psets_.push_back(PSet(false, db->b_, _Bound_(false, 1, 1, 0)));
    else
      all_psets_.push_back(PSet(false, _Bound_(true, 1, 1, 0), db->b_));
  }
  else
  {
    std::set<_Directed_Bound_>::const_iterator it = all_bounds_.begin();

    _Bound_ lb, hb;
    hb = _Bound_(true, 1, 1, -1);
    while(it != all_bounds_.end())
    {
      if(it->is_lb_)
      {
        lb = it->b_;
        ++it;
      }
      else
        lb = hb + 1;

      if(it != all_bounds_.end())
      {
        if(it->is_lb_)
          hb = it->b_-1;
        else
        {
          hb = it->b_;
          ++it;
        }
      }
      else
        hb = _Bound_(false, 1, 1, 0);

      all_psets_.push_back(PSet(false, lb, hb));
    }
  }
}

//=======================================================================================
std::vector<PSet> FinestPSetGranularity::getPSets()
{
  return all_psets_;
}

//=======================================================================================
//TODO: should I call getPSets  inside the print function?????
string FinestPSetGranularity::toStr() const
{
    ostringstream out;
    out << "\n\nFinestPSetGranularityResult: \n  DIRECTED_BOUND:  ";
    std::set<_Directed_Bound_>::const_iterator itb;
    for(itb = this->all_bounds_.begin(); itb != this->all_bounds_.end(); ++itb)
      out << " " << itb->toStr();

    out << "\n  PSETS:  ";
    std::vector<PSet>::const_iterator itp;
    for(itp = this->all_psets_.begin(); itp != this->all_psets_.end(); ++itp)
      out << " " << itp->toString();
    return out.str();
}
