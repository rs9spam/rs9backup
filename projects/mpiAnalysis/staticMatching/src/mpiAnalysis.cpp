// Example translator to generate dot files of virtual,
// inter_procedural control flow graphs
// Extended to build an mpi_cfg

#include "rose.h"
#include "mpiCFG.h"
#include <string>
#include <list>
#include <sstream>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <map>
#include <ctype.h>
#include <boost/algorithm/string.hpp>

#include "genericDataflowCommon.h"
#include "VirtualCFGIterator.h"
#include "cfgUtils.h"
#include "CallGraphTraverse.h"
#include "analysisCommon.h"
#include "analysis.h"
#include "dataflow.h"
#include "latticeFull.h"
#include "printAnalysisStates.h"
#include "constantPropagation.h"
#include "liveDeadVarAnalysis.h"
#include "rankAnalysis.h"

using namespace std;

int numFails = 0, numPass = 0;

int main(int argc, char *argv[])
{
  // Build the AST used by ROSE
  SgProject* project = frontend(argc,argv);
  ROSE_ASSERT (project != NULL);

  /////////////////////////////////////////////////////////////////////////////////////////////
  //Perform inter_procedural constant propagation in order to gain additional information
  //src/midend/programAnalysis/gernericDataflow/analysis/analysisCommne.h
  //initializes CFGUtils, builds Call graph as an SgIncidenceDirected Graph
  //SgInterface annotateExpressionwithUniqeName

  initAnalysis(project);
  Dbg::init("Live dead variable analysis Test", ".", "index.html");
//  liveDeadAnalysisDebugLevel = 1;
//  analysisDebugLevel = 1;
  LiveDeadVarsAnalysis ldva(project);
  UnstructuredPassInterDataflow ciipd_ldva(&ldva);
  ciipd_ldva.runAnalysis();

  // prepare call graph
  CallGraphBuilder cgb(project);
  cgb.buildCallGraph();
  SgIncidenceDirectedGraph* graph = cgb.getGraph();

  //use constant propagation within the
  //context insensitive inter-procedural data-flow driver
  ConstantPropagationAnalysis cpA(&ldva);
//  ConstantPropagationAnalysis cpA(NULL);
  ContextInsensitiveInterProceduralDataflow cpInter(&cpA, graph);
  cpInter.runAnalysis();

//  RankAnalysis rankA(project);
////  rankA.setProject(project);
////  ROSE_ASSERT (rankA.setSizeAndRank(project));
////  rankA.setRankAnalysis(&rankA); // don't do that!
//  ContextInsensitiveInterProceduralDataflow rankInter(&rankA, graph);
//  rankInter.runAnalysis();

  /////////////////////////////////////////////////////////////////////////////////////////////
  std::cerr << "\n\n## Going to create MPI_ICFG changes today 18th of September";
  //Find main function
  SgFunctionDeclaration* mainDefDecl = SageInterface::findMain(project);
  ROSE_ASSERT (mainDefDecl != NULL);
  SgFunctionDefinition* mainDef = mainDefDecl->get_definition();
  ROSE_ASSERT (mainDef != NULL);

  //Initializes the mpi_cfg class
  MpiAnalysis::MPICFG mpi_cfg(mainDef, &cpA);
  //Builds the full mpi_cfg and performs all possible pruning steps.
  mpi_cfg.build();
  std::cerr << "\n## Successfully created MPI_ICFG";
  // Dump out the full MPI_CFG
  mpi_cfg.mpicfgToDot();
  std::cerr << "## Dumped out the full MPI_CFG as\n";
  // Call functions to refine the graph.......
  return 0;
}
