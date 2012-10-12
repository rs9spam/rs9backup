// Example translator to generate dot files of virtual, interprocedural control flow graphs
// Extended to build an mpi cfg
#include "rose.h"
//#include "staticCFG.h"
#include "mpiCFG.h"
#include <string>

using namespace std;

int main(int argc, char *argv[])
{
  // Build the AST used by ROSE
  SgProject* proj = frontend(argc,argv);
  ROSE_ASSERT (proj != NULL);

//  SgFunctionDeclaration* mainDefDecl = SageInterface::findMain(dynamic_cast<SgNode*>(proj));
  SgFunctionDeclaration* mainDefDecl = SageInterface::findMain(proj);
  ROSE_ASSERT (mainDefDecl != NULL);

  SgFunctionDefinition* mainDef = mainDefDecl->get_definition();
  ROSE_ASSERT (mainDef != NULL);

  string fileName= StringUtility::stripPathFromFileName(mainDef->get_file_info()->get_filenameString());
  string dotFileName1=fileName+"."+ mainDef->get_declaration()->get_name() +".MPIICFG.dot";

  MpiAnalysis::MPICFG mpi_cfg(mainDef);

  // Dump out the full MPI_CFG
  mpi_cfg.cfgToDot(mainDef, dotFileName1);

  // Call functions to refine the graph.......

  return 0;
}
