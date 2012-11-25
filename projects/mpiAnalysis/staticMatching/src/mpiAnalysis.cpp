// Example translator to generate dot files of virtual,
// interprocedural control flow graphs
// Extended to build an mpi cfg

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
  /////////////////////////////////////////////////////////////////////////////////////////////
  //Performe Interprocedural constant propagation in order to gain additional information
  initAnalysis(project);
  Dbg::init("Live dead variable analysis Test", ".", "index.html");
  liveDeadAnalysisDebugLevel = 1;
  analysisDebugLevel = 1;
  LiveDeadVarsAnalysis ldva(project);
  UnstructuredPassInterDataflow ciipd_ldva(&ldva);
  ciipd_ldva.runAnalysis();

  // prepare call graph
  CallGraphBuilder cgb(project);
  cgb.buildCallGraph();
  SgIncidenceDirectedGraph* graph = cgb.getGraph();

  //use constant propagation within the context insensitive interprocedural dataflow driver
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

  ///////////////////////////////////////////////////////////////////////////////////////////

  std::cerr << "\n## Going to create MPI_ICFG\n";
  SgFunctionDeclaration* mainDefDecl = SageInterface::findMain(project);
  ROSE_ASSERT (mainDefDecl != NULL);

  SgFunctionDefinition* mainDef = mainDefDecl->get_definition();
  ROSE_ASSERT (mainDef != NULL);

  MpiAnalysis::MPICFG mpi_cfg(mainDef, &cpA);
  std::cerr << "\n## Successfully created MPI_ICFG\n";

  // Dump out the full MPI_CFG
  std::cerr << "\n## Going to dump the full MPI_ICFG\n";

  string fileName =
      StringUtility::stripPathFromFileName(mainDef->get_file_info()->get_filenameString());
  string dotFileName1 = fileName+"."+ mainDef->get_declaration()->get_name() +".MPIICFG.dot";

  mpi_cfg.cfgToDot(mainDef, dotFileName1);
  std::cerr << "\n## Dumped out the full MPI_CFG as\n" << dotFileName1 << endl;

  // Call functions to refine the graph.......

  return 0;
}
