/*
 * MpiCommunication.h
 *
 *  Created on: Nov 18, 2013
 *      Author: ro
 */

#ifndef MPICOMMUNICATION_H_
#define MPICOMMUNICATION_H_

#include "mpiUtils/mpiUtils.h"
#include "latticeFull.h"
#include <map>
#include <vector>
#include <string>

#define MPI_NUM_SEND_EXP 6
#define MPI_NUM_RECV_EXP 7
#define MPI_VAR_ARG 1
#define MPI_SIZE_ARG 2
#define MPI_TYPE_ARG 3
#define MPI_SOURCE_ARG 4
#define MPI_TAG_ARG 5
#define MPI_COMM_WORLD_ARG 6
#define MPI_STATUS_ARG 7

class ConstantPropagationAnalysis;
class LoopAnalysis;
class RankAnalysis;
class CallAnalysis;

namespace MpiAnalysis
{
using VirtualCFG::CFGNode;

//###################################################################################
//CFG is from StaticCFG
class MpiCommunication
{
//===================================================================================
protected:
  /// Pointer to the whole project.
  SgNode* mpi_project_;

//  const std::map<CFGNode, SgGraphNode*> all_nodes_;
  std::vector<CFGNode> all_nodes_;

#if 0
  /**
   * List of all DataflowNodes since the conversion from CFG to DataflowNodes in order
   * to access NodeStates is not possible.
   */
  std::vector<DataflowNode> cpa_nodes_;
  std::vector<DataflowNode> ra_nodes_;
  std::vector<DataflowNode> la_nodes_;
  std::vector<DataflowNode> cla_nodes_;
#endif
  ConstantPropagationAnalysis* const_prop_;
//  IntraProceduralDataflow* const_prop_;
  RankAnalysis* rank_analysis_;
  LoopAnalysis* loop_analysis_;
  CallAnalysis* call_analysis_;

  /**
   * @brief All SgGraphNodes which perform some sort of MPI_Send.
   */
  std::vector<CFGNode> mpi_send_nodes_;

  /**
   * @brief All SgGraphNodes which perform some sort of MPI_Recv.
   */
  std::vector<CFGNode> mpi_recv_nodes_;

  /**
   * @brief All MPI_Barrier nodes.
   */
  std::vector<CFGNode> mpi_barrier_nodes_;

  /**
   * @brief ALL possible MPI connections between MPI_Send and MPI_Recv
   */
  std::multimap<CFGNode, CFGNode> mpi_connections_;

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
public:
  /**
   *
   */
  MpiCommunication() {};

  /**
   *
   */
  MpiCommunication(std::vector<CFGNode> an)
      : all_nodes_(an) {};

  /**
   * @brief Constructor with all analyses.
   */
  MpiCommunication(std::vector<CFGNode> an,
//                   IntraProceduralDataflow* cp,
                   ConstantPropagationAnalysis* cp,
                   RankAnalysis* ra,
                   LoopAnalysis* la,
                   CallAnalysis* ca) :
               all_nodes_(an), const_prop_(cp),
               rank_analysis_(ra), loop_analysis_(la), call_analysis_(ca)
  {};

#if 0
  void setDataflowNodesCPA(std::vector<DataflowNode> cpa_nodes)
    { cpa_nodes_.assign(cpa_nodes.begin(), cpa_nodes.end()); };
  void setDataflowNodesRA(std::vector<DataflowNode> ra_nodes)
    {  ra_nodes_.assign(ra_nodes.begin(), ra_nodes.end()); };
  void setDataflowNodesLA(std::vector<DataflowNode> la_nodes)
    {  la_nodes_.assign(la_nodes.begin(), la_nodes.end()); };
  void setDataflowNodesCA(std::vector<DataflowNode> cla_nodes)
    {  cla_nodes_.assign(cla_nodes.begin(), cla_nodes.end()); };
#endif

  /**
   * @brief Fills a Map with all possible match sets.
   */
  bool buildFullMPIMatchSet();

  /**
   * @brief Refines mpi_communications_ whit constant not matching function parameters.
   *
   * Removes communication Edges with non matching constant data types.
   * Removes communication Edges with non matching constant data size.
   * Removes communication Edges with non matching constant communication tag.
   * Removes communication Edges with non matching constant communication world.
   */
  void refineConstantMatch();

//===================================================================================
protected:
  /**
   * @brief Find all MPI_Send function calls and insert the pairs into mpiSendNodes.
   */
  void determineMPISend();

  /**
   * @brief Find all MPI_Recv function calls and insert the pairs into mpiRecvNodes.
   */
  void determineMPIRecv();

  /**
   * @brief Compares the data type of the MPI_Send and MPI_Recv node.
   * @return Returns false only if constant Type Arguments do not match.
   */
  bool constTypeMatch(const CFGNode& send_node,
                      const CFGNode& recv_node) const;

  /**
   * @brief Compares the data type of the MPI_Send and MPI_Recv node.
   * @return False only if constant Size Arguments are not valid for a match.
   */
  bool constSizeMatch(const CFGNode& send_node,
                      const CFGNode& recv_node) const;

  /**
   * @brief Compares the data type of the MPI_Send and MPI_Recv node.
   * @return False only if constant Tag Arguments do not match.
   */
  bool constTagMatch(const CFGNode& send_node,
                     const CFGNode& recv_node) const;

  /**
   * @brief Compares the data type of the MPI_Send and MPI_Recv node.
   * @return False only if constant MPICommWorld Arguments do not match.
   */
  bool constCommWorldMatch(const CFGNode& send_node,
                           const CFGNode& recv_node) const;

  /**
   * @brief
   */
  bool hasConstValue(/*const*/ SgNode* node, int idx = 0) const;

  /**
   * @brief
   */
  int getConstPropValue(/*const*/ SgNode* node, int idx = 0) const;

public:
  /**
   * @brief Returns the MPI connection std::multimap.
   * @return
   *
   * This const getter function returns a std::multimap<CFGNode, CFGNode>
   * which represents the MPI connections.
   */
  std::multimap<CFGNode, CFGNode> getMpiConnections() const
    {return this->mpi_connections_;};

//=======================================================================================
  //RANK RELATED FUNCTIONS
#if 0
  /**
   * @brief Sets the RankAnalysis pointer.
   */
  void setRankInfo(RankAnalysis* ra) {rank_analysis_ = (IntraProceduralDataflow*)ra;};

  /**
   * @brief
   */
  bool hasRankInfo(const SgGraphNode* node) const;
#endif
//=======================================================================================
  //LOOP RELATED FUNCTIONS


//=======================================================================================
//              DEBUG OUTPUT -- PRINT FUNCTIONS
//=======================================================================================
  /**
   * @brief Output the possible communications to a Dot graph.
   */
  void mpiCommToDot(const std::string& file_name) const;

protected:
  /**
   * @brief Generates general Dot file information.
   */
  void mpiPrintDotHeader(std::ostream& o) const;

  /**
   * @brief
   */
  void mpiPrintNodes(std::ostream& o) const;

  /**
   *
   */
  void mpiPrintEdges(std::ostream& o) const;

  /**
   *
   */
  void mpiPrintNode(std::ostream& o, const VirtualCFG::CFGNode& node) const;

  /**
   *
   */
  void mpiPrintEdge(std::ostream& o,
                    const VirtualCFG::CFGNode& x,
                    const VirtualCFG::CFGNode& y) const;

};

}
#endif /* MPICOMMUNICATION_H_ */
