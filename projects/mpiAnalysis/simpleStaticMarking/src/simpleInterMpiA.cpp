#include "rose.h"
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
#include "nonDetAnalysis.h"
#include "divAnalysis.h"
#include "printAnalysisStates.h"
#include "liveDeadVarAnalysis.h"

using namespace std;
int numFails = 0, numPass = 0;

int main(int argc, char *argv[])
{
  std::cerr << "========== S T A R T ==========\n";

  // Build the AST used by ROSE
  std::cerr << "*************************************************************\n";
  std::cerr << "**************   Build the AST used by ROSE   ***************\n";
  std::cerr << "*************************************************************\n";
  SgProject* project = frontend(argc,argv);
  ROSE_ASSERT (project != NULL);
  cfgUtils::initCFGUtils(project);

  initAnalysis(project);  // can't be used with Live/Dead Variable Analysis...
  std::cerr << "*************************************************************\n";

  //Call graph get/s built in initAnalysis()....
//  std::cerr << "*************************************************************\n";
//  std::cerr << "*******************   Build Call Graph   ********************\n";
//  std::cerr << "*************************************************************\n";
//  CallGraphBuilder cgb(project);
//  cgb.buildCallGraph();
//  SgIncidenceDirectedGraph* graph = cgb.getGraph();
//  std::cerr << "*************************************************************\n";

//  std::cerr << "*************************************************************\n";
//  std::cerr << "**************   Live/Dead Variable Analysis   **************\n";
//  std::cerr << "*************************************************************\n";
//  initAnalysis(project);
//  Dbg::init("Live dead variable analysis Test", ".", "index.html");
//  liveDeadAnalysisDebugLevel = 1;
//  analysisDebugLevel = 1;
//  LiveDeadVarsAnalysis ldva(project);
//  UnstructuredPassInterDataflow ciipd_ldva(&ldva);
//  ciipd_ldva.runAnalysis();

//  std::cerr << "*************************************************************\n";
//  std::cerr << "*****************   Divisibility Analysis   *****************\n";
//  std::cerr << "*************************************************************\n";
//  DivAnalysis da(&ldva);
//  ContextInsensitiveInterProceduralDataflow ciipd_da(&da, graph);
//  //UnstructuredPassInterDataflow ciipd_da(&da);
//  ciipd_da.runAnalysis();


  std::cerr << "*************************************************************\n";
  std::cerr << "**********   Non determinism Dependency Analysis   **********\n";
  std::cerr << "*************************************************************\n";
  Dbg::enterFunc("nonDetAnalysis");
  analysisDebugLevel = 0;
  MPINonDetAnalysisDebugLevel = 0;
  MPINonDetAnalysis* rda = runMPINonDetAnalysis(getCallGraph());
//  MPINonDetAnalysis* rda = runMPINonDetAnalysis(graph, "");
//  MPINonDetAnalysis* rda = runMPINonDetAnalysis(graph, &ldva, "");
  vector<SgNode*> non_det_nodes = rda->getNonDetNodes();
//  MPINonDetAnalysis* rda = runMPINonDetAnalysis(graph);
  printMPINonDetAnalysisStates("");
  Dbg::exitFunc("nonDetAnalysis");

  std::cerr << "*************************************************************\n";


















  ///////////////////////////////////////////////////////////////////////////////////////////

//  std::cerr << "\n## Going to create MPI_ICFG\n";
//  SgFunctionDeclaration* mainDefDecl = SageInterface::findMain(project);
//  ROSE_ASSERT (mainDefDecl != NULL);
//
//  SgFunctionDefinition* mainDef = mainDefDecl->get_definition();
//  ROSE_ASSERT (mainDef != NULL);
//
//  MpiAnalysis::MPICFG mpi_cfg(mainDef, &cpA);
//  std::cerr << "\n## Successfully created MPI_ICFG\n";
//
//  // Dump out the full MPI_CFG
//  std::cerr << "\n## Going to dump the full MPI_ICFG\n";
//
//  string fileName =
//      StringUtility::stripPathFromFileName(mainDef->get_file_info()->get_filenameString());
//  string dotFileName1 = fileName+"."+ mainDef->get_declaration()->get_name() +".MPIICFG.dot";
//
//  mpi_cfg.cfgToDot(mainDef, dotFileName1);
//  std::cerr << "\n## Dumped out the full MPI_CFG as\n" << dotFileName1 << endl;

  // Call functions to refine the graph.......
  std::cerr << "========== E N D ==========\n";

  return 0;
}
