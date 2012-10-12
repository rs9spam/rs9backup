#ifndef MPI_CFG_H
#define MPI_CFG_H

//#include "staticMPICFG.h"
#include "staticCFG.h"
#include "CallGraph.h"
#include <map>
#include <set>
#include <string>


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
    //! Removes communication Edges with non matching data types
    void refineTypeMatch();
    //! Compares the data type of the MPI_Send and MPI_Recv node
    bool checkMatchTypes(CFGNode send_node, CFGNode recv_node);
};

} // end of namespace MpiAnalysis

#endif
