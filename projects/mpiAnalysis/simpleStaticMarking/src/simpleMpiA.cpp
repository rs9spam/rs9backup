// Example translator to generate dot files of virtual,
// interprocedural control flow graphs
// Extended to build an mpi cfg

#include "rose.h"
//#include "mpiCFG.h"
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
//  std::cerr << "1************************************************************\n";
  ROSE_ASSERT (project != NULL);
//  std::cerr << "2************************************************************\n";
  cfgUtils::initCFGUtils(project);
//  std::cerr << "3************************************************************\n";
  initAnalysis(project);  // can't be used with Live/Dead Variable Analysis...
//  std::cerr << "4************************************************************\n";
  SgIncidenceDirectedGraph* graph = getCallGraph();
//  std::cerr << "5************************************************************\n";

//  //Call graph get/s built in initAnalysis()....
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
//  MPINonDetAnalysis* rda = runMPINonDetAnalysis(getCallGraph());
  MPINonDetAnalysis* rda = runMPINonDetAnalysis(graph, "");
//  MPINonDetAnalysis* rda = runMPINonDetAnalysis(graph, &ldva, "");
  vector<SgNode*> non_det_nodes = rda->getNonDetNodes();
//  MPINonDetAnalysis* rda = runMPINonDetAnalysis(graph);
  printMPINonDetAnalysisStates("");
  Dbg::exitFunc("nonDetAnalysis");

//  //prepare nonDetAnalysis
//  MPINonDetAnalysis* nonDetAnalysis = new MPINonDetAnalysis();
//  ROSE_ASSERT( nonDetAnalysis != NULL);
//
//  Rose_STL_Container<SgNode*> nodeList= NodeQuery::querySubTree(project, V_SgFunctionDeclaration);
//
//  Rose_STL_Container<SgNode*>::const_iterator i;
//  for (i = nodeList.begin() ; i != nodeList.end(); ++i)
//  {
//    SgFunctionDeclaration* sgfunction = isSgFunctionDeclaration(*i);
//
//    Function func = CGFunction(sgfunction, graph );
//    if(func.get_definition())
//    {
//      FunctionState* fState = FunctionState::getDefinedFuncState(func);
//      IntraProceduralDataflow *intraDataflow = dynamic_cast<IntraProceduralDataflow *>(nonDetAnalysis);
//      if (intraDataflow->visited.find(func) == intraDataflow->visited.end())
//      {
//        vector<Lattice*>  initLats;
//        vector<NodeFact*> initFacts;
//        std::cerr << "6*************************************************************\n";
//        intraDataflow->genInitState(func, cfgUtils::getFuncStartCFG(func.get_definition()),
//                                    fState->state, initLats, initFacts);
//        //                        intraAnalysis->genInitState(func, cfgUtils::getFuncEndCFG(func.get_definition()),
//        //                            fState->state, initLats, initFacts);
//        std::cerr << "7*************************************************************\n";
//        fState->state.setLattices(nonDetAnalysis, initLats);
//        std::cerr << "8*************************************************************\n";
//        fState->state.setFacts(nonDetAnalysis, initFacts);
//      }
//
//      std::cerr << "9*************************************************************\n";
//      // Run the intra-procedural dataflow analysis on the current function
//      dynamic_cast<IntraProceduralDataflow*>(nonDetAnalysis)->runAnalysis(func, &(fState->state));
//    }
//  }
//







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
