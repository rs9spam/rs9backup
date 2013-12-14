// Example translator to generate dot files of virtual,
// inter_procedural control flow graphs
// Extended to build an mpi_cfg

#define CONSTANTPROPANALYSIS 1
#define CALLLISTANALYSIS 1

#if CALLLISTANALYSIS
#define LOOPANALYSIS 1
#define RANKANALYSIS 1
#else
#define RANKANALYSIS 1
#define LOOPANALYSIS 1
#endif

#include "rose.h"
#include "mpiCFG/mpiCFG.h"
#include "constPropAnalysis/constantPropagationAnalysis.h"
#include "liveDeadVarAnalysis.h"
#include "rankAnalysis/rankAnalysis.h"
#include "loopAnalysis/loopAnalysis.h"
#include "rankAnalysis/FinestPSetGranularity.h"
#include "rankMpiCallList/CallAnalysis.h"

int main(int argc, char *argv[])
{
  // Build the AST used by ROSE
  SgProject* project = frontend(argc,argv);
  ROSE_ASSERT (project != NULL);

//  TimingPerformance timer ("ROSE:");

  ///////////////////////////////////////////////////////////////////////////////////////
  //Perform inter_procedural constant propagation in order to gain additional information
  //src/midend/programAnalysis/gernericDataflow/analysis/analysisCommne.h
  //initializes CFGUtils, builds Call graph as an SgIncidenceDirected Graph
  //SgInterface annotateExpressionwithUniqeName

  std::cerr << "\n## Init Analysis";
  initAnalysis(project);
  Dbg::init("Live dead variable analysis Test", ".", "index.html");

  analysisDebugLevel = 0;

  liveDeadAnalysisDebugLevel = 0;
  rrankAnalysisDebugLevel = 0;
  loopAnalysisDebugLevel = 0;
  mpiCallAnalysisDebugLevel = 0;

//  MpiAnalysis::MpiCFGDebugLevel = 0;

  std::cerr << "\n## Call Graph Builder";
  // prepare call graph
  CallGraphBuilder cgb(project);
  cgb.buildCallGraph();
  SgIncidenceDirectedGraph* graph = cgb.getGraph();


//#######################################################################################
#if CONSTANTPROPANALYSIS
  std::cerr << "\n## LiveDeadVars Analysis";
  LiveDeadVarsAnalysis ldva(project);
  UnstructuredPassInterDataflow ciipd_ldva(&ldva);
  ciipd_ldva.runAnalysis();
  std::cerr << "\n## Constant Propagation Analysis";
  ConstantPropagationAnalysis cpA(&ldva);
  ContextInsensitiveInterProceduralDataflow cpInter(&cpA, graph);
  cpInter.runAnalysis();
#endif
//#######################################################################################
#if RANKANALYSIS
  std::cerr << "\n## Before Rank Analysis";
  RankAnalysis rankA(project);
  std::cerr << "\n## Before Rank Analysis Inter initialisation";
  ContextInsensitiveInterProceduralDataflow rankInter(&rankA, graph);
  std::cerr << "\n## Before Rank Analysis RUN ...";
  rankInter.runAnalysis();
#endif
//#######################################################################################
#if LOOPANALYSIS
  std::cerr << "\n## Before Loop Analysis";
  LoopAnalysis loopA(project);
  ContextInsensitiveInterProceduralDataflow loopInter(&loopA, graph);
  loopInter.runAnalysis();
#endif
//#######################################################################################
#if CALLLISTANALYSIS
  std::cerr << "\n## Finest Process Set Granularity Analysis";
  FinestPSetGranularity fpsg(&rankA);
  UnstructuredPassInterAnalysis upia_fpsg(fpsg);
  upia_fpsg.runAnalysis();
  fpsg.buildPSets();
  std::cerr << "\n## " << fpsg.toStr();

  std::cerr << "\n## Rank Call List Analysis";
  CallAnalysis callA = CallAnalysis(project, &rankA, &fpsg, &loopA);
  ContextInsensitiveInterProceduralDataflow callInter(&callA, graph);
  callInter.runAnalysis();
  std::cerr << callA.getPSetCallList().toStringForDebugging();
#endif
//#######################################################################################


#if 0
  ///////////////////////////////////////////////////////////////////////////////////////
  std::cerr << "\n## Going to create MPI_ICFG changes today 4th of December";

  MpiAnalysis::MPICFG mpi_cfg(project,
#if CONSTANTPROPANALYSIS
                              &cpA,
#else
                              NULL,
#endif
#if RANKANALYSIS
                              &rankA,
#else
                              NULL,
#endif
#if LOOPANALYSIS
                              &loopA,
#else
                              NULL,
#endif
#if CALLLISTANALYSIS
                              &callA,
#else
                              NULL,
#endif
                              true);

  ///////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////
  //Builds the full mpi_cfg and performs all possible pruning steps.
  mpi_cfg.build();
  std::cerr << "\n## Successfully created MPI_ICFG";
  // Dump out the full MPI_CFG
  mpi_cfg.mpicfgToDot();
  std::cerr << "\n## Dumped out the full MPI_CFG as";
  mpi_cfg.mpiCommToDot();
  std::cerr << "\n## Dumped out MPI communication as Dot graph";
//  mpi_cfg.callListToDot();
  std::cerr << "\n## Dumped out MPI callListToDot\n";
  // Call functions to refine the graph.......
//  TimingPerformance timer ("ROSE: end");
#endif
  std::cerr << "\n";
  return 0;
}
