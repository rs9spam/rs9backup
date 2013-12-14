#ifndef MPI_CFG_H
#define MPI_CFG_H

#include "staticCFG.h"
#include "CallGraph.h"
#include "dataflow.h"
#include "mpiUtils/mpiUtils.h"
#include "mpiCFG/MpiCommunication.h"
#include "latticeFull.h"

class SgIncidenceDirectedGraph;
class SgGraphNode;
class SgDirectedGraphEdge;
class ConstantPropagationAnalysis;
class LoopAnalysis;
class RankAnalysis;
class CallAnalysis;

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

  /**
   *
   */
  MpiCommunication mpic_;

  /// Pointer to Constant Propagation Analysis information.
//  IntraProceduralDataflow* const_prop_;
  ConstantPropagationAnalysis* const_prop_;
  /// Pointer to Rank Analysis information.
  RankAnalysis* rank_analysis_;
//  IntraProceduralDataflow* rank_analysis_;
  /// Pointer to Loop Analysis information.
  LoopAnalysis* loop_analysis_;
//  IntraProceduralDataflow* loop_analysis_;
  /// Pointer to Call Analysis class.
  CallAnalysis* call_analysis_;
//  IntraProceduralDataflow* call_analysis_;

#if 0
  //This is just an auxiliary nodes vectore since I can't access the real nodes.
  /**
   * @brief Auxiliary vector with DataflowNodes to avoid creating new DataFlowNodes
   *        from SgGraph nodes which result in a Segmentation fault.
   */
  std::vector<DataflowNode> cpa_nodes_;
  std::vector<DataflowNode> ra_nodes_;
  std::vector<DataflwoNode> la_nodes_;
  std::vector<DataflwoNode> cla_nodes_;
#endif

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
         ConstantPropagationAnalysis* const_prop,
         RankAnalysis* rank_analysis,
         LoopAnalysis* loop_analysis,
         CallAnalysis* call_analysis,
         bool is_filtered = false)
    : CFG() { graph_ = NULL;
              mpi_project_ = NULL;
              is_filtered_ = is_filtered;
              start_ = node;
              const_prop_ = const_prop;
//              rank_analysis_ = (IntraProceduralDataflow*)rank_analysis;
//              loop_analysis_ = (IntraProceduralDataflow*)loop_analysis;
//              call_analysis_ = (IntraProceduralDataflow*)call_analysis;
              rank_analysis_ = rank_analysis;
              loop_analysis_ = loop_analysis;
              call_analysis_ = call_analysis;
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

  /**
   * @brief Add all possible MPI Edges to MPI_ICFG.
   */
  void addMPIEdgestoICFG();

  /**
   * @brief Remove the SgDirectedGraphEdge from the MPI_ICFG.
   * @param edge Edge to be removed from the MPI_ICFG.
   */
  void removeMPIEdge(SgDirectedGraphEdge* edge);



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
  SgIncidenceDirectedGraph* getGraph() { return graph_; };

  /**
   *
   */
  SgGraphNode* getGraphNode(CFGNode n) { return all_nodes_[n]; };

#if 0
  /**
   * @brief Sets the DataflowNodes vector for accessing rank-, constant propagation-,
   *        loop- and callList-Analysis.
   */
  void setDataflowNodesCPA(const std::vector<DataflowNode> dfnv)
    { cpa_nodes_.assign(dfnv.begin(), dfnv.end()); };
#endif
//=======================================================================================
  //RANK RELATED FUNCTIONS
#if 0
  /**
   * @brief
   */
  void setDataflowNodesRA(const std::vector<DataflowNode> rn) {ra_nodes_ = rn;};
#endif

  /**
   * @brief Sets the RankAnalysis pointer.
   */
  void setRankInfo(RankAnalysis* ra) {rank_analysis_ = (RankAnalysis*)ra;};
//  void setRankInfo(RankAnalysis* ra) {rank_analysis_ = (IntraProceduralDataflow*)ra;};

#if 0
  /**
   * @brief
   */
  void setRankInfo(RankAnalysis* ra, std::vector<DataflowNode> rn)
    { ra_nodes_ = rn; rank_analysis_ = (IntraProceduralDataflow*)ra; };
#endif

  /**
   * @brief
   */
  string getRankPSetString(const CFGNode& node) const;

  /**
   * @brief
   */
  bool hasRankInfo(const CFGNode& node) const;

//=======================================================================================
  //LOOP RELATED FUNCTIONS



//=======================================================================================
//=======================================================================================
//              DEBUG OUTPUT -- PRINT FUNCTIONS
//=======================================================================================

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

};

#if 0
//TODO: remove this method from this class and put to mpi_utils!
// The following are some auxiliary functions, since SgGraphNode cannot provide them.
std::vector<SgDirectedGraphEdge*> mpiOutEdges(SgGraphNode* node);
std::vector<SgDirectedGraphEdge*> mpiInEdges(SgGraphNode* node);
#endif /*#if 0*/

} /* end of namespace MpiAnalysis */
#endif /* MPI_CFG_H */
