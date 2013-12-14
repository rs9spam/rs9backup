/*
 * mpiUtils.h
 *
 *  Created on: Nov 1, 2013
 *      Author: ro
 */
#ifndef MPIUTILS_H_
#define MPIUTILS_H_

#include "dataflow.h"

namespace MpiUtils
{
  /**
   * @brief Check if MPI communication function call.
   * @param node SgNode* to be checked if MPI_Communication_Function_Call.
   * @return Returns true if it is any MPI_Send/Recv/Barrier/I_Send/I_Recv/Broadcast.
   */
  bool isMPIFunctionCall(const SgNode* node);

  /**
   * @brief Check if MPI communication function call.
   * @param node SgNode* to be checked if MPI_Communication_Function_Call.
   * @return Returns true if it is any MPI_Send/Recv/Barrier/I_Send/I_Recv/Broadcast.
   */
  bool isMPIFunctionCall(const DataflowNode& n);

  /**
   *
   */
  bool isMPIInit(const DataflowNode& n);

  /**
   *
   */
  bool isMPIInit(const SgNode* node);

  /**
   *
   */
  bool isMPIFinalize(const DataflowNode& n);

  /**
   *
   */
  bool isMPIFinalize(const SgNode* node);

  /**
   *
   */
  bool isMPICommRank(const SgNode* node);

  /**
   *
   */
  bool isMPICommSize(const SgNode* node);

  /**
   * @brief Check if SgFunctionCall node is of type MPI_Barrier.
   * @param node Checks if the SgNode is a valid MPI_Barrier node.
   * @return True if node is MPI_Barrier node.
   */
  bool isMPIBarrier(const SgNode* node);

  /**
   * @brief Check if SgFunctionCall node is of type MPI_Recv, MPI_Irecv.
   * @param node Checks if the SgNode is a valid MPI_Recv node.
   * @return True if node is MPI_Recv node.
   */
  bool isMPIRecv(const SgNode* node);

  /**
   * @brief Check if SgFunctionCall node is of type MPI Send, MPI_Isend.
   * @param node Checks if the SgNode is a valid MPI_Send node.
   * @return True if node is MPI_Send node.
   */
  bool isMPISend(const SgNode* node);

  /**
   *
   */
  bool isLoopStmt(const DataflowNode& n);

  /**
   * @brief Returns the SgNode of the True successor DataflowNode of n.
   */
  SgNode* getTrueSuccessor(const DataflowNode& n);

  /**
   * @brief Returns the SgNode of the False successor DataflowNode of n.
   */
  SgNode* getFalseSuccessor(const DataflowNode& n);

  /**
   *
   */
  SgExpression* getExpAt(const SgExpressionPtrList& argsptr ,unsigned int arg_nr);


//=======================================================================================
  /**
   * DEBRECATED
   * @brief Find DataflowNode in vector.
   */
  DataflowNode* findDataflowNode_(const CFGNode& n, std::vector<DataflowNode> dfvec);

  /**
   * DEBRECATED
   * @brief Find DataflowNode in vector.
   */
  DataflowNode* findDataflowNode_(const SgNode* n, unsigned int idx,
                                 std::vector<DataflowNode> dfvec);

  /**
   * @brief Find DataflowNode in vector.
   */
  bool hasDataflowNode(const CFGNode& n, std::vector<DataflowNode> dfvec);

  /**
   * @brief Find DataflowNode in vector.
   */
  bool hasDataflowNode(const SgNode* n, unsigned int idx, std::vector<DataflowNode> dfvec);


  /**
   *
   */
  DataflowNode getDataflowNode(const CFGNode& n, std::vector<DataflowNode> dfvec);

  /**
   *
   */
  DataflowNode getDataflowNode(/*const*/ SgNode* n, unsigned int idx,
                               std::vector<DataflowNode> dfvec);


}

#endif /* MPIUTILS_H_ */
