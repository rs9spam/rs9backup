using namespace std;

#include "sage3basic.h"
#include "CallGraph.h"
#include "mpiCFG.h"
#include <boost/foreach.hpp>
#include <vector>

#define foreach BOOST_FOREACH

namespace MpiAnalysis
{

//void MPICFG::addMPIEdge(CFGNode from, CFGNode to, std::vector<CFGEdge>& result) {
//  // Makes a CFG edge, adding appropriate labels
//
//}

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
	alNodes_ = all_nodes_;
	neededStart_ = start;
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
  buildFullCFG();
  buildMPISend();
  buildMPIRecv();
  addMPIEdgestoICFG();
  refineTypeMatch();

//  std::cerr << "DEBUG build MPIICFG ##########################################" << endl;
//  typedef std::pair<VirtualCFG::CFGNode, SgGraphNode*> pair_t;
//  foreach ( pair_t& p, alNodes)
//  {
//    alNodes[];
//    alNodes[p.first];
//    all_nodes_[VirtualCFG::CFGNode(p.first.getNode(), 0)] = p.second;
//  }
}

void MPICFG::buildMPISend()
{
  for(map<CFGNode, SgGraphNode*>::iterator iter = all_nodes_.begin(); iter != all_nodes_.end(); ++iter) {
    if(isSgFunctionCallExp((iter->first).getNode()))
      if( ((iter->first).getIndex() == 1)  && isMPISend((iter->first).getNode()))
      {
        mpiSendNodes_[iter->first] = iter->second;
      }
  }
}

bool MPICFG::isMPISend(SgNode* node)
{
  string name;
  if( isSgFunctionCallExp(node)) {
    if( isSgFunctionRefExp(isSgFunctionCallExp(node)->get_function())) {
      name = isSgFunctionRefExp(isSgFunctionCallExp(node)->get_function())->get_symbol()->get_name();
      if(name == "MPI_Send" || name == "MPI_Isend")
      {
        //TODO: remove DEBUG
        std::cerr << name << endl;
        return true;
      }
    }
  }
  return false;
}

void MPICFG::buildMPIRecv()
{
  for(map<CFGNode, SgGraphNode*>::iterator iter = all_nodes_.begin(); iter != all_nodes_.end(); ++iter) {
    if(isSgFunctionCallExp((iter->first).getNode()))
      if( ((iter->first).getIndex() == 1)  && isMPIRecv((iter->first).getNode()))
    	  mpiRecvNodes_[iter->first] = iter->second;
  }
}

bool MPICFG::isMPIRecv(SgNode* node)
{
  string name;
  if( isSgFunctionCallExp(node))
  {
    if( isSgFunctionRefExp(isSgFunctionCallExp(node)->get_function()))
    {
      name = isSgFunctionRefExp(isSgFunctionCallExp(node)->get_function())->get_symbol()->get_name();
      if(name == "MPI_Recv" || name == "MPI_Irecv")
      {
        //TODO: remove DEBUG
        std::cerr << name << endl;
        return true;
      }
    }
  }
  return false;
}

void MPICFG::addMPIEdgestoICFG()
{
  for(map<CFGNode, SgGraphNode*>::iterator iter1 = mpiSendNodes_.begin(); iter1 != mpiSendNodes_.end(); iter1++)
    for(map<CFGNode, SgGraphNode*>::iterator iter2 = mpiRecvNodes_.begin(); iter2 != mpiRecvNodes_.end(); iter2++)
    {
      SgDirectedGraphEdge* new_graph_edge = new SgDirectedGraphEdge(iter1->second, iter2->second);
      CFGEdge new_cfg_edge = CFGEdge((iter1->first), (iter2->first));
      CFGEdgeAttribute<CFGEdge>* new_cfg_edge_attr = new CFGEdgeAttribute<CFGEdge>(new_cfg_edge);
      new_graph_edge->addNewAttribute("info", new_cfg_edge_attr);
      //Add edge to MPI_ICFG graph
      graph_->addDirectedEdge(new_graph_edge);

      //Add out edge of source node
      //Add to in edge of target node

      std::cerr << "Add MPI Edge" << endl;
    }
}

void MPICFG::refineTypeMatch()
{
  map<CFGNode, SgGraphNode*>::iterator node_iter;
  map<CFGNode, SgGraphNode*>::iterator iter2;
  vector<CFGEdge>::iterator edge_iter;
  for(node_iter = mpiSendNodes_.begin(); node_iter != mpiSendNodes_.end(); node_iter++)
  {
    std::cerr<< "send nodes\n";
    for(iter2 = mpiRecvNodes_.begin(); iter2 != mpiRecvNodes_.end(); iter2++)
    {
    //std::cerr<< "workes fine here 1" << endl;
      //(edge_iter->source()).getNode();
      std::cerr<< "test \n"<< (node_iter->first).toString() << endl << (iter2->first).toString() << endl;
//      std::cerr<< "workes fine here 2" << endl;
//      if(isSgFunctionCallExp((edge_iter->source()).getNode()))
//          std::cerr<< "workes fine here 3" << endl;
//
//

      if(!checkMatchTypes(node_iter->first, iter2->first))
      {
        //removeMPIedge(edge_iter->first);
      }

    }
    CFGNode tempnode = node_iter->first;
    vector<CFGEdge> oedges = tempnode.outEdges();
    for(edge_iter = oedges.begin(); edge_iter != oedges.end(); edge_iter++)
    {
      std::cerr<< "\n out edges  \n" << edge_iter->source().toString() << endl
          << edge_iter->target().toString() << endl << endl;
    }


  }
}

bool MPICFG::checkMatchTypes(CFGNode send_node, CFGNode recv_node)
{

  std::cerr << "checking " << endl;
  if(send_node != NULL)
  {
    std::cerr << "there is a node " << endl;
    if(isSgFunctionCallExp((send_node.getNode())))
    {
      std::cerr << "it is a SgCall Expression" << endl;
      return true;
    }
  }


  return false;
}

} // end of namespace MpiAnalysis
