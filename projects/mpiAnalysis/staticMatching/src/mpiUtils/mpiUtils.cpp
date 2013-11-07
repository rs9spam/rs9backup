/*
 * mpiUtils.cpp
 *
 *  Created on: Nov 1, 2013
 *      Author: ro
 */

#include "mpiUtils.h"

//=======================================================================================
bool MpiUtils::isMPIInit(const DataflowNode& n)
{
  std::cerr << "DEBUG";
  return isMPIInit(n.getNode());
}

//=======================================================================================
bool MpiUtils::isMPIInit(const SgNode* node)
{
  string name;
  if( isSgFunctionRefExp(node)) {
    name = isSgFunctionRefExp(node)->get_symbol()->get_name();
    if(name == "MPI_Init")
      return true;
  }
  return false;
}

//=======================================================================================
bool MpiUtils::isMPICommRank(const SgNode* node)
{
  string name;
  if( isSgFunctionRefExp(node)) {
    name = isSgFunctionRefExp(node)->get_symbol()->get_name();
    if(name == "MPI_Comm_rank")
      return true;
  }
  return false;
}

//=======================================================================================
bool MpiUtils::isMPICommSize(const SgNode* node)
{
  string name;
  if( isSgFunctionRefExp(node)) {
    name = isSgFunctionRefExp(node)->get_symbol()->get_name();
    if(name == "MPI_Comm_size")
      return true;
  }
  return false;
}

//=======================================================================================
bool MpiUtils::isLoopStmt(const DataflowNode& n)
{
  SgNode* node = n.getNode();
  if(isSgForStatement(node))
    return true;
  return false;
}

#if 0
//=======================================================================================
bool MpiUtils::isFalseSuccessor(CFGNode* this_n, CFGNode* that_n)
{
  //TODO find out how to do that....
  //First check if it is a forking node.
  //SgStatement* else_body = (that_n->getNode())->get_else_body();
  return false;
}
#endif

//=======================================================================================
SgNode* MpiUtils::getTrueSuccessor(const DataflowNode& n)
{
  std::vector<DataflowEdge> out_edges = n.outEdges();
  std::vector<DataflowEdge>::iterator it = out_edges.begin();
  SgNode* true_target = NULL;
  for(; it != out_edges.end(); ++it)
    if(it->condition() == VirtualCFG::eckTrue)
    {
      true_target = it->target().getNode();
      break;
    }
  return true_target;
}

//=======================================================================================
SgNode* MpiUtils::getFalseSuccessor(const DataflowNode& n)
{
  std::vector<DataflowEdge> out_edges = n.outEdges();
  std::vector<DataflowEdge>::iterator it = out_edges.begin();
  SgNode* false_target = NULL;
  for(; it != out_edges.end(); ++it)
    if(it->condition() == VirtualCFG::eckFalse)
    {
      false_target = it->target().getNode();
      break;
    }
  return false_target;
}

#if 0
/*
 * mpiUtils.cpp
 *
 *  Created on: Nov 1, 2013
 *      Author: ro
 */

#include "mpiUtils.h"

namespace mpiUtils
{
SgProject* project;

//=======================================================================================
// =true if initMPIUtils has been called and =false otherwise
static bool MPIinitialized = false;

// initializes the mpiUtils module
void initMPIUtils(SgProject* project_arg)
{
  // if the cfgUtils has not been initialized
  if(project == NULL)
  {
    project = project_arg;
  }
  else
    ROSE_ASSERT(project == project_arg);

  MPIinitialized = true;
}

//=======================================================================================
SgProject* getProject()
{
  return project;
}

//=======================================================================================
bool isMPIInit(const DataflowNode& n)
{
  return isMPIInit(n.getNode());
}

//=======================================================================================
bool isMPIInit(const SgNode* node)
{
  string name;
  if( isSgFunctionRefExp(node)) {
    name = isSgFunctionRefExp(node)->get_symbol()->get_name();
    if(name == "MPI_Init")
      return true;
  }
  return false;
}

//=======================================================================================
bool isMPICommRank(const SgNode* node)
{
  string name;
  if( isSgFunctionRefExp(node)) {
    name = isSgFunctionRefExp(node)->get_symbol()->get_name();
    if(name == "MPI_Comm_rank")
      return true;
  }
  return false;
}

//=======================================================================================
bool isMPICommSize(const SgNode* node)
{
  string name;
  if( isSgFunctionRefExp(node)) {
    name = isSgFunctionRefExp(node)->get_symbol()->get_name();
    if(name == "MPI_Comm_size")
      return true;
  }
  return false;
}

} /*end namespace mpiUtils*/

#endif


