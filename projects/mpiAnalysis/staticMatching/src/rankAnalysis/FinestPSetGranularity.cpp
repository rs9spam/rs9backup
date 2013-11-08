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
          this->all_bounds_.insert(directed_bound(it->getLBound(),true));
          this->all_bounds_.insert(directed_bound(it->getHBound(),false));
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
    std::set<directed_bound>::const_iterator db = all_bounds_.begin();
    if(db->is_lb_)
      all_psets_.push_back(PSet(false, db->b_, bound(false, 1, 1, 0)));
    else
      all_psets_.push_back(PSet(false, bound(true, 1, 1, 0), db->b_));
  }
  else
  {
    std::set<directed_bound>::const_iterator it = all_bounds_.begin();

    bound lb, hb;
    hb = bound(true, 1, 1, -1);
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
        hb = bound(false, 1, 1, 0);

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
    std::set<directed_bound>::const_iterator itb;
    for(itb = this->all_bounds_.begin(); itb != this->all_bounds_.end(); ++itb)
      out << " " << itb->toStr();

    out << "\n  PSETS:  ";
    std::vector<PSet>::const_iterator itp;
    for(itp = this->all_psets_.begin(); itp != this->all_psets_.end(); ++itp)
      out << " " << itp->toString();
    return out.str();
}

#if 0
int main(int argc, char *argv[])
{
  // Build the AST used by ROSE
  SgProject* project = frontend(argc,argv);
  ROSE_ASSERT (project != NULL);
  initAnalysis(project);
//  analysisDebugLevel=0;
  Dbg::init("RANK ANALYSIS TEST", ".", "index.html");
//  rankAnalysisDebugLevel = 0;
  CallGraphBuilder cgb(project);
  cgb.buildCallGraph();
  SgIncidenceDirectedGraph* graph = cgb.getGraph();
  RankAnalysis rankA(project);
  ContextInsensitiveInterProceduralDataflow rankInter(&rankA, graph);
  rankInter.runAnalysis();


  FinestPSetGranularity fpsg(&rankA);
  UnstructuredPassInterAnalysis upia_fpsg(fpsg);
  upia_fpsg.runAnalysis();

  return 0;
}
#endif
