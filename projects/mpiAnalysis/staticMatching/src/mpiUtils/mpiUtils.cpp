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


