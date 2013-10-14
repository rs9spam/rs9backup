#ifndef MPI_CFG_H
#define MPI_CFG_H

//#include "staticMPICFG.h"
#include "staticCFG.h"
#include "CallGraph.h"
#include <map>
#include <set>
#include <string>
#include "mpiCFG.h"
#include "dataflow.h"

#include "latticeFull.h"
#include "liveDeadVarAnalysis.h"
#include "constantPropagationAnalysis.h"

#define MPI_NUM_SEND_EXP 6
#define MPI_NUM_RECV_EXP 7
#define MPI_VAR_ARG 1
#define MPI_SIZE_ARG 2
#define MPI_TYPE_ARG 3
#define MPI_SOURCE_ARG 4
#define MPI_TAG_ARG 5
#define MPI_COMM_WORLD_ARG 6
#define MPI_STATUS_ARG 7

class SgIncidenceDirectedGraph;
class SgGraphNode;
class SgDirectedGraphEdge;

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

//===================================================================================
  protected:
    virtual void buildCFG(CFGNode n,
                  std::map<CFGNode, SgGraphNode*>& all_nodes,
                  std::set<CFGNode>& explored,
                  ClassHierarchyWrapper* classHierarchy);

    //Constant Propagation Element
    //Required for constant propagation pruning
    IntraProceduralDataflow* const_prop_;

    //void addMPIEdge(CFGNode from, CFGNode to, std::vector<CFGEdge>& result);
    //all nodes already defined in staticCFG.h
    //std::map<CFGNode, SgGraphNode*> all_nodes_;

    //pointer to the whole project
    SgNode* mpi_project_;

    std::map<CFGNode, SgGraphNode*> mpi_send_nodes_;
    std::map<CFGNode, SgGraphNode*> mpi_recv_nodes_;
    //ALL possible MPI connections between MPI_Send and MPI_Recv
    std::multimap<CFGNode, CFGNode> mpi_connections_;

    //std::map<CFGEdge, SgDirectedGraphEdge*> mpiCommEdges_;

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  public:
    MPICFG() : CFG() {}

    //! Valid node types are SgProject, SgStatement, SgExpression, SgInitializedName
    MPICFG(SgNode* node,
           IntraProceduralDataflow* intraDataflowAnalysis,
           bool is_filtered = false)
      : CFG() { graph_ = NULL;
                mpi_project_ = NULL;
                is_filtered_ = is_filtered;
                start_ = node;
                const_prop_ = intraDataflowAnalysis;
      }

    //Entry to build the full MPIICFG
    //TODO: add the pruning option parameters to the build function
    void build();

    SgNode* getEntry(){
      return start_;
    }

    SgIncidenceDirectedGraph* getGraph(){
      return graph_;
    }

    SgGraphNode* getGraphNode(CFGNode n) {
      return all_nodes_[n];
    }

    //! Build CFG according to the 'is_filtered_' flag.
    virtual void buildCFG(){
      buildFullCFG();
    }

    //! Build CFG for debugging.
    virtual void buildFullCFG();

    //! Outer constructor calling construct.....
    void buildMPIICFG();

    //! Find all MPI_Send function calls and insert the pairs into mpiSendNodes
    void buildMPISend();

    //! Check if SgFunctionCall node is of type MPI Send, MPI_Isend, ...
    bool isMPISend(SgNode* expr);

    //! Find all MPI_Recv function calls and insert the pairs into mpiRecvNodes
    void buildMPIRecv();

    //! Check if SgFunctionCall node is of type MPI_Recv, MPI_Irecv, ...
    bool isMPIRecv(SgNode* expr);

    //! Fills a Map with all possible match sets, ...
    bool buildFullMPIMatchSet();

    //! Add all possible MPI Edges to MPI_ICFG
    void addMPIEdgestoICFG();
#if 0
    void addMPIEdge(CFGNode from, CFGNode to, std::vector<CFGEdge>& result);
#endif
    //! remove the SgDirectedGraphEdge from the MPI_ICFG
    void removeMPIEdge(SgDirectedGraphEdge* edge);

    //! Removes communication Edges with non matching constant data types
    //! Removes communication Edges with non matching constant data size
    //! Removes communication Edges with non matching constant communication tag
    //! Removes communication Edges with non matching constant communication world
    void refineConstantMatch();

    //! Compares the data type of the MPI_Send and MPI_Recv node
    //!returns false only if constant Type Arguments do not match!
    bool constTypeMatch(SgGraphNode* send_node, SgGraphNode* recv_node);

    //! Compares the data type of the MPI_Send and MPI_Recv node
    //!returns false only if constant Size Arguments are not valid for a match!
    bool constSizeMatch(SgGraphNode* send_node, SgGraphNode* recv_node);

    //! Compares the data type of the MPI_Send and MPI_Recv node
    //!returns false only if constant Tag Arguments do not match!
    bool constTagMatch(SgGraphNode* send_node, SgGraphNode* recv_node);

    //! Compares the data type of the MPI_Send and MPI_Recv node
    //!returns false only if constant CommWorld Arguments do not match!
    bool constCommWorldMatch(SgGraphNode* send_node, SgGraphNode* recv_node);

    bool hasConstValue(SgNode* node);
    int getConstPropValue(SgNode* node);

    //! Returns the corresponding SGNode from SgGraphNode
    const CFGNode getCFGNode(SgGraphNode* node);

    //! Output the MPI ICFG to a DOT file and generates default file name.
    void mpicfgToDot();

    //! Output the MPI ICFG to a DOT file.
    void mpicfgToDot(const std::string& file_name);

    //! Output the possible communications to a Dot graph and generates file name.
    void mpiCommToDot();

    //! Output the possible communications to a Dot graph.
    void mpiCommToDot(const std::string& file_name);
    void mpiPrintDotHeader(std::ostream& o);
    void mpiPrintNodes(std::ostream& o);
    void mpiPrintEdges(std::ostream& o);
    void mpiPrintNode(std::ostream& o, const VirtualCFG::CFGNode& node);
    void mpiPrintEdge(std::ostream& o,
                      const VirtualCFG::CFGNode& x,
                      const VirtualCFG::CFGNode& y);
};


//class MPIInfo
//{
//private:
////  std::vector<CFGEdge>* mpi_outedges;
////  std::vector<CFGEdge>* mpi_inedges;
//
//public:
////  MPIInfo() {mpi_outedges=0; mpi_inedges=0;}
////  std::vector<CFGEdge>* getMpiOutEdges();
////  std::vector<CFGEdge>* getMpiInEdges();
////  void addMpiOutEdge();
////  void addMpiInEdge();
//};

// The following are some auxiliary functions, since SgGraphNode cannot provide them.
std::vector<SgDirectedGraphEdge*> mpiOutEdges(SgGraphNode* node);
std::vector<SgDirectedGraphEdge*> mpiInEdges(SgGraphNode* node);



} // end of namespace MpiAnalysis

#endif
