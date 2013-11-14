/*
 * mpiUtils.cpp
 *
 *  Created on: Nov 1, 2013
 *      Author: stoero
 */

#include "mpiUtils.h"

namespace MpiUtils
{

//=======================================================================================
bool isMPIFunctionCall(const SgNode* node)
{
  return    MpiUtils::isMPISend(node)
         || MpiUtils::isMPIRecv(node)
         || MpiUtils::isMPIBarrier(node);
}

//=======================================================================================
bool isMPIFunctionCall(const DataflowNode& n)
{
  return isMPIFunctionCall(n.getNode());
}

//=======================================================================================
bool isMPIInit(const DataflowNode& n)
{
  //TODO: Remove
  std::cerr << "DEBUG";
  return MpiUtils::isMPIInit(n.getNode());
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

//===================================================================================
bool isMPIBarrier(const SgNode* node)
{
  string name;
  if( isSgFunctionCallExp(node))
    if( isSgFunctionRefExp(isSgFunctionCallExp(node)->get_function()))
    {
      name = isSgFunctionRefExp(isSgFunctionCallExp(node)->get_function())
                                                         ->get_symbol()->get_name();
      if(name == "MPI_Barrier")
        return true;
    }
  return false;
}

//===================================================================================
bool isMPIRecv(const SgNode* node)
{
  string name;
  if( isSgFunctionCallExp(node))
    if( isSgFunctionRefExp(isSgFunctionCallExp(node)->get_function()))
    {
      name = isSgFunctionRefExp(isSgFunctionCallExp(node)->get_function())
                                                         ->get_symbol()->get_name();
      if(name == "MPI_Recv" || name == "MPI_Irecv")
        return true;
    }
  return false;
}

//===================================================================================
bool isMPISend(const SgNode* node)
{
  string name;
  if( isSgFunctionCallExp(node))
    if( isSgFunctionRefExp(isSgFunctionCallExp(node)->get_function()))
    {
      name = isSgFunctionRefExp(isSgFunctionCallExp(node)
                                      ->get_function())->get_symbol()->get_name();
      if(name == "MPI_Send" || name == "MPI_Isend")
        return true;
    }
  return false;
}

//=======================================================================================
bool isLoopStmt(const DataflowNode& n)
{
  SgNode* node = n.getNode();
  if(isSgForStatement(node))
    return true;
  return false;
}

//=======================================================================================
SgNode* getTrueSuccessor(const DataflowNode& n)
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
SgNode* getFalseSuccessor(const DataflowNode& n)
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

} /*end namespcae mpiUtils*/
