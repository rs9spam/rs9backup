#include "rose.h"
#include "constPropAnalysis/constantPropagationAnalysis.h"
#include "liveDeadVarAnalysis.h"
#include "loopAnalysis/loopAnalysis.h"
#include "mpiCFG/mpiCFG.h"

int loopAnalysisTestDebugLevel = 3;

//=======================================================================================
bool cfgNodeFilter (CFGNode cfgn)
{
  SgNode * node = cfgn.getNode();
  assert (node != NULL) ;
  bool ret = true;
  if (isSgInitializedName(node))
    ret = (cfgn == node->cfgForEnd());
  else
    ret = (cfgn.isInteresting());

  if(loopAnalysisTestDebugLevel > 3)
  {
    if(ret)
      std::cerr << "#";
    else
      std::cerr << "_";
  }

  return ret;
}

//=======================================================================================
class EvaluationPass: public UnstructuredPassIntraAnalysis
{
public:
    LoopAnalysis *la;
    int pass, fail;
    bool failure;
    bool (*filter) (CFGNode cfgn);

    EvaluationPass(LoopAnalysis* that_la, bool (*f)(CFGNode))
        : Analysis(f), la(that_la), filter(f)
    {
      pass = 0;
      fail = 0;
      failure = false;
    }

    void visit(const Function& func, const DataflowNode& n, NodeState& state);
};

//=======================================================================================
void EvaluationPass::visit(const Function& func, const DataflowNode& n, NodeState& state)
{
  bool visit_fail = false;
  if(state.isInitialized(la))
  {
    std::vector<Lattice*> lv = state.getLatticeBelow(la);
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
    LoopLattice* loop_lattice = dynamic_cast<LoopLattice*>(lattice);

    if(loop_lattice != NULL)
    {
      if(loopAnalysisTestDebugLevel > 1)
        std::cerr << "\n\n Node: " << n.toStringForDebugging();
        std::cerr << "  " << loop_lattice->toStringForDebugging();
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

//=======================================================================================
int main(int argc, char *argv[])
{
  // Build the AST used by ROSE
  SgProject* project = frontend(argc,argv);
  ROSE_ASSERT (project != NULL);

  std::cerr << "\n## Init Analysis:";
  initAnalysis(project);
//  analysisDebugLevel=0;
  Dbg::init("LOOP ANALYSIS TEST", ".", "index.html");
  loopAnalysisDebugLevel = 0;

  std::cerr << "\n## Call Graph Builder;";
  CallGraphBuilder cgb(project);
  cgb.buildCallGraph();
  SgIncidenceDirectedGraph* graph = cgb.getGraph();

  std::cerr << "\n## Before Loop Analysis:";
  LoopAnalysis loopA(project);
  ContextInsensitiveInterProceduralDataflow loopInter(&loopA, graph);
  loopInter.runAnalysis();

  if(loopAnalysisTestDebugLevel >= 1)
  {
    std::cerr << "\n## Evaluation Pass:";
    EvaluationPass ep(&loopA, cfgNodeFilter);
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
  }

  liveDeadAnalysisDebugLevel = 0;
  analysisDebugLevel = 0;
//  rrankAnalysisDebugLevel = 2;

  std::cerr << "\n## LiveDeadVars Analysis";
  LiveDeadVarsAnalysis ldva(project);
  UnstructuredPassInterDataflow ciipd_ldva(&ldva);
  ciipd_ldva.runAnalysis();

  std::cerr << "\n## Constant Propagation Analysis";
  ConstantPropagationAnalysis cpA(&ldva);
  ContextInsensitiveInterProceduralDataflow cpInter(&cpA, graph);
  cpInter.runAnalysis();

//  std::cerr << "\n## Before Rank Analysis:";
//  RankAnalysis rankA(project);
//  ContextInsensitiveInterProceduralDataflow rankInter(&rankA, graph);
//  rankInter.runAnalysis();
//  const std::vector<DataflowNode> radfn = rankA.getDFNodes();
  RankAnalysis* rankA = NULL;
  CallAnalysis* callA = NULL;

  ///////////////////////////////////////////////////////////////////////////////////////
  std::cerr << "\n## Going to create MPI_ICFG changes today 21th of September";
  MpiAnalysis::MPICFG mpi_cfg((SgNode*) project,
                              (IntraProceduralDataflow*) &cpA,
                              (RankAnalysis*)rankA,
                              (LoopAnalysis*)&loopA,
                              (CallAnalysis*)callA,
                              true);
  mpi_cfg.build();

//  std::cerr << "\n## Successfully created MPI_ICFG";
//  mpi_cfg.setRankInfo(radfn);
//  mpi_cfg.mpicfgToDot();

  std::cerr << "\n## Dumped out the full MPI_CFG Communication Graph as";
  mpi_cfg.mpiCommToDot();
  std::cerr << "\n## Dumped out MPI communication as Dot graph\n";

  return 0;
}
