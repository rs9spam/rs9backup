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
  return MpiUtils::isMPIInit(n.getNode());
}

//=======================================================================================
bool isMPIInit(const SgNode* node)
{
  if( isSgFunctionRefExp(node))
    if((isSgFunctionRefExp(node)->get_symbol()->get_name()) == "MPI_Init")
      return true;
  return false;
}

//=======================================================================================
bool isMPIFinalize(const DataflowNode& n)
{
  return MpiUtils::isMPIFinalize(n.getNode());
}

//=======================================================================================
bool isMPIFinalize(const SgNode* node)
{
  if( isSgFunctionRefExp(node))
    if((isSgFunctionRefExp(node)->get_symbol()->get_name()) == "MPI_Finalize")
      return true;
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

//===================================================================================
SgExpression* getExpAt(const SgExpressionPtrList& argsptr ,unsigned int arg_nr)
{
  if(argsptr.size() >= arg_nr)
    return argsptr[arg_nr-1];
  ROSE_ASSERT(!"\nMpi function SgExpressionPtrList doesn't contain argument.");
  return NULL;
}

#if 0
//===================================================================================
SgExpression* getExpAt(const SgExpressionPtrList& argsptr ,unsigned int arg_nr)
{
  SgExpressionPtrList::iterator exp_iter;

  if(argsptr.size() >= arg_nr)
  {
    exp_iter = argsptr.begin();

    for(unsigned int i=1; i < arg_nr; i++)
      exp_iter++;

    SgExpression* exp = (*exp_iter);
    return exp;
  }
  return NULL;
}
#endif


//===================================================================================
//DEBRECATED
DataflowNode* findDataflowNode_(const CFGNode& n, std::vector<DataflowNode> dfvec)
{
  return MpiUtils::findDataflowNode_(n.getNode(), n.getIndex(), dfvec);
}

//===================================================================================
//DEBRECATED
DataflowNode* findDataflowNode_(const SgNode* n, unsigned int idx,
                               std::vector<DataflowNode> dfvec)
{
  std::vector<DataflowNode>::iterator it;
  for(it = dfvec.begin(); it != dfvec.end(); ++it)
    if(n == it->getNode() && idx == it->getIndex())
      return &(*it);
  return NULL;
}

//===================================================================================
bool hasDataflowNode(const CFGNode& n, std::vector<DataflowNode> dfvec)
{
  return MpiUtils::hasDataflowNode(n.getNode(), n.getIndex(), dfvec);
}

//===================================================================================
bool hasDataflowNode(const SgNode* n, unsigned int idx,
                     std::vector<DataflowNode> dfvec)
{
  std::vector<DataflowNode>::iterator it;
  for(it = dfvec.begin(); it != dfvec.end(); ++it)
    if(n == it->getNode() && idx == it->getIndex())
      return true;
  return false;
}

//===================================================================================
DataflowNode getDataflowNode(const CFGNode& n, std::vector<DataflowNode> dfvec)
{
  return MpiUtils::getDataflowNode(n.getNode(), n.getIndex(), dfvec);
}

//===================================================================================
DataflowNode getDataflowNode(/*const*/ SgNode* n, unsigned int idx,
                     std::vector<DataflowNode> dfvec)
{
  CFGNode cfg = CFGNode(n, idx);
  DataflowNode dfn = DataflowNode(cfg, defaultFilter);

  std::vector<DataflowNode>::iterator it;
  for(it = dfvec.begin(); it != dfvec.end(); ++it)
    if(n == it->getNode() && idx == it->getIndex())
      dfn = *it;
  return dfn;
}

} /*end namespcae mpiUtils*/
