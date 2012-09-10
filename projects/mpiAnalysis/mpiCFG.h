#ifndef MPI_CFG_H
#define MPI_CFG_H

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

class MPICFG : public CFG
{
protected:
    virtual void buildCFG(CFGNode n,
                  std::map<CFGNode, SgGraphNode*>& all_nodes,
                  std::set<CFGNode>& explored,
                  ClassHierarchyWrapper* classHierarchy);
public:
    MPICFG() : CFG() {}

    // The valid nodes are SgProject, SgStatement, SgExpression and SgInitializedName
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
        return alNodes[n];
    }
    // Build CFG according to the 'is_filtered_' flag.
    virtual void buildCFG()
    {
        buildFullCFG();
    }
    void addMPIEdge(CFGNode from, CFGNode to, std::vector<CFGEdge>& result);

    std::map<CFGNode, SgGraphNode*> alNodes;
    std::map<CFGNode, SgGraphNode*>	mpiSendNodes;
    std::map<CFGNode, SgGraphNode*> mpiRecvNodes;
    CFGNode neededStart;

    // Build CFG for debugging.
    virtual void buildFullCFG();
    // Build filtered CFG which only contains interesting nodes.
    virtual void buildFilteredCFG();
    // Outer constructor calling construct.....
    void buildMPIICFG();
    // Find all MPI_Send function calls and insert the pairs into mpiSendNodes
    void buildMPISend();
    // Find all MPI_Recv function calls and insert the pairs into mpiRecvNodes
    void buildMPIRecv();

    void addMPIEdgestoICFG();
};

} // end of namespace MpiAnalysis

#endif
