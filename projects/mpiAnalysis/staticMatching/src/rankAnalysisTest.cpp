#include "rose.h"
#include "rankAnalysis/rankAnalysis.h"

int rankAnalysisTestDebugLevel = 0;

bool cfgNodeFilter (CFGNode cfgn)
{
  SgNode * node = cfgn.getNode();
  assert (node != NULL) ;
  bool ret = true;
  if (isSgInitializedName(node))
    ret = (cfgn == node->cfgForEnd());
  else
    ret = (cfgn.isInteresting());

  if(rankAnalysisTestDebugLevel > 1)
  {
    if(ret)
      std::cerr << "#";
    else
      std::cerr << "_";
  }

  return ret;
}

class EvaluationPass: public UnstructuredPassIntraAnalysis
{
public:
    RankAnalysis *ra;
    int pass, fail;
    bool failure;
    bool (*filter) (CFGNode cfgn);

    EvaluationPass(RankAnalysis* that_ra, bool (*f)(CFGNode))
        : Analysis(f), ra(that_ra), filter(f)
    {
      pass = 0;
      fail = 0;
      failure = false;
    }

    void visit(const Function& func, const DataflowNode& n, NodeState& state);
};

void EvaluationPass::visit(const Function& func, const DataflowNode& n, NodeState& state)
{
  bool visit_fail = false;
  if(state.isInitialized(ra))
  {
    std::vector<Lattice*> lv = state.getLatticeAbove(ra);
    if(lv.empty())
    {
      std::cerr << "ERROR lattice vector == NULL!\n";
      visit_fail = true;
    }
    Lattice* lattice = *(lv.begin());
    if(lattice == NULL)
    {
      std::cerr << "ERROR lattice == NULL\n";
      visit_fail = true;
    }
    RankLattice* rank_lattice = dynamic_cast<RankLattice*>(lattice);

    if(rank_lattice != NULL)
    {
      if(rankAnalysisTestDebugLevel > 1)
        std::cerr << "\n" << rank_lattice->toString();
      failure = failure || false;
      this->pass++;
    }
    else
      visit_fail = true;
  }
  else
    visit_fail = true;

  if(visit_fail)
  {
    this->failure = true;
    this->fail++;
  }
}

int main(int argc, char *argv[])
{
  // Build the AST used by ROSE
  SgProject* project = frontend(argc,argv);
  ROSE_ASSERT (project != NULL);

  std::cerr << "\n## Init Analysis";
  initAnalysis(project);
//  analysisDebugLevel=0;
  Dbg::init("RANK ANALYSIS TEST", ".", "index.html");
//  rankAnalysisDebugLevel = 0;

  std::cerr << "\n## Call Graph Builder";
  CallGraphBuilder cgb(project);
  cgb.buildCallGraph();
  SgIncidenceDirectedGraph* graph = cgb.getGraph();

  std::cerr << "\n## Before Rank Analysis";
  RankAnalysis rankA(project);
  std::cerr << "\n## Before Inter_Procedura_Rank_Analysis initialization";
  ContextInsensitiveInterProceduralDataflow rankInter(&rankA, graph);
  std::cerr << "\n## Before Rank Analysis RUN ...";
  rankInter.runAnalysis();
  std::cerr << "\n## Finished analysis.\n\n";

  EvaluationPass ep(&rankA, cfgNodeFilter);
  UnstructuredPassInterAnalysis upia_ep(ep);
  upia_ep.runAnalysis();

  if(ep.failure)
  {
      std::cerr << "\nTEST FAIL\n";
      std::cerr << ep.fail << " lattice was not defined at certain nodes.\n";
      std::cerr << ep.pass << " Passe!\n";
  }
  else
  {
      std::cerr << "\nTEST PASSED\n";
      std::cerr << ep.pass << " nodes passed successfully!\n";
      std::cerr << ep.fail << " Fails!\n";
  }
  return 0;
}
