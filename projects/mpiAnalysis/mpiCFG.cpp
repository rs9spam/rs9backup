using namespace std;

#include "sage3basic.h"
#include "CallGraph.h"
#include "mpiCFG.h"
#include <boost/foreach.hpp>
#include <vector>

#define foreach BOOST_FOREACH

namespace MpiAnalysis
{

void MPICFG::addMPIEdge(CFGNode from, CFGNode to, std::vector<CFGEdge>& result) {
  // Makes a CFG edge, adding appropriate labels

}

void addEdge(CFGNode from, CFGNode to, std::vector<CFGEdge>& result) {
  // Makes a CFG edge, adding appropriate labels
  SgNode* fromNode = from.getNode();
  unsigned int fromIndex = from.getIndex();
  SgNode* toNode = to.getNode();

  // Exit early if the edge should not exist because of a control flow discontinuity
  if (fromIndex == 1 && (isSgGotoStatement(fromNode) || isSgBreakStmt(fromNode) || isSgContinueStmt(fromNode))) {
    return;
  }
  if (isSgReturnStmt(fromNode) && toNode == fromNode->get_parent()) {
    SgReturnStmt* rs = isSgReturnStmt(fromNode);
    if (fromIndex == 1 || (fromIndex == 0 && !rs->get_expression())) return;
  }
  if (isSgStopOrPauseStatement(fromNode) && toNode == fromNode->get_parent()) {
    SgStopOrPauseStatement* sps = isSgStopOrPauseStatement(fromNode);
    if (fromIndex == 0 && sps->get_stop_or_pause() == SgStopOrPauseStatement::e_stop) return;
  }
  if (fromIndex == 1 && isSgSwitchStatement(fromNode) &&
      isSgSwitchStatement(fromNode)->get_body() == toNode) return;

  // Create the edge
  result.push_back(CFGEdge(from, to));
}

void MPICFG::buildFullCFG()
{
	std::set<VirtualCFG::CFGNode> explored;
	graph_ = new SgIncidenceDirectedGraph;
	ClassHierarchyWrapper classHierarchy(SageInterface::getProject());

	// If the start node is an SgProject, build the CFG from main.
	CFGNode start;
	if ( isSgProject(start_) ) {
	  SgFunctionDeclaration* mainDefDecl = SageInterface::findMain(start_);
	  if (mainDefDecl == NULL)
		  ROSE_ASSERT (!"Cannot build CFG for project with no main function");

	  SgFunctionDefinition* mainDef = mainDefDecl->get_definition();
	  if (mainDef == NULL)
		  ROSE_ASSERT (!"Cannot build CFG for project with no main function");

	  start = mainDef->cfgForBeginning();
	} else {
	  start = start_->cfgForBeginning();
	}

	buildCFG(start, all_nodes_, explored, &classHierarchy);
	alNodes = all_nodes_;
	neededStart = start;
}

void MPICFG::buildFilteredCFG()
{
  ROSE_ASSERT(!"MPICFG:buildFilteredCFG() is unimplemented");
}

void MPICFG::buildCFG(CFGNode n,
                       std::map<CFGNode, SgGraphNode*>& all_nodes,
                       std::set<CFGNode>& explored,
                       ClassHierarchyWrapper* classHierarchy)
{
    SgNode* sgnode = n.getNode();
    ROSE_ASSERT(sgnode);

    if (explored.count(n) > 0)
        return;
    explored.insert(n);

    SgGraphNode* from = NULL;
    if (all_nodes.count(n) > 0)
    {
        from = all_nodes[n];
    }
    else
    {
        from = new SgGraphNode;
        from->set_SgNode(sgnode);
        from->addNewAttribute("info", new CFGNodeAttribute(n.getIndex(), graph_));
        all_nodes[n] = from;
        graph_->addNode(from);
    }

    std::vector<CFGEdge> outEdges;
    unsigned int idx = n.getIndex();

    if ((isSgFunctionCallExp(sgnode) &&
         idx == SGFUNCTIONCALLEXP_INTERPROCEDURAL_INDEX) ||
        (isSgConstructorInitializer(sgnode) &&
         idx == SGCONSTRUCTORINITIALIZER_INTERPROCEDURAL_INDEX)) {
          ROSE_ASSERT( isSgExpression(sgnode) );
          Rose_STL_Container<SgFunctionDefinition*> defs;
          CallTargetSet::getDefinitionsForExpression(isSgExpression(sgnode), classHierarchy, defs);
          if (defs.size() == 0) {
            std::cerr << sgnode->get_file_info()->get_filenameString()
                      << ":"
                      << sgnode->get_file_info()->get_line()
                      << " warning: CallGraph found no definition(s) for "
                      << sgnode->class_name()
                      << ". Skipping interprocedural behavior."
                      << std::endl;
            outEdges = n.outEdges();
          } else {
            foreach (SgFunctionDefinition* def, defs) {
              addEdge(n, def->cfgForBeginning(), outEdges);
              addEdge(def->cfgForEnd(), CFGNode(sgnode, idx+1), outEdges);
            }
          }
    }
    else {
      outEdges = n.outEdges();
    }

    std::set<CFGNode> targets;
    foreach (const CFGEdge& edge, outEdges)
    {
        CFGNode tar = edge.target();
        targets.insert(tar);

        SgGraphNode* to = NULL;
        if (all_nodes.count(tar) > 0)
            to = all_nodes[tar];
        else
        {
            to = new SgGraphNode;
            to->set_SgNode(tar.getNode());
            to->addNewAttribute("info", new CFGNodeAttribute(tar.getIndex(), graph_));
            all_nodes[tar] = to;
            graph_->addNode(to);
        }

        SgDirectedGraphEdge* new_edge = new SgDirectedGraphEdge(from, to);
        new_edge->addNewAttribute("info", new CFGEdgeAttribute<CFGEdge>(edge));
        graph_->addDirectedEdge(new_edge);
    }

    foreach (const CFGNode& target, targets)
    {
        buildCFG(target, all_nodes, explored, classHierarchy);
    }
}

void MPICFG::buildMPIICFG()
{
  buildCFG();
  buildMPISend();
  buildMPIRecv();
  addMPIEdgestoICFG();

  //std::cerr << "DEBUG build MPIICFG ##########################################" << endl;
  //typedef std::pair<VirtualCFG::CFGNode, SgGraphNode*> pair_t;
  //foreach ( pair_t& p, alNodes)
  //{
    //alNodes[];
    //alNodes[p.first];
    //all_nodes_[VirtualCFG::CFGNode(p.first.getNode(), 0)] = p.second;
  //}
}

void MPICFG::buildMPISend()
{
  for(map<CFGNode, SgGraphNode*>::iterator iter = all_nodes_.begin(); iter != all_nodes_.end(); ++iter) {
    //std::cerr << "SGFUNCTINCALL " << count << " ##########################################" << endl;
    if(isSgFunctionRefExp((iter->first).getNode())) {
      //TODO use a smart value for all MPI_SEND VALUES....
      //std::cerr << "SGFUNCTINCALL ########################################## << (iter->first).toString() << " ####" << endl;
      if( isSgFunctionRefExp((iter->first).getNode())->get_symbol()->get_name() == "MPI_Send") {
         std::cerr << isSgFunctionRefExp((iter->first).getNode())->get_symbol()->get_name()
                   << ".... real name .... ######################" << endl;
        mpiSendNodes[iter->first] = iter->second;
      }
    }
  }
}

void MPICFG::buildMPIRecv()
{
  for(map<CFGNode, SgGraphNode*>::iterator iter = all_nodes_.begin(); iter != all_nodes_.end(); iter++) {
    if(isSgFunctionRefExp((iter->first).getNode())) {
      //std::cerr << "SGFUNCTINCALL ##########################################" << endl;
      //TODO use a smart value for all MPI_RECV VALUES....
      if( isSgFunctionRefExp((iter->first).getNode())->get_symbol()->get_name() == "MPI_Recv" ) {
        std::cerr << isSgFunctionRefExp((iter->first).getNode())->get_symbol()->get_name()
                  << ".... real name .... ######################" << endl;
        mpiRecvNodes[iter->first] = iter->second;
      }
    }
  }
}

void MPICFG::addMPIEdgestoICFG()
{
  for(map<CFGNode, SgGraphNode*>::iterator iter1 = mpiSendNodes.begin(); iter1 != mpiSendNodes.end(); iter1++)
    for(map<CFGNode, SgGraphNode*>::iterator iter2 = mpiRecvNodes.begin(); iter2 != mpiRecvNodes.end(); iter2++) {
   	  SgDirectedGraphEdge* new_edge = new SgDirectedGraphEdge(iter1->second, iter2->second);
      new_edge->addNewAttribute("info", new CFGEdgeAttribute<CFGEdge>(CFGEdge((iter1->first), (iter2->first))));
      graph_->addDirectedEdge(new_edge);
      std::cerr << "HELLO MPI EDGES   !!!!!!!!!!!!!!!!" << endl << endl;

//      SgDirectedGraphEdge* new_edge1 = new SgDirectedGraphEdge(iter1->second, iter2->second);
//      new_edge1->addNewAttribute("info", new CFGEdgeAttribute<CFGEdge>(CFGEdge((iter1->first), (iter2->first))));
//      graph_->addDirectedEdge(new_edge1);
//      std::cerr << "HELLO MPI EDGES 1   !!!!!!!!!!!!!!!!" << endl << endl;
    }
}

} // end of namespace MpiAnalysis
