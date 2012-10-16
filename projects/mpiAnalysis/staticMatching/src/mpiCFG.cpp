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
//  //create NewAttribute for SgNode (for send nodes) (class MPIInfo)
//  for(map<CFGNode, SgGraphNode*>::iterator iter1 = mpiSendNodes_.begin(); iter1 != mpiSendNodes_.end(); iter1++)
//  {
//    MPIInfo mpi_info = new MPIInfo();
//  }
//  //create NewAttribute for SgNode (for Recv nodes) (class MPIInfo)
//  for(map<CFGNode, SgGraphNode*>::iterator iter2 = mpiRecvNodes_.begin(); iter2 != mpiRecvNodes_.end(); iter2++)
//  {
//
//  }

  for(map<CFGNode, SgGraphNode*>::iterator iter1 = mpiSendNodes_.begin(); iter1 != mpiSendNodes_.end(); iter1++)

    for(map<CFGNode, SgGraphNode*>::iterator iter2 = mpiRecvNodes_.begin(); iter2 != mpiRecvNodes_.end(); iter2++)
    {
      SgDirectedGraphEdge* new_graph_edge = new SgDirectedGraphEdge(iter1->second, iter2->second);
      CFGEdge new_cfg_edge = CFGEdge((iter1->first), (iter2->first));
      CFGEdgeAttribute<CFGEdge>* new_cfg_edge_attr = new CFGEdgeAttribute<CFGEdge>(new_cfg_edge);
      new_graph_edge->addNewAttribute("info", new_cfg_edge_attr);
      CFGEdgeAttribute<bool>* mpi_info_attr = new CFGEdgeAttribute<bool>(true);
      new_graph_edge->addNewAttribute("mpi_info", mpi_info_attr);
      //Add edge to MPI_ICFG graph
      graph_->addDirectedEdge(new_graph_edge);

      std::cerr << "Add MPI Edge" << endl;
    }
}

void MPICFG::refineTypeMatch()
{
  map<CFGNode, SgGraphNode*>::iterator node_iter;
  std::vector<SgDirectedGraphEdge*>::iterator edge_iter;
  for(node_iter = mpiSendNodes_.begin(); node_iter != mpiSendNodes_.end(); node_iter++)
  {
    std::cerr<< "send node\n";
    std::vector<SgDirectedGraphEdge*> out_edges = mpiOutEdges((node_iter->second));
    for(edge_iter = out_edges.begin(); edge_iter != out_edges.end(); edge_iter++)
    {
      std::cerr<< "  recv node\n";
      if(!checkMatchTypes((*edge_iter)->get_from(),(*edge_iter)->get_to()))
      {
        //removeMPIedge(edge_iter->first);
      }

    }
//    CFGNode tempnode = node_iter->first;
//    vector<CFGEdge> oedges = tempnode.outEdges();
//    for(edge_iter = oedges.begin(); edge_iter != oedges.end(); edge_iter++)
//    {
//      std::cerr<< "\n out edges  \n" << edge_iter->source().toString() << endl
//          << edge_iter->target().toString() << endl << endl;
//    }
//
//    SgGraphNode* tempgn = node_iter->second;
//    std::cerr << "##############number of traversal successors: "
//        << tempgn->get_numberOfTraversalSuccessors()
//        << " number of attributes: " << tempgn->numberOfAttributes() << endl;
//
//    std::vector<SgDirectedGraphEdge*> tempvector = outEdges(tempgn);
//    std::vector<SgDirectedGraphEdge*>::iterator sgiter;
//    for(sgiter = tempvector.begin(); sgiter != tempvector.end(); sgiter++)
//    {
//      std::cerr << "##test" << endl;//sgiter->toString() << endl;
//    }
//
//
//    tempvector = mpiOutEdges(tempgn);
//    for(sgiter = tempvector.begin(); sgiter != tempvector.end(); sgiter++)
//    {
//      std::cerr << "##test for MPI" << endl;//sgiter->toString() << endl;
//    }

    //vector<CFGEdge> oedges = tempnode.outEdges();
//    for(edge_iter = oedges.begin(); edge_iter != oedges.end(); edge_iter++)
//    {
//      std::cerr<< "\n out edges  \n" << edge_iter->source().toString() << endl
//          << edge_iter->target().toString() << endl << endl;
//    }
  }
}

SgExpression* getExpAt(SgExpressionPtrList argsptr ,unsigned int arg_nr)
{
  SgExpressionPtrList::iterator exp_iter;

  if(argsptr.size() >= arg_nr)
  {
    exp_iter = argsptr.begin();
//    std::cerr << "From size: " << argsptr.size() << endl;

    for(unsigned int i=1; i < arg_nr; i++)
      exp_iter++;

    SgExpression* exp = (*exp_iter);
    return exp;
  }
  return NULL;
}

//bool MPICFG::checkMatchTypes(SgGraphNode* send_node, SgGraphNode* recv_node)
//{
//  CFGNode from = getCFGNode(send_node);
//  CFGNode to = getCFGNode(recv_node);
//  if ((from == NULL) || (to == NULL))
//    ROSE_ASSERT (!"Cannot find the SgGraphNode in map");
//
//  if(isSgFunctionCallExp(from.getNode()) && isSgFunctionCallExp(to.getNode()))
//  {
//    std::cerr << "send/recv are SgCall Expressions" << endl;
//    SgExpressionPtrList fromargsptr = isSgFunctionCallExp(from.getNode())->get_args()->get_expressions();
//    SgExpressionPtrList::iterator from_exp_iter;
//    SgExpressionPtrList toargsptr = isSgFunctionCallExp(to.getNode())->get_args()->get_expressions();
//    SgExpressionPtrList::iterator to_exp_iter;
//
//    if(fromargsptr.size() >= MPI_NUM_SEND_EXP && toargsptr.size() >= MPI_NUM_RECV_EXP)
//    {
//      from_exp_iter = fromargsptr.begin();
//      to_exp_iter = toargsptr.begin();
////      std::cerr << "From size: " << fromargsptr.size() << endl;
////      std::cerr << "To size: " << toargsptr.size() << endl;
//      for(int i=1; i < MPI_TYPE_ARG; i++)
//      {
//        from_exp_iter++;
//        to_exp_iter++;
//      }
//
//      SgExpression* from_exp = (*from_exp_iter);
//      while(isSgUnaryOp(from_exp))
//        from_exp = isSgUnaryOp(from_exp)->get_operand();
//      SgExpression* to_exp = (*to_exp_iter);
//      while(isSgUnaryOp(to_exp))
//        to_exp = isSgUnaryOp(to_exp)->get_operand();
////      std::cerr << "CLASS: " << (from_exp)->sage_class_name() << endl;
////      std::cerr << "CLASS: " << (to_exp)->sage_class_name() << endl;
////      std::cerr << "TYPE: " << (from_exp)->get_type()->unparseToString() << endl;
////      std::cerr << "TYPE: " << (to_exp)->get_type()->unparseToString() << endl;
//
//      if(isSgVarRefExp(from_exp) && isSgVarRefExp(to_exp))
//      {
////        std::cerr << "Symbol name: " << isSgVarRefExp(from_exp)->get_symbol()->get_name().str() << endl;
////        std::cerr << "Symbol name: " << isSgVarRefExp(to_exp)->get_symbol()->get_name().str() << endl;
//        //TODO ... #define a value for that .. check if it's always ompi_mpi ...
//        std::string ompi = "ompi_mpi";
//        std::string from_s = isSgVarRefExp(from_exp)->get_symbol()->get_name().str();
//        std::string to_s = isSgVarRefExp(to_exp)->get_symbol()->get_name().str();
//        if(from_s == to_s && from_s.substr(0,ompi.size()) == ompi)
//        {
//          std::cerr << "Send and Recv have same Type: " << from_s << endl;
//          return true;
//        }
//      }
//    }
//  }
//  return false;
//}

bool MPICFG::checkMatchTypes(SgGraphNode* send_node, SgGraphNode* recv_node)
{
  std::cerr << endl << "Refine: Match Types" << endl;
  CFGNode from = getCFGNode(send_node);
  CFGNode to = getCFGNode(recv_node);
  if ((from == NULL) || (to == NULL))
    ROSE_ASSERT (!"Cannot find the SgGraphNode in map");

  if(isSgFunctionCallExp(from.getNode()) && isSgFunctionCallExp(to.getNode()))
  {
    SgExpressionPtrList fromargsptr = isSgFunctionCallExp(from.getNode())->get_args()->get_expressions();
    SgExpressionPtrList toargsptr = isSgFunctionCallExp(to.getNode())->get_args()->get_expressions();

    SgExpression* from_exp = getExpAt(fromargsptr ,MPI_TYPE_ARG);
    if(from_exp == NULL)
      return false;
    while(isSgUnaryOp(from_exp))
      from_exp = isSgUnaryOp(from_exp)->get_operand();

    SgExpression* to_exp = getExpAt(toargsptr ,MPI_TYPE_ARG);
    if(to_exp == NULL)
      return false;
    while(isSgUnaryOp(to_exp))
      to_exp = isSgUnaryOp(to_exp)->get_operand();
//    std::cerr << "CLASS: " << (from_exp)->sage_class_name() << endl;
//    std::cerr << "CLASS: " << (to_exp)->sage_class_name() << endl;
//    std::cerr << "TYPE: " << (from_exp)->get_type()->unparseToString() << endl;
//    std::cerr << "TYPE: " << (to_exp)->get_type()->unparseToString() << endl;

    if(isSgVarRefExp(from_exp) && isSgVarRefExp(to_exp))
    {
//      std::cerr << "Symbol name: " << isSgVarRefExp(from_exp)->get_symbol()->get_name().str() << endl;
//      std::cerr << "Symbol name: " << isSgVarRefExp(to_exp)->get_symbol()->get_name().str() << endl;
      //TODO ... #define a value for that .. check if it's always ompi_mpi ...
      std::string ompi = "ompi_mpi";
      std::string from_s = isSgVarRefExp(from_exp)->get_symbol()->get_name().str();
      std::string to_s = isSgVarRefExp(to_exp)->get_symbol()->get_name().str();
      if(from_s == to_s && from_s.substr(0,ompi.size()) == ompi)
      {
        std::cerr << "Send and Recv have same Type: " << from_s << endl;
        return true;
      }
    }
  }
  return false;
}

const CFGNode MPICFG::getCFGNode(SgGraphNode* node)
{
  map<CFGNode, SgGraphNode*>::iterator iter;
  for(iter = alNodes_.begin(); iter != alNodes_.end(); iter++)
    if(node == (iter->second))
      return (iter->first);

  return NULL;
}

std::vector<SgDirectedGraphEdge*> mpiOutEdges(SgGraphNode* node)
{
  std::vector<SgDirectedGraphEdge*> out_edges = outEdges(dynamic_cast<SgGraphNode*>(node));
  std::vector<SgDirectedGraphEdge*> mpi_out_edges;
  std::vector<SgDirectedGraphEdge*>::iterator iter;
  for(iter = out_edges.begin(); iter != out_edges.end(); iter++)
  {
    //remove not mpi_nodes from vector
    CFGEdgeAttribute<bool>* mpi_info_attr = dynamic_cast<CFGEdgeAttribute<bool>*>((*iter)->getAttribute("mpi_info"));
    if(mpi_info_attr)
      mpi_out_edges.push_back(*iter);
  }
  return std::vector<SgDirectedGraphEdge*>(mpi_out_edges.begin(), mpi_out_edges.end());
}

std::vector<SgDirectedGraphEdge*> mpiInEdges(SgGraphNode* node)
{
  std::vector<SgDirectedGraphEdge*> in_edges = inEdges(dynamic_cast<SgGraphNode*>(node));
  std::vector<SgDirectedGraphEdge*> mpi_in_edges;
  std::vector<SgDirectedGraphEdge*>::iterator iter;
  for(iter = in_edges.begin(); iter != in_edges.end(); iter++)
  {
    //remove not mpi_nodes from vector
    CFGEdgeAttribute<bool>* mpi_info_attr = dynamic_cast<CFGEdgeAttribute<bool>*>((*iter)->getAttribute("mpi_info"));
    if(mpi_info_attr)
      mpi_in_edges.push_back(*iter);
  }
  return std::vector<SgDirectedGraphEdge*>(mpi_in_edges.begin(), mpi_in_edges.end());
}

} // end of namespace MpiAnalysis
