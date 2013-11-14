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
}

#endif /* MPIUTILS_H_ */



#if 0

#ifndef MPIUTILS_H_
#define MPIUTILS_H_

#include "dataflow.h"

class MpiUtils
{
public:
  MpiUtils() {};
  //! initializes the mpiUtils module
  //static void initMPIUtil(SgProject* project_arg);

//=======================================================================================
  /**
   * @brief Check if MPI communication function call.
   * @param node SgNode* to be checked if MPI_Communication_Function_Call.
   * @return Returns true if it is any MPI_Send/Recv/Barrier/I_Send/I_Recv/Broadcast.
   */
  static bool isMPIFunctionCall(const SgNode* node) const;

  /**
   *
   */
  static bool isMPIInit(const DataflowNode& n) const;

  /**
   *
   */
  static bool isMPIInit(const SgNode* node) const;

  /**
   *
   */
  static bool isMPICommRank(const SgNode* node) const;

  /**
   *
   */
  static bool isMPICommSize(const SgNode* node) const;

  /**
   * @brief Check if SgFunctionCall node is of type MPI_Recv, MPI_Irecv.
   * @param expr Checks if the SgNode is a valid MPI_Recv node.
   * @return True if expr is MPI_Recv node.
   */
  static bool isMPIRecv(const SgNode* node) const;

  /**
   * @brief Check if SgFunctionCall node is of type MPI Send, MPI_Isend.
   * @param expr Checks if the SgNode is a valid MPI_Send node.
   * @return True if expr is MPI_Send node.
   */
  static bool isMPISend(const SgNode* node) const;

  /**
   *
   */
  static bool isLoopStmt(const DataflowNode& n) const;
#if 0
  /**
   *
   */
  static bool isFalseSuccessor(const CFGNode* this_n,
                               const CFGNode* that_n) const;
#endif
  //! Returns the SgNode of the True successor DataflowNode of n.
  static SgNode* getTrueSuccessor(const DataflowNode& n) const;
  //! Returns the SgNode of the False successor DataflowNode of n.
  static SgNode* getFalseSuccessor(const DataflowNode& n) const;
};

#endif /* MPIUTILS_H_ */
#endif
