#include "rose.h"
#include "rankAnalysis/rankAnalysis.h"
#include "rankAnalysis/FinestPSetGranularity.h"
#include "rankMpiCallList/CallAnalysis.h"

int rankMpiCallListTestDebugLevel = 2;

//=======================================================================================
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
//  loopAnalysisDebugLevel = 0;

  std::cerr << "\n## Call Graph Builder";
  CallGraphBuilder cgb(project);
  cgb.buildCallGraph();
  SgIncidenceDirectedGraph* graph = cgb.getGraph();

  std::cerr << "\n## Rank Analysis";
  RankAnalysis rankA(project);
  ContextInsensitiveInterProceduralDataflow rankInter(&rankA, graph);
  rankInter.runAnalysis();

  std::cerr << "\n## Finest Process Set Granularity Analysis";
  FinestPSetGranularity fpsg(&rankA);
  UnstructuredPassInterAnalysis upia_fpsg(fpsg);
  upia_fpsg.runAnalysis();
  fpsg.buildPSets();
  std::cerr << fpsg.toStr() << std::endl;

  std::cerr << "\n## Loop Analysis";
  LoopAnalysis loopA(project);
  ContextInsensitiveInterProceduralDataflow loopInter(&loopA, graph);
  loopInter.runAnalysis();

  std::cerr << "\n## Rank Call List Analysis";
  CallAnalysis callA = CallAnalysis(project, &rankA, &fpsg, &loopA);
  ContextInsensitiveInterProceduralDataflow callInter(&callA, graph);
  callInter.runAnalysis();

  return 0;
}
