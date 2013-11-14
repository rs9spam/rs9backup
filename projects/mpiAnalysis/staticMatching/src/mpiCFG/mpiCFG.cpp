using namespace std;

#include "sage3basic.h"
#include "CallGraph.h"
#include "DataflowCFG.h"
#include "mpiCFG/mpiCFG.h"
#include "rankAnalysis/RankLattice.h"
#include <boost/foreach.hpp>
#include <vector>
#include <set>

namespace MpiAnalysis
{
#if 0
=====================================================================================
void MPICFG::addMPIEdge(CFGNode from, CFGNode to, std::vector<CFGEdge>& result) {
  // Creates a CFG edge and adding appropriate labels.

}
#endif

//===================================================================================
void MPICFG::build(){
  //TODO that is not true .... why should start_ be an mpi_project_ ....
  mpi_project_ = start_;
  // If the start node is an SgProject, build the CFG from main.
  if(isSgProject(start_)) {
    SgFunctionDeclaration* main_def_decl = SageInterface::findMain(start_);
    if(main_def_decl == NULL)
      ROSE_ASSERT(!"\nCannot build CFG for project without main function");
    SgFunctionDefinition* main_def = main_def_decl->get_definition();
    if(main_def == NULL)
      ROSE_ASSERT (!"Cannot build CFG for project without main function");
    start_ = main_def;
  }
  buildMPIICFG();
}

//===================================================================================
void MPICFG::buildCFG()
{
  CFG::buildFullCFG();

  // Use buildFilteredCFG from MPICFG since the one from CFG pushes wrong CFG nodes
  // to all_nodes node collection.
//  MPICFG::buildFilteredCFG();
}

//===================================================================================
void MPICFG::buildMPIICFG()
{
  buildCFG();
  buildMPISend();
  buildMPIRecv();
  buildFullMPIMatchSet();
//TODO: buildMPIPossibleMatchSet();

  refineConstantMatch();

  addMPIEdgestoICFG();


  //pruneCfgUsingSimpleConstantPropagation ... added to the constant Match refinement
  //TODO: implement this functions ...


  //developeProcess_sets:
  //start a constant propagation like functionality to create another analysis
  //which adds lattices with process set information to every mpi node.

  //refineSourceParameter()   //process sets required
}

//===================================================================================
void MPICFG::buildFullCFG()
{
	std::set<VirtualCFG::CFGNode> explored;
	graph_ = new SgIncidenceDirectedGraph;
	ClassHierarchyWrapper classHierarchy(SageInterface::getProject());

	CFGNode start;
	start = start_->cfgForBeginning();

	buildCFG(start, all_nodes_, explored, &classHierarchy);
}

//===================================================================================
void MPICFG::buildFilteredCFG()
{
    all_nodes_.clear();
    clearNodesAndEdges();

    std::set<VirtualCFG::InterestingNode> explored;
    std::map<VirtualCFG::InterestingNode, SgGraphNode*> all_nodes;

    graph_ = new SgIncidenceDirectedGraph;

    if (SgProject* project = isSgProject(start_))
    {
        Rose_STL_Container<SgNode*> functions =
            NodeQuery::querySubTree(project, V_SgFunctionDefinition);
        for (Rose_STL_Container<SgNode*>::const_iterator i = functions.begin();
             i != functions.end();
             ++i)
        {
            SgFunctionDefinition* proc = isSgFunctionDefinition(*i);
            if (proc)
            {
                CFG::buildCFG<VirtualCFG::InterestingNode,
                              VirtualCFG::InterestingEdge>
                    (VirtualCFG::makeInterestingCfg(proc), all_nodes, explored);
            }
        }
    }
    else
        CFG::buildCFG<VirtualCFG::InterestingNode, VirtualCFG::InterestingEdge>
            (VirtualCFG::makeInterestingCfg(start_), all_nodes, explored);

    typedef std::pair<VirtualCFG::InterestingNode, SgGraphNode*> pair_t;
    BOOST_FOREACH (const pair_t& p, all_nodes)
        all_nodes_[VirtualCFG::CFGNode(p.first.getNode(),
                                       p.first.getIndex())] = p.second;
}

//===================================================================================
void addEdge(CFGNode from, CFGNode to, std::vector<CFGEdge>& result) {
  // Makes a CFG edge, adding appropriate labels
  SgNode* fromNode = from.getNode();
  unsigned int fromIndex = from.getIndex();
  SgNode* toNode = to.getNode();

  // Exit early if the edge should not exist because of a control flow discontinuity
  if (fromIndex == 1 && (isSgGotoStatement(fromNode)
      || isSgBreakStmt(fromNode) || isSgContinueStmt(fromNode))) {
    return;
  }
  if (isSgReturnStmt(fromNode) && toNode == fromNode->get_parent()) {
    SgReturnStmt* rs = isSgReturnStmt(fromNode);
    if (fromIndex == 1 || (fromIndex == 0 && !rs->get_expression()))
      return;
  }
  if (isSgStopOrPauseStatement(fromNode) && toNode == fromNode->get_parent()) {
    SgStopOrPauseStatement* sps = isSgStopOrPauseStatement(fromNode);
    if (fromIndex == 0 && sps->get_stop_or_pause() == SgStopOrPauseStatement::e_stop)
      return;
  }
  if (fromIndex == 1 && isSgSwitchStatement(fromNode) &&
      isSgSwitchStatement(fromNode)->get_body() == toNode) return;

  // Create the edge
  result.push_back(CFGEdge(from, to));
}

//===================================================================================
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
          CallTargetSet::getDefinitionsForExpression(
              isSgExpression(sgnode), classHierarchy, defs);
          if (defs.size() == 0) {
            outEdges = n.outEdges();
          } else {
            BOOST_FOREACH(SgFunctionDefinition* def, defs) {
              addEdge(n, def->cfgForBeginning(), outEdges);
              addEdge(def->cfgForEnd(), CFGNode(sgnode, idx+1), outEdges);
            }
          }
    }
    else {
      outEdges = n.outEdges();
    }

    std::set<CFGNode> targets;
    BOOST_FOREACH(const CFGEdge& edge, outEdges)
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
            to->addNewAttribute("info",
                                new CFGNodeAttribute(tar.getIndex(),
                                graph_));
            all_nodes[tar] = to;
            graph_->addNode(to);
        }

        SgDirectedGraphEdge* new_edge = new SgDirectedGraphEdge(from, to);
        new_edge->addNewAttribute("info", new CFGEdgeAttribute<CFGEdge>(edge));
        graph_->addDirectedEdge(new_edge);
    }
    BOOST_FOREACH(const CFGNode& target, targets)
        buildCFG(target, all_nodes, explored, classHierarchy);
}

//===================================================================================
void MPICFG::buildMPISend()
{
  BOOST_FOREACH( pair_n p, all_nodes_) {
    if(isSgFunctionCallExp((p.first).getNode())) {
//      if( ((p.first).getIndex() == 1)  && MpiUtils::isMPISend((p.first).getNode())) {
      if( ((p.first).isInteresting())  && MpiUtils::isMPISend((p.first).getNode())) {
//      if( ((p.first).getIndex() == 0)  && MpiUtils::isMPISend((p.first).getNode())) {
        //mpi_send_nodes_[p.first] = p.second;
        pair_n new_p = pair<VirtualCFG::CFGNode, SgGraphNode*>(p.first,p.second);
        mpi_send_nodes_.insert(new_p);
        //TODO: remove debug output
//        std::cerr << endl
//                  << "SIZE MPI SEND NODES: "
//                  << mpi_send_nodes_.size();
      }
    }
  }
}

//===================================================================================
void MPICFG::buildMPIRecv()
{
  BOOST_FOREACH( pair_n p, all_nodes_) {
    if(isSgFunctionCallExp((p.first).getNode())) {
//      if( ((p.first).getIndex() == 1)  && MpiUtils::isMPIRecv((p.first).getNode())) {
      if( ((p.first).isInteresting()) && MpiUtils::isMPIRecv((p.first).getNode())) {
//      if( ((p.first).getIndex() == 0) && MpiUtils::isMPIRecv((p.first).getNode())) {
        mpi_recv_nodes_[p.first] = p.second;
        //TODO: remove debug output
//        std::cerr << endl
//                  << "SIZE MPI RECV NODES: "
//                  << mpi_recv_nodes_.size();
      }
    }
  }
}

//===================================================================================
bool MPICFG::buildFullMPIMatchSet()
{
  BOOST_FOREACH(pair_n p_send, mpi_send_nodes_)
    BOOST_FOREACH(pair_n p_recv, mpi_recv_nodes_)
      mpi_connections_.insert( pair_cfg_node(p_send.first,p_recv.first));

  return true;
}

//===================================================================================
void MPICFG::addMPIEdgestoICFG()
{
  BOOST_FOREACH( pair_cfg_node c, mpi_connections_) {
    SgDirectedGraphEdge* new_graph_edge =
              new SgDirectedGraphEdge(all_nodes_[c.first],
                                      all_nodes_[c.second], "anyname");

    CFGEdge new_cfg_edge = CFGEdge((c.first), (c.second));

    //std::cerr << new_cfg_edge.toString() << endl;
    CFGEdgeAttribute<CFGEdge>* new_cfg_edge_attr
                                      = new CFGEdgeAttribute<CFGEdge>(new_cfg_edge);
    new_graph_edge->addNewAttribute("info", new_cfg_edge_attr);

    CFGEdgeAttribute<bool>* mpi_info_attr = new CFGEdgeAttribute<bool>(true);
    new_graph_edge->addNewAttribute("mpi_info", mpi_info_attr);

    //Add edge to MPI_ICFG graph
    graph_->addDirectedEdge(new_graph_edge);
//TODO: remove
//    std::cerr << endl << "Add MPI Edge...";
  }
//  //create NewAttribute for SgNode (for send nodes) (class MPIInfo)
//  for(map<CFGNode, SgGraphNode*>::iterator iter1 = mpi_send_nodes_.begin();
//      iter1 != mpi_send_nodes_.end(); iter1++)
//  {
//    MPIInfo mpi_info = new MPIInfo();
//  }
//  //create NewAttribute for SgNode (for Recv nodes) (class MPIInfo)
//  for(map<CFGNode, SgGraphNode*>::iterator iter2 = mpi_recv_nodes_.begin();
//      iter2 != mpi_recv_nodes_.end(); iter2++)
//  {
//
//  }

//  BOOST_FOREACH( pair_n p_send, mpi_send_nodes_) {
//    BOOST_FOREACH( pair_n p_recv, mpi_recv_nodes_) {
//      SgDirectedGraphEdge* new_graph_edge =
//          new SgDirectedGraphEdge(p_send.second, p_recv.second, "anyname");
//      CFGEdge new_cfg_edge = CFGEdge((p_send.first), (p_recv.first));
//      //std::cerr << new_cfg_edge.toString() << endl;
//      CFGEdgeAttribute<CFGEdge>* new_cfg_edge_attr =
//                                   new CFGEdgeAttribute<CFGEdge>(new_cfg_edge);
//      new_graph_edge->addNewAttribute("info", new_cfg_edge_attr);
//      CFGEdgeAttribute<bool>* mpi_info_attr = new CFGEdgeAttribute<bool>(true);
//      new_graph_edge->addNewAttribute("mpi_info", mpi_info_attr);
//
//      //Add edge to MPI_ICFG graph
//      graph_->addDirectedEdge(new_graph_edge);
//
//      //graph_->removeDirectedEdge(new_graph_edge);
//      //graph_->addDirectedEdge(iter1->second, iter2->second, "mpi_info");
//
//      std::cerr << endl << "Add MPI Edge...";
//    }
//  }
}

//===================================================================================
void MPICFG::refineConstantMatch()
{
//  std::cerr << endl << "Refine: Constant Argument Matching" << "\n";
//  BOOST_FOREACH(pair_cfg_node p, mpi_connections_){
//    if( !constTypeMatch(all_nodes_[p.first],all_nodes_[p.second])
//        || !constSizeMatch(all_nodes_[p.first],all_nodes_[p.second])
//        || !constTagMatch(all_nodes_[p.first],all_nodes_[p.second])
//        || !constCommWorldMatch(all_nodes_[p.first],all_nodes_[p.second]))

//  bool finished = false;
//  while(finished) {

    Conn_It e_it = mpi_connections_.begin();
    while(e_it != mpi_connections_.end()) {
      if( !constTypeMatch(all_nodes_[(*e_it).first],all_nodes_[(*e_it).second])
          || !constSizeMatch(all_nodes_[(*e_it).first],all_nodes_[(*e_it).second])
          || !constTagMatch(all_nodes_[(*e_it).first],all_nodes_[(*e_it).second])
          || !constCommWorldMatch(all_nodes_[(*e_it).first],all_nodes_[(*e_it).second]))
      {
#if 1
        Conn_It save_it = e_it;
        e_it++;
        mpi_connections_.erase(save_it);

//        Conn_It save_it = e_it;
//        save_it++;
//        mpi_connections_.erase(e_it);
//        e_it = save_it;
#else
        e_it++;
#endif
        //TODO:: Remove debug outptu.

        std::cerr << "\nOOO removing MPI Edge because of miss match";
      }
      else
        e_it++;
    }
//  }

//  map<CFGNode, SgGraphNode*>::iterator node_iter;
//  std::vector<SgDirectedGraphEdge*>::iterator edge_iter;
//  for(node_iter = mpi_send_nodes_.begin();
//      node_iter != mpi_send_nodes_.end();
//      node_iter++)
//  {
//    std::vector<SgDirectedGraphEdge*> out_edges = mpiOutEdges((node_iter->second));
//    for(edge_iter = out_edges.begin(); edge_iter != out_edges.end(); edge_iter++)
//    {
//      if(!constTypeMatch((*edge_iter)->get_from(),(*edge_iter)->get_to())
////         || !constSizeMatch((*edge_iter)->get_from(),(*edge_iter)->get_to())
//         ||!constTagMatch((*edge_iter)->get_from(),(*edge_iter)->get_to())
//         ||!constCommWorldMatch((*edge_iter)->get_from(),(*edge_iter)->get_to()))
//      {
//        removeMPIEdge(*edge_iter);
//        std::cerr << "removing MPI Edge because of miss match\n";
//      }
//    }
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
//    tempvector = mpiOutEdges(tempgn);
//    for(sgiter = tempvector.begin(); sgiter != tempvector.end(); sgiter++)
//    {
//      std::cerr << "##test for MPI" << endl;//sgiter->toString() << endl;
//    }
//
////    vector<CFGEdge> oedges = tempnode.outEdges();
//    for(edge_iter = oedges.begin(); edge_iter != oedges.end(); edge_iter++)
//    {
//      std::cerr<< "\n out edges  \n" << edge_iter->source().toString() << endl
//          << edge_iter->target().toString() << endl << endl;
//    }
//  }
}

//===================================================================================
void MPICFG::removeMPIEdge(SgDirectedGraphEdge* edge)
{
  std::cerr << endl << "#### WATCH OUT, PROBLEM WITH REMOVEING EDGES FROM GRAPH";
  if(graph_->removeDirectedEdge(edge))
    std::cerr << "Successfully removed directed edge from graph\n";
  else
    std::cerr << "This edge did not exist! \n";
}

//===================================================================================
SgExpression* getExpAt(SgExpressionPtrList argsptr ,unsigned int arg_nr)
{
  SgExpressionPtrList::iterator exp_iter;

  if(argsptr.size() >= arg_nr)
  {
    exp_iter = argsptr.begin();

    for(unsigned int i=1; i < arg_nr; i++)
      exp_iter++;

    SgExpression* exp = (*exp_iter);
    return exp;
  }
  return NULL;
}

//===================================================================================
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
//    SgExpressionPtrList fromargsptr =
//      isSgFunctionCallExp(from.getNode())->get_args()->get_expressions();
//    SgExpressionPtrList::iterator from_exp_iter;
//    SgExpressionPtrList toargsptr =
//      isSgFunctionCallExp(to.getNode())->get_args()->get_expressions();
//    SgExpressionPtrList::iterator to_exp_iter;
//
//    if(fromargsptr.size() >= MPI_NUM_SEND_EXP
//       && toargsptr.size() >= MPI_NUM_RECV_EXP)
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
////      std::cerr << "CLASS: " << (from_exp)->sage_class_name() << endl
////                << "CLASS: " << (to_exp)->sage_class_name() << endl
////                << "TYPE: "
////                << (from_exp)->get_type()->unparseToString() << endl
////                << "TYPE: "
////                << (to_exp)->get_type()->unparseToString() << endl;
//
//      if(isSgVarRefExp(from_exp) && isSgVarRefExp(to_exp))
//      {
////        std::cerr << "Symbol name: "
////                  << isSgVarRefExp(from_exp)->get_symbol()->get_name().str()
////                  << endl
////                  << "Symbol name: "
////                  << isSgVarRefExp(to_exp)->get_symbol()->get_name().str()
////                  << endl;
//        //TODO ... #define a value for that .. check if it's always ompi_mpi ...
//        string ompi = "ompi_mpi";
//        string from_s = isSgVarRefExp(from_exp)->get_symbol()->get_name().str();
//        string to_s = isSgVarRefExp(to_exp)->get_symbol()->get_name().str();
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

//===================================================================================
bool MPICFG::constTypeMatch(SgGraphNode* send_node, SgGraphNode* recv_node)
{
  CFGNode from = getCFGNode(send_node);
  CFGNode to = getCFGNode(recv_node);
  if ((from == NULL) || (to == NULL))
    ROSE_ASSERT (!"Cannot find the SgGraphNode in map");

  if(isSgFunctionCallExp(from.getNode()) && isSgFunctionCallExp(to.getNode()))
  {
    SgExpressionPtrList fromargsptr =
        isSgFunctionCallExp(from.getNode())->get_args()->get_expressions();
    SgExpressionPtrList toargsptr =
        isSgFunctionCallExp(to.getNode())->get_args()->get_expressions();

    SgExpression* from_exp = getExpAt(fromargsptr ,MPI_TYPE_ARG);
    SgExpression* to_exp = getExpAt(toargsptr ,MPI_TYPE_ARG);
    if((from_exp == NULL) || (to_exp == NULL))
      ROSE_ASSERT (!"Cannot find type SgExpression*");
    while(isSgUnaryOp(from_exp))
      from_exp = isSgUnaryOp(from_exp)->get_operand();
    while(isSgUnaryOp(to_exp))
      to_exp = isSgUnaryOp(to_exp)->get_operand();
//    std::cerr << "CLASS: " << (from_exp)->sage_class_name() << endl;
//    std::cerr << "CLASS: " << (to_exp)->sage_class_name() << endl;
//    std::cerr << "TYPE: " << (from_exp)->get_type()->unparseToString() << endl;
//    std::cerr << "TYPE: " << (to_exp)->get_type()->unparseToString() << endl;

    if(isSgVarRefExp(from_exp) && isSgVarRefExp(to_exp))
    {
//      std::cerr << "Symbol name: "
//                << isSgVarRefExp(from_exp)->get_symbol()->get_name().str() << endl;
//      std::cerr << "Symbol name: "
//                << isSgVarRefExp(to_exp)->get_symbol()->get_name().str() << endl;
      //TODO ... #define a value for that .. check if it's always ompi_mpi ...
      std::string ompi = "ompi_mpi";
      std::string from_s = isSgVarRefExp(from_exp)->get_symbol()->get_name().str();
      std::string to_s = isSgVarRefExp(to_exp)->get_symbol()->get_name().str();
      if(from_s == to_s && from_s.substr(0,ompi.size()) == ompi)
      {
//        std::cerr << "Send and Recv have same constant type: " << from_s << endl;
        //TODO ... set Type checked Flag at communication edge.
        return true;
      }
      if(from_s.substr(0,ompi.size()) == ompi && to_s.substr(0,ompi.size()) == ompi)
      {
//        std::cerr << "Send and Recv have different constant types: "
//                  << from_s << " and " << to_s << endl;
        return false;
      }
    }
  }
  //The default return in case of uncertanty is true.
  return true;
}

//===================================================================================
bool MPICFG::constSizeMatch(SgGraphNode* send_node, SgGraphNode* recv_node)
{
  CFGNode from = getCFGNode(send_node);
  CFGNode to = getCFGNode(recv_node);
  if ((from == NULL) || (to == NULL))
    ROSE_ASSERT (!"Cannot find the SgGraphNode in the map");

  if(isSgFunctionCallExp(from.getNode()) && isSgFunctionCallExp(to.getNode()))
  {
    SgExpressionPtrList fromargsptr =
        isSgFunctionCallExp(from.getNode())->get_args()->get_expressions();
    SgExpressionPtrList toargsptr =
        isSgFunctionCallExp(to.getNode())->get_args()->get_expressions();

    SgExpression* from_exp = getExpAt(fromargsptr ,MPI_SIZE_ARG);
    SgExpression* to_exp = getExpAt(toargsptr ,MPI_SIZE_ARG);
    if( (from_exp == NULL) || (to_exp == NULL) )
      ROSE_ASSERT (!"Cannot find size SgExpression*");

    if(isSgIntVal(from_exp) && isSgIntVal(to_exp))
    {
      int from_i = isSgIntVal(from_exp)->get_value();
      int to_i = isSgIntVal(to_exp)->get_value();
      return (from_i <= to_i)? true : false;
//      if(from_i == to_i)
//      {
//        std::cerr << "Send and Recv have same constant size: " << from_i << endl;
//        //TODO ... set Type checked Flag at communication edge.
//        return true;
//      }
//      else
//      {
//        std::cerr << "Send and Recv have different constant size: "
//                  << from_i << " and " << to_i << endl;
//        return false;
//      }
    }
    if(isSgIntVal(from_exp))//check the values from initial Constant propagation
    {
//      std::cerr << "check for constant propagation value" << endl;
      while(isSgUnaryOp(to_exp))
        to_exp = isSgUnaryOp(to_exp)->get_operand();

      if(isSgVarRefExp(to_exp) && hasConstValue(isSgNode(to_exp)))
      {
        int from_i = isSgIntVal(from_exp)->get_value();
        int to_i = getConstPropValue(to_exp);
        return (from_i <= to_i) ? true : false;
      }
      return true;
    }
    if(isSgIntVal(to_exp))
    {
//      std::cerr << "check for constant propagation value" << endl;
      while(isSgUnaryOp(from_exp))
        from_exp = isSgUnaryOp(from_exp)->get_operand();

      if(isSgVarRefExp(from_exp))
      {
        if(hasConstValue(isSgNode(from_exp)))
        {
          //compare const values
          int from_i = getConstPropValue(from_exp);
          int to_i = isSgIntVal(to_exp)->get_value();
          return (from_i <= to_i) ? true : false;
//          if(from_i == to_i)
//          {
//            std::cerr << "Send and Recv have same constant size: "
//                      << from_i << endl;
//            //TODO ... set Type checked Flag at communication edge.
//            return true;
//          }
//          else
//          {
//            std::cerr << "Send and Recv have different constant size: "
//                      << from_i << " and " << to_i << endl;
//            return false;
//          }
        }
      }
      return true;
    }

    while(isSgUnaryOp(from_exp))
      from_exp = isSgUnaryOp(from_exp)->get_operand();

    while(isSgUnaryOp(to_exp))
      to_exp = isSgUnaryOp(to_exp)->get_operand();

    if(isSgVarRefExp(from_exp) && isSgVarRefExp(to_exp) &&
       hasConstValue(isSgNode(from_exp)) && hasConstValue(isSgNode(to_exp)))
    {
      int from_i = getConstPropValue(from_exp);
      int to_i = getConstPropValue(to_exp);
      return (from_i <= to_i) ? true : false;
    }
  }
  return true;
}


//===================================================================================
bool MPICFG::constTagMatch(SgGraphNode* send_node, SgGraphNode* recv_node)
{
  CFGNode from = getCFGNode(send_node);
  CFGNode to = getCFGNode(recv_node);
  if ((from == NULL) || (to == NULL))
    ROSE_ASSERT (!"Cannot find the SgGraphNode in the map");

  if(isSgFunctionCallExp(from.getNode()) && isSgFunctionCallExp(to.getNode()))
  {
    SgExpressionPtrList fromargsptr =
        isSgFunctionCallExp(from.getNode())->get_args()->get_expressions();
    SgExpressionPtrList toargsptr =
        isSgFunctionCallExp(to.getNode())->get_args()->get_expressions();

    SgExpression* from_exp = getExpAt(fromargsptr ,MPI_TAG_ARG);
    SgExpression* to_exp = getExpAt(toargsptr ,MPI_TAG_ARG);
    if( (from_exp == NULL) || (to_exp == NULL) )
      ROSE_ASSERT (!"Cannot find tag SgExpression*");

    //Both values are Integer Constants.
    if(isSgIntVal(from_exp) && isSgIntVal(to_exp))
    {
      int from_i = isSgIntVal(from_exp)->get_value();
      int to_i = isSgIntVal(to_exp)->get_value();
      return (from_i == to_i) ? true : false;
//      if(from_i == to_i)
//      {
//        std::cerr << "Send and Recv have same constant tag: " << from_i << endl;
//        //TODO ... set Type checked Flag at communication edge.
//        return true;
//      }
//      if(from_i != to_i)
//      {
//        std::cerr << "Send and Recv have different constant tags: "
//                  << from_i << " and " << to_i << endl;
//        return false;
//      }
    }
    //If send is MPI_ANY_TAG
    if(isSgMinusOp(from_exp))
      if(isSgIntVal(isSgMinusOp(from_exp)->get_operand()))
        if(isSgIntVal(isSgMinusOp(from_exp)->get_operand())->get_value() == 1)
        {
//          std::cerr << "Send with MPI_ANY_TAG: " << endl;
          //TODO ... set Type checked Flag at communication edge.
          return true;
        }
    //If recv is MPI_ANY_TAG
    if(isSgMinusOp(to_exp))
      if(isSgIntVal(isSgMinusOp(to_exp)->get_operand()))
        if(isSgIntVal(isSgMinusOp(to_exp)->get_operand())->get_value() == 1)
        {
//          std::cerr << "Recv with MPI_ANY_TAG: " << endl;
          //TODO ... set Type checked Flag at communication edge.
          return true;
        }
    //from_exp is int to_exp is int by constant propagation (check to_i == -1)
    if(isSgIntVal(from_exp))
    {
      while(isSgUnaryOp(to_exp))
        to_exp = isSgUnaryOp(to_exp)->get_operand();

      if(isSgVarRefExp(to_exp))
        if(hasConstValue(isSgNode(to_exp)))
        {
          //compare const values
          int from_i = isSgIntVal(from_exp)->get_value();
          int to_i = getConstPropValue(to_exp);
          if(to_i == -1)
            return true;
          return (from_i == to_i) ? true : false;
        }
      return true;
    }
    //to_exp is int from_exp is int by constant propagation (check from_i == -1)
    if(isSgIntVal(to_exp))
    {
//      std::cerr << "check for constant propagation value" << endl;
      while(isSgUnaryOp(from_exp))
        from_exp = isSgUnaryOp(from_exp)->get_operand();

      if(isSgVarRefExp(from_exp))
        if(hasConstValue(isSgNode(from_exp)))
        {
          //compare const values
          int from_i = getConstPropValue(from_exp);
          int to_i = isSgIntVal(to_exp)->get_value();
          if(from_i == -1)
            return true;
          return (from_i == to_i) ? true : false;
        }
      return true;
    }
    //both are int by constant propagation
    while(isSgUnaryOp(from_exp))
      from_exp = isSgUnaryOp(from_exp)->get_operand();

    while(isSgUnaryOp(to_exp))
      to_exp = isSgUnaryOp(to_exp)->get_operand();

    if(isSgVarRefExp(from_exp) && isSgVarRefExp(to_exp) &&
       hasConstValue(isSgNode(from_exp)) && hasConstValue(isSgNode(to_exp)))
    {
      int from_i = getConstPropValue(from_exp);
      int to_i = getConstPropValue(to_exp);
      return (from_i == to_i) ? true : false;
    }
  }
  return true;
}

//===================================================================================
bool MPICFG::constCommWorldMatch(SgGraphNode* send_node, SgGraphNode* recv_node)
{
  CFGNode from = getCFGNode(send_node);
  CFGNode to = getCFGNode(recv_node);
  if ((from == NULL) || (to == NULL))
    ROSE_ASSERT (!"Cannot find the SgGraphNode in map");

  if(isSgFunctionCallExp(from.getNode()) && isSgFunctionCallExp(to.getNode()))
  {
    SgExpressionPtrList fromargsptr =
        isSgFunctionCallExp(from.getNode())->get_args()->get_expressions();
    SgExpressionPtrList toargsptr =
        isSgFunctionCallExp(to.getNode())->get_args()->get_expressions();

    SgExpression* from_exp = getExpAt(fromargsptr ,MPI_COMM_WORLD_ARG);
    SgExpression* to_exp = getExpAt(toargsptr ,MPI_COMM_WORLD_ARG);
    if((from_exp == NULL) || (to_exp == NULL))
      ROSE_ASSERT (!"Cannot find CommWorld SgExpression*");
    while(isSgUnaryOp(from_exp))
      from_exp = isSgUnaryOp(from_exp)->get_operand();
    while(isSgUnaryOp(to_exp))
      to_exp = isSgUnaryOp(to_exp)->get_operand();

    if(isSgVarRefExp(from_exp) && isSgVarRefExp(to_exp))
    {
      //TODO ... #define a value for that .. check if it's always ompi_mpi ...
      std::string ompi = "ompi_mpi";
      std::string from_s = isSgVarRefExp(from_exp)->get_symbol()->get_name().str();
      std::string to_s = isSgVarRefExp(to_exp)->get_symbol()->get_name().str();
      if(from_s == to_s && from_s.substr(0,ompi.size()) == ompi)
      {
//        std::cerr << "Send and Recv have same constant type: " << from_s << endl;
        //TODO ... set Type checked Flag at MPI Send and MPI Recv nodes.
        return true;
      }
      if(from_s.substr(0,ompi.size()) == ompi && to_s.substr(0,ompi.size()) == ompi)
      {
//        std::cerr << "Send and Recv have different constant types: "
//                  << from_s << " and " << to_s << endl;
        return false;
      }
    }
  }
  return true;
}

//===================================================================================
//
//                  Constant Propagation Lattice helper functions
//
//===================================================================================

//===================================================================================
bool MPICFG::hasConstValue(SgNode* node)
{
  ROSE_ASSERT(node != NULL);

  if(isSgVarRefExp(node))
  {
    string var = node->unparseToString();
    VarsExprsProductLattice* lattice =
      dynamic_cast <VarsExprsProductLattice *>
        (NodeState::getLatticeAbove(const_prop_, node,0)[0]);

    ConstantPropagationLattice* varlattice =
        dynamic_cast <ConstantPropagationLattice*>
                              (lattice->getVarLattice(varID(node)));

    if(varlattice != NULL)
    {
      if(varlattice->getLevel() == (short)2)
        return true;
    }
  }
  return false;
}

//===================================================================================
int MPICFG::getConstPropValue(SgNode* node)
{
  ROSE_ASSERT(node != NULL);

  if(isSgVarRefExp(node)) {
    string var = node->unparseToString();
    VarsExprsProductLattice* lattice =
      dynamic_cast <VarsExprsProductLattice *>
        (NodeState::getLatticeAbove(const_prop_, node,0)[0]);

    ConstantPropagationLattice* varlattice =
        dynamic_cast <ConstantPropagationLattice*>(lattice->getVarLattice(varID(node)));

    if(varlattice != NULL) {
      if(varlattice->getLevel() == (short)2) {
        return varlattice->getValue();
      }
    }
  }
  return 0;
}

//===================================================================================
bool MPICFG::hasRankInfo(SgGraphNode* node)
{
  CFGNode cfg_n = toCFGNode(node);
  SgNode* n = cfg_n.getNode();
  unsigned int idx = cfg_n.getIndex();
  SgNode* n2;
  unsigned int idx2;

  for(std::vector<DataflowNode>::iterator it = ra_nodes_.begin();
      it != ra_nodes_.end();
      ++it)
  {
    n2 = it->getNode();
    idx2 = it->getIndex();
    if(n == n2 && idx == idx2)
      return true;
  }

  return false;
}


//===================================================================================
string MPICFG::getRankPSetString(SgGraphNode* node)
{
//  CFGNode n = toCFGNode(node);
//
//  //NodeState *state =  NodeState::getNodeState(node.getNode(), node.getIndex());
//  DataflowNode dfn(n, defaultFilter);
//  NodeState* state = NodeState::getNodeState(dfn, n.getIndex());
  CFGNode cfg_n = toCFGNode(node);
  SgNode* n = cfg_n.getNode();
  unsigned int idx = cfg_n.getIndex();
  SgNode* n2;
  unsigned int idx2;
  NodeState* state = NULL;

  for(std::vector<DataflowNode>::iterator it = ra_nodes_.begin();
      it != ra_nodes_.end();
      ++it)
  {
    n2 = it->getNode();
    idx2 = it->getIndex();
    if(n == n2 && idx == idx2)
    {
      state = NodeState::getNodeState(*it, 0);
      break;
    }
  }

  if(state != NULL)
  {
    if(state->isInitialized(rank_analysis_))
    {
      std::vector<Lattice*> lv = state->getLatticeBelow(rank_analysis_);
      if(lv.empty())
        return "ERROR lattice vector == NULL!";
      Lattice* lattice = *(lv.begin());
      if(lattice == NULL)
        return "ERROR lattice == NULL";
      RankLattice* rank_lattice = dynamic_cast<RankLattice*>(lattice);
      if(rank_lattice != NULL)
      {
        return rank_lattice->psetsToString();
      }
    }
  }

#if 0
  if(state->isInitialized(rank_analysis_))
  {
    std::cerr << "#";
    return " good ";
  }
#endif

//  if(state->isInitialized(rank_analysis_))
//  {
//    std::vector<Lattice*> lv = state->getLatticeAbove(rank_analysis_);
//    if(lv.empty())
//      return "ERROR lattice vector == NULL!";
//    Lattice* lattice = *(lv.begin());
//    if(lattice == NULL)
//      return "ERROR lattice == NULL";
//    RankLattice* rank_lattice = dynamic_cast<RankLattice*>(lattice);
////  RankLattice* lattice = dynamic_cast <RankLattice*>   (NodeState::getLatticeAbove(this->rank_analysis_, node, 0)[0]);
////  LiveVarsLattice* lAb = dynamic_cast<LiveVarsLattice*>  (*(state.getLatticeAbove(ldva).begin()));
////  if(lattice == NULL)
////    return "ERROR_1";
////
////  RankLattice* ranklattice =
////                       dynamic_cast <RankLattice*> (lattice->getVarLattice(varID(node)));
//
//    if(rank_lattice != NULL)
//      return rank_lattice->toString();
//  }
//  std::cerr << "ERROR Rank Lattice == NULL! NODE: " << node.toString();
  std::cerr << "_";
  return "";
}

//===================================================================================
const CFGNode MPICFG::getCFGNode(SgGraphNode* node)
{
  BOOST_FOREACH(pair_n p, all_nodes_) {
    if(node == p.second)
      return p.first;
  }
  return NULL;
}

//===================================================================================
void MPICFG::setRankInfo(std::vector<DataflowNode> rn)
{
  this->ra_nodes_ = rn;
}

//===================================================================================
void MPICFG::setRankInfo(RankAnalysis* ra)
{
  this->rank_analysis_ = ra;
}

//===================================================================================
void MPICFG::setRankInfo(RankAnalysis* ra, std::vector<DataflowNode> rn)
{
  this->rank_analysis_ = ra;
  this->ra_nodes_ = rn;
}

//===================================================================================
void MPICFG::mpicfgToDot()
{
  SgFunctionDefinition* main_def;
  ROSE_ASSERT (isSgFunctionDefinition(start_));
  main_def = (SgFunctionDefinition*)start_;
  string file_name = StringUtility
      ::stripPathFromFileName(
          main_def->get_file_info()->get_filenameString());
  string dot_file_name = file_name
                        + "."
                        + main_def->get_declaration()->get_name()
                        +".mpiicfg.dot";

  mpicfgToDot(dot_file_name);
}

//===================================================================================
void MPICFG::mpicfgToDot(const std::string& file_name)
{
  SgFunctionDefinition* main_def;
  ROSE_ASSERT (isSgFunctionDefinition(start_));
  main_def = (SgFunctionDefinition*)start_;
//  cfgToDot(main_def, file_name);
  std::ofstream ofile(file_name.c_str(), std::ios::out);
  ofile << "digraph defaultName {\n";
  std::set<SgGraphNode*> explored;
  processNodes(ofile, cfgForBeginning(main_def), explored);
  ofile << "}\n";
}

//===================================================================================
void MPICFG::processNodes(std::ostream & o,
                       SgGraphNode* n,
                       std::set<SgGraphNode*>& explored)
{
    if (explored.count(n) > 0)
        return;
    explored.insert(n);

    printNodePlusEdges(o, n);

    std::set<SgDirectedGraphEdge*> out_edges = graph_->computeEdgeSetOut(n);
    BOOST_FOREACH (SgDirectedGraphEdge* e, out_edges)
              processNodes(o, e->get_to(), explored);

    std::set<SgDirectedGraphEdge*> in_edges = graph_->computeEdgeSetIn(n);
    BOOST_FOREACH (SgDirectedGraphEdge* e, in_edges)
              processNodes(o, e->get_from(), explored);
}

//===================================================================================
void MPICFG::printNodePlusEdges(std::ostream & o, SgGraphNode* node)
{
    printNode(o, node);

    std::set<SgDirectedGraphEdge*> out_edges = graph_->computeEdgeSetOut(node);
    BOOST_FOREACH (SgDirectedGraphEdge* e, out_edges)
        printEdge(o, e, false);
}

//===================================================================================
void MPICFG::printNode(std::ostream & o, SgGraphNode* node)
{
    CFGNode n = toCFGNode(node);

    std::string id = n.id();
    std::string nodeColor = "black";

    if (isSgStatement(n.getNode()))
        nodeColor = "blue";
    else if (isSgExpression(n.getNode()))
        nodeColor = "green";
    else if (isSgInitializedName(n.getNode()))
        nodeColor = "red";

    o << id;
    o << " [label=\""  << escapeString(n.toString());
    o << escapeString((hasRankInfo(node) ? "\n" : ""));
    o << escapeString((hasRankInfo(node) ? getRankPSetString(node) : ""));
    o << "\", color=\"" << nodeColor;
    o << "\", style=\"" << (n.isInteresting()? "solid" : "dotted");
    o << "\"];\n";
}

//===================================================================================
void MPICFG::mpiCommToDot()
{
  SgFunctionDefinition* main_def;
  ROSE_ASSERT (isSgFunctionDefinition(start_));
  main_def = (SgFunctionDefinition*)start_;
  string file_name = StringUtility
      ::stripPathFromFileName(
          main_def->get_file_info()->get_filenameString());
  string dot_file_name = file_name
                        + "."
                        + main_def->get_declaration()->get_name()
                        +".mpi_comm.dot";

  mpiCommToDot(dot_file_name);
}

//===================================================================================
void MPICFG::mpiCommToDot(const std::string& file_name)
{
  std::ofstream ofile(file_name.c_str(), std::ios::out);
  mpiPrintDotHeader(ofile);
  mpiPrintNodes(ofile);
  mpiPrintEdges(ofile);
  ofile << "}\n";
}

//===================================================================================
void MPICFG::mpiPrintDotHeader(std::ostream& o)
{
  o << "digraph MPI_COMMUNCATION_GRAPH {\n"
    << "rankdir=TB\n"
    << "node[style=\"filled\", color=\"white\", fontcolor=\"white\"]\n"
    << "edge[color=\"white\"]\n";

//    << "subgraph cluster0{\n"
//    << "  node[style=\"filled\", color=\"white\"];\n"
//    << "  style=\"filled\";\n"
//    << "  color=\"lightgray\";\n"
//    << "  ";
  //go through all send nodes and receive nodes and enter their line
  //numbers in a sorted integer vector.
  std::set<int> lines;
  SgLocatedNode* node;
  BOOST_FOREACH(pair_n p, mpi_send_nodes_) {
    if(isSgLocatedNode(p.first.getNode())) {
      node = (SgLocatedNode*)(p.first.getNode());
      if(node->get_file_info() != NULL) {
        lines.insert((node->get_file_info())->get_line());
      }
      else {
        std::cerr << "Node without file info!";
      }
    }
    else {
      std::cerr << "MPI Send SgNode is not an SgLocatedNode!";
    }
  }
  BOOST_FOREACH(pair_n p, mpi_recv_nodes_) {
    if(isSgLocatedNode(p.first.getNode())) {
      node = (SgLocatedNode*)(p.first.getNode());
      if(node->get_file_info() != NULL) {
        lines.insert((node->get_file_info())->get_line());
      }
      else {
        std::cerr << "Node without file info!";
      }
    }
    else {
      std::cerr << "MPI Send SgNode is not an SgLocatedNode!";
    }
  }
  BOOST_FOREACH(int x, lines) {
    o << x << " -> ";
  }
  o << "end;\n";
  o << "node[fontcolor=\"black\"]\n";
  o << "edge[color=\"black\"]\n";
//  o << "  label=\"lines\";\n}\n";
}

//===================================================================================
void MPICFG::mpiPrintNodes(std::ostream& o)
{
  BOOST_FOREACH(pair_n p, mpi_send_nodes_) {
    mpiPrintNode(o, p.first);
  }
  BOOST_FOREACH(pair_n p, mpi_recv_nodes_) {
    mpiPrintNode(o, p.first);
  }
}

//===================================================================================
void MPICFG::mpiPrintEdges(std::ostream& o)
{
  BOOST_FOREACH(pair_cfg_node p, mpi_connections_) {
    mpiPrintEdge(o, p.first, p.second);
  }
}

//===================================================================================
void MPICFG::mpiPrintNode(std::ostream& o, const VirtualCFG::CFGNode& node)
{
  std::string id = node.id();
  int line = ((node.getNode())->get_file_info())->get_line();
  std::string nodeColor = "black";
  std::string nodeStyle = "solid";

  if(MpiUtils::isMPISend(node.getNode())) {
    nodeColor = "blue";
    nodeStyle = "bold";
  }

  o << id
    << " [label=\"" << escapeString(node.toString()) << "\""
    << ", color=\"" << nodeColor << "\""
//    << ", style=\"" << (node.isInteresting()? "solid" : "dotted") << "\""
    << ", style=\"" << nodeStyle << "\""
    << "];\n";

  o << "{ rank=same; " << line << "; " << id << "; }\n";
}

//===================================================================================
void MPICFG::mpiPrintEdge(std::ostream& o,
                          const VirtualCFG::CFGNode& x,
                          const VirtualCFG::CFGNode& y)
{
  std::string id_x = x.id();
  std::string id_y = y.id();

  o << id_x << " -> " << id_y
    << " [label=\"\""
    << ", color=\"black\""
    << "];\n";
}

//===================================================================================//:TODO
std::vector<SgDirectedGraphEdge*> mpiOutEdges(SgGraphNode* node)
{
  std::vector<SgDirectedGraphEdge*> out_edges = outEdges(dynamic_cast<SgGraphNode*>(node));
  std::vector<SgDirectedGraphEdge*> mpi_out_edges;
  std::vector<SgDirectedGraphEdge*>::iterator iter;
  for(iter = out_edges.begin(); iter != out_edges.end(); iter++)
  {
    //remove not mpi_nodes from vector
    CFGEdgeAttribute<bool>* mpi_info_attr =
        dynamic_cast<CFGEdgeAttribute<bool>*>((*iter)->getAttribute("mpi_info"));
    if(mpi_info_attr)
      mpi_out_edges.push_back(*iter);
  }
  return std::vector<SgDirectedGraphEdge*>(mpi_out_edges.begin(), mpi_out_edges.end());
}

//===================================================================================//:TODO
std::vector<SgDirectedGraphEdge*> mpiInEdges(SgGraphNode* node)
{
  std::vector<SgDirectedGraphEdge*> in_edges = inEdges(dynamic_cast<SgGraphNode*>(node));
  std::vector<SgDirectedGraphEdge*> mpi_in_edges;
  std::vector<SgDirectedGraphEdge*>::iterator iter;
  for(iter = in_edges.begin(); iter != in_edges.end(); iter++)
  {
    //remove not mpi_nodes from vector
    CFGEdgeAttribute<bool>* mpi_info_attr =
        dynamic_cast<CFGEdgeAttribute<bool>*>((*iter)->getAttribute("mpi_info"));
    if(mpi_info_attr)
      mpi_in_edges.push_back(*iter);
  }
  return std::vector<SgDirectedGraphEdge*>(mpi_in_edges.begin(), mpi_in_edges.end());
}

} // end of namespace MpiAnalysis
