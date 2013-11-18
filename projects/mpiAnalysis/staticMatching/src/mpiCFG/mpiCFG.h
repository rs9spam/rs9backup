#ifndef MPI_CFG_H
#define MPI_CFG_H

#include "staticCFG.h"
//#include "CallGraph.h"
//#include "dataflow.h"
#include "mpiUtils/mpiUtils.h"
#include "latticeFull.h"

#if 0
#include "liveDeadVarAnalysis.h"
#include "constPropAnalysis/constantPropagationAnalysis.h"
#include "rankAnalysis/rankAnalysis.h"
#endif /*if 0*/
//class SgIncidenceDirectedGraph;
//class SgGraphNode;
//class SgDirectedGraphEdge;
class LoopAnalysis;
class RankAnalysis;
class CallAnalysis;


#define MPI_NUM_SEND_EXP 6
#define MPI_NUM_RECV_EXP 7
#define MPI_VAR_ARG 1l
#define MPI_SIZE_ARG 2
#define MPI_TYPE_ARG 3
#define MPI_SOURCE_ARG 4
#define MPI_TAG_ARG 5
#define MPI_COMM_WORLD_ARG 6
#define MPI_STATUS_ARG 7

namespace MpiAnalysis
{
//using namespace StaticMPICFG;
//using VirtualMPICFG::CFGNode;
//using VirtualMPICFG::CFGEdge;

using namespace StaticCFG;
using VirtualCFG::CFGNode;
using VirtualCFG::CFGEdge;

typedef std::pair<VirtualCFG::CFGNode, SgGraphNode*> pair_n;
typedef std::pair<VirtualCFG::CFGNode,VirtualCFG::CFGNode> pair_cfg_node;
typedef std::multimap<VirtualCFG::CFGNode, VirtualCFG::CFGNode>::iterator Conn_It;

//###################################################################################
//CFG is from StaticCFG
class MPICFG : public CFG
{
//-----------------------------------------------------------------------------------
private:
  //TODO: make constant whatever possible!
  //TODO: make rankAnalysis and loopAnalysis depended on weather they are set or NULL.


//===================================================================================
protected:
  //Constant Propagation Element
  //Required for constant propagation pruning

  /// Pointer to the whole project.
  SgNode* mpi_project_;

  /// Pointer to Constant Propagation Analysis information.
  IntraProceduralDataflow* const_prop_;
  /// Pointer to Rank Analysis information.
//  RankAnalysis* rank_analysis_;
  IntraProceduralDataflow* rank_analysis_;
  /// Pointer to Loop Analysis information.
//  LoopAnalysis* loop_analysis_;
  IntraProceduralDataflow* loop_analysis_;
  /// Pointer to Call Analysis class.
//  CallAnalysis* call_analysis_;
  IntraProceduralDataflow* call_analysis_;

  //TODO: This is just an auxiliary nodes vectore since I can't access the real nodes.
  /**
   * @brief Auxiliary vector with DataflowNodes to avoid creating new DataFlowNodes
   *        from SgGraph nodes which result in a Segmentation fault.
   */
  std::vector<DataflowNode> ra_nodes_;

  /**
   * @brief All SgGraphNodes which perform some sort of MPI_Send.
   */
  std::map<CFGNode, SgGraphNode*> mpi_send_nodes_;

  /**
   * @brief All SgGraphNodes which perform some sort of MPI_Recv.
   */
  std::map<CFGNode, SgGraphNode*> mpi_recv_nodes_;

  /**
   * @brief All MPI_Barrier nodes.
   */
  std::map<CFGNode, SgGraphNode*> mpi_barrier_nodes_;

  /**
   * @brief ALL possible MPI connections between MPI_Send and MPI_Recv
   */
  std::multimap<CFGNode, CFGNode> mpi_connections_;

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
public:
  MPICFG() : CFG() {}

  /**
   * @brief
   * @param node
   * @param const_prop
   * @param rank_analysis
   * @param loop_analysis
   * @param call_analysis
   * Valid node types are SgProject, SgStatement, SgExpression, SgInitializedName
   */
  MPICFG(SgNode* node,
         IntraProceduralDataflow* const_prop,
         RankAnalysis* rank_analysis,
         LoopAnalysis* loop_analysis,
         CallAnalysis* call_analysis,
         bool is_filtered = false)
    : CFG() { graph_ = NULL;
              mpi_project_ = NULL;
              is_filtered_ = is_filtered;
              start_ = node;
              const_prop_ = const_prop;
              rank_analysis_ = (IntraProceduralDataflow*)rank_analysis;
              loop_analysis_ = (IntraProceduralDataflow*)loop_analysis;
              call_analysis_ = (IntraProceduralDataflow*)call_analysis;
    }

  //Entry to build the full MPIICFG
  //TODO: add the pruning option parameters to the build function
  /**
   * @brief builds the MPI_ICFG and performs all pruning possible depending on the
   *        analyzes set.
   */
  void build();

//===================================================================================
protected:
  /**
   * @brief Big picture MPIICFG construction function calling fine grain construction.
   */
  void buildMPIICFG();

  /**
   * @brief Build CFG according to the 'is_filtered_' flag.
   */
  virtual void buildCFG();

  /**
   * @brief Build CFG for debugging.
   */
  virtual void buildFullCFG();

  /**
   * @brief Build nice CFG.
   */
  virtual void buildFilteredCFG();

#if 0
  /**
   * @brief
   * @param n
   * @param all_nodes
   * @param expolred
   * @param classHierarchy
   */
  virtual void buildCFG(CFGNode n,
                std::map<CFGNode, SgGraphNode*>& all_nodes,
                std::set<CFGNode>& explored,
                ClassHierarchyWrapper* classHierarchy);

  /**
   * @brief
   */
  void addEdge(CFGNode from, CFGNode to, std::vector<CFGEdge>& result);
#endif

  /**
   * @brief Find all MPI_Send function calls and insert the pairs into mpiSendNodes.
   */
  void buildMPISend();

  /**
   * @brief Find all MPI_Recv function calls and insert the pairs into mpiRecvNodes.
   */
  void buildMPIRecv();

  /**
   * @brief Fills a Map with all possible match sets.
   */
  bool buildFullMPIMatchSet();

  /**
   * @brief Add all possible MPI Edges to MPI_ICFG.
   */
  void addMPIEdgestoICFG();

  /**
   * @brief Remove the SgDirectedGraphEdge from the MPI_ICFG.
   * @param edge Edge to be removed from the MPI_ICFG.
   */
  void removeMPIEdge(SgDirectedGraphEdge* edge);










  /**
   * @brief Refines mpi_communications_ whit constant not matching function parameters.
   *
   * Removes communication Edges with non matching constant data types.
   * Removes communication Edges with non matching constant data size.
   * Removes communication Edges with non matching constant communication tag.
   * Removes communication Edges with non matching constant communication world.
   */
  void refineConstantMatch();

  /**
   * @brief Compares the data type of the MPI_Send and MPI_Recv node.
   * @return Returns false only if constant Type Arguments do not match.
   */
  bool constTypeMatch(SgGraphNode* send_node, SgGraphNode* recv_node);

  /**
   * @brief Compares the data type of the MPI_Send and MPI_Recv node.
   * @return False only if constant Size Arguments are not valid for a match.
   */
  bool constSizeMatch(SgGraphNode* send_node, SgGraphNode* recv_node);

  /**
   * @brief Compares the data type of the MPI_Send and MPI_Recv node.
   * @return False only if constant Tag Arguments do not match.
   */
  bool constTagMatch(SgGraphNode* send_node, SgGraphNode* recv_node);

  /**
   * @brief Compares the data type of the MPI_Send and MPI_Recv node.
   * @return False only if constant MPICommWorld Arguments do not match.
   */
  bool constCommWorldMatch(SgGraphNode* send_node, SgGraphNode* recv_node);

  /**
   * @brief
   */
  bool hasConstValue(SgNode* node);

  /**
   * @brief
   */
  int getConstPropValue(SgNode* node);

  /**
   * @brief Returns the corresponding SGNode from SgGraphNode
   */
  const CFGNode getCFGNode(SgGraphNode* node);

  /**
   *
   */
  void mpiPrintDotHeader(std::ostream& o);

  /**
   *
   */
  void mpiPrintNodes(std::ostream& o);

  /**
   *
   */
  void mpiPrintEdges(std::ostream& o);

  /**
   *
   */
  void mpiPrintNode(std::ostream& o, const VirtualCFG::CFGNode& node);

  /**
   *
   */
  void mpiPrintEdge(std::ostream& o,
                    const VirtualCFG::CFGNode& x,
                    const VirtualCFG::CFGNode& y);

public:
//=======================================================================================
//=======================================================================================
//                   Getter / Setter
//=======================================================================================
  /**
   * @brief Returns the start node of the MPI_ICFG.
   */
  SgNode* getEntry() {return start_;};

  /**
   * @brief Returns the graph.
   */
  SgIncidenceDirectedGraph* getGraph() {return graph_;};

  /**
   *
   */
  SgGraphNode* getGraphNode(CFGNode n) {return all_nodes_[n];};

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
  /**
   * @brief
   */
  void setRankInfo(std::vector<DataflowNode> rn) {ra_nodes_ = rn;};

  /**
   * @brief Sets the RankAnalysis pointer.
   */
  void setRankInfo(RankAnalysis* ra) {rank_analysis_ = (IntraProceduralDataflow*)ra;};

  /**
   * @brief
   */
  void setRankInfo(RankAnalysis* ra, std::vector<DataflowNode> rn)
    {ra_nodes_ = rn; rank_analysis_ = (IntraProceduralDataflow*)ra;};

  /**
   * @brief
   */
  string getRankPSetString(SgGraphNode* node);

  /**
   * @brief
   */
  bool hasRankInfo(SgGraphNode* node);

//=======================================================================================
  //LOOP RELATED FUNCTIONS





protected:
  ///////////////////////////////////////////////////////////////////////////////////////
  // function override from CFG
  /**
   * @brief Copy of CFG::processNodes(...) that calls MPI_CFG::printNodePlusEdges
   *
   * processNodes, printNOdePlusEdges are overwritten in order to call the the custom
   * printNode function from MPI ICFG.
   */
  void processNodes(std::ostream & o, SgGraphNode* n, std::set<SgGraphNode*>& explored);

  /**
   * @brief Copy of CFG::printNodePlusEdges(...) that calls MPI_CFG::printNode
   *
   * printNOdePlusEdges is overwritten in order to call the the custom
   * printNode function from MPI ICFG.
   */
  void printNodePlusEdges(std::ostream & o, SgGraphNode* node);

  /**
   * @brief Prints MPI nodes with additional RankAnalysis, and LoopAnalysis information.
   * @param o Output stream for the Debug Information.
   * @param node Graph node to be processed.
   */
  void printNode(std::ostream & o, SgGraphNode* node);


//=======================================================================================
public:
  //Functions For Debug OUTPUT
  /**
   * @brief Output the MPI ICFG to a DOT file and generates default file name.
   */
  void mpicfgToDot();

  /**
   * @brief Output the MPI ICFG to a DOT file.
   */
  void mpicfgToDot(const std::string& file_name);

  /**
   * @brief Output the possible communications to a Dot graph and generates file name.
   */
  void mpiCommToDot();

  /**
   * @brief Output the possible communications to a Dot graph.
   */
  void mpiCommToDot(const std::string& file_name);

};

#if 0
//TODO: remove this method from this class and put to mpi_utils!
// The following are some auxiliary functions, since SgGraphNode cannot provide them.
std::vector<SgDirectedGraphEdge*> mpiOutEdges(SgGraphNode* node);
std::vector<SgDirectedGraphEdge*> mpiInEdges(SgGraphNode* node);
#endif /*#if 0*/

} /* end of namespace MpiAnalysis */
#endif /* MPI_CFG_H */
