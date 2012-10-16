#ifndef MPI_CFG_H
#define MPI_CFG_H

//#include "staticMPICFG.h"
#include "staticCFG.h"
#include "CallGraph.h"
#include <map>
#include <set>
#include <string>
#include "mpiCFG.h"

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

using namespace StaticCFG;
using VirtualCFG::CFGNode;
using VirtualCFG::CFGEdge;

//using namespace StaticMPICFG;
//using VirtualMPICFG::CFGNode;
//using VirtualMPICFG::CFGEdge;

//CFG is from StaticCFG
class MPICFG : public CFG
{
  protected:
    virtual void buildCFG(CFGNode n,
                  std::map<CFGNode, SgGraphNode*>& all_nodes,
                  std::set<CFGNode>& explored,
                  ClassHierarchyWrapper* classHierarchy);
  public:
    MPICFG() : CFG() {}

    //! The valid nodes are SgProject, SgStatement, SgExpression and SgInitializedName
    MPICFG(SgNode* node, bool is_filtered = false)
      : CFG() {
        graph_ = NULL;
        is_filtered_ = is_filtered;
        start_ = node;
        buildMPIICFG();
      }
    SgNode* getEntry()
    {
      return start_;
    }
    SgIncidenceDirectedGraph* getGraph()
    {
      return graph_;
    }
    SgGraphNode* getGraphNode(CFGNode n) {
      return alNodes_[n];
    }
    //! Build CFG according to the 'is_filtered_' flag.
    virtual void buildCFG()
    {
      buildFullCFG();
    }
    //void addMPIEdge(CFGNode from, CFGNode to, std::vector<CFGEdge>& result);

    std::map<CFGNode, SgGraphNode*> alNodes_;
    std::map<CFGNode, SgGraphNode*>	mpiSendNodes_;
    std::map<CFGNode, SgGraphNode*> mpiRecvNodes_;
    //std::map<CFGEdge, SgDirectedGraphEdge*> mpiCommEdges_;
    CFGNode neededStart_;

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
    //! Add all possible MPI Edges to MPI_ICFG
    void addMPIEdgestoICFG();
    //! remove the SgDirectedGraphEdge from the MPI_ICFG
    void removeMPIEdge(SgDirectedGraphEdge* edge);

    //! Removes communication Edges with non matching constant data types
    //! Removes communication Edges with non matching constant data size
    //! Removes communication Edges with non matching constant communication tag
    //! Removes communication Edges with non matching constant communication world
    void refineConstantMatch();

    //! Compares the data type of the MPI_Send and MPI_Recv node
    //!returns false only if constant Type Arguments do not match!
    bool checkConstTypeMatch(SgGraphNode* send_node, SgGraphNode* recv_node);
    //! Compares the data type of the MPI_Send and MPI_Recv node
    //!returns false only if constant Size Arguments do not match!
    bool checkConstSizeMatch(SgGraphNode* send_node, SgGraphNode* recv_node);
    //! Compares the data type of the MPI_Send and MPI_Recv node
    //!returns false only if constant Tag Arguments do not match!
    bool checkConstTagMatch(SgGraphNode* send_node, SgGraphNode* recv_node);
    //! Compares the data type of the MPI_Send and MPI_Recv node
    //!returns false only if constant CommWorld Arguments do not match!
    bool checkConstCommWorldMatch(SgGraphNode* send_node, SgGraphNode* recv_node);

    //! Returns the corresponding SGNode from SgGraphNode
    const CFGNode getCFGNode(SgGraphNode* node);
};

class MPIInfo
{
private:
//  std::vector<CFGEdge>* mpi_outedges;
//  std::vector<CFGEdge>* mpi_inedges;

public:
//  MPIInfo() {mpi_outedges=0; mpi_inedges=0;}
//  std::vector<CFGEdge>* getMpiOutEdges();
//  std::vector<CFGEdge>* getMpiInEdges();
//  void addMpiOutEdge();
//  void addMpiInEdge();
};

// The following are some auxiliary functions, since SgGraphNode cannot provide them.
std::vector<SgDirectedGraphEdge*> mpiOutEdges(SgGraphNode* node);
std::vector<SgDirectedGraphEdge*> mpiInEdges(SgGraphNode* node);

} // end of namespace MpiAnalysis

#endif
