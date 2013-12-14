#include "sage3basic.h"
#include "CallGraph.h"
#include "DataflowCFG.h"
#include "mpiCFG/mpiCFG.h"
//#include "mpiCFG/MpiCommunication.h"
#include "rankAnalysis/RankLattice.h"
#include "rankAnalysis/rankAnalysis.h"
#include "loopAnalysis/loopAnalysis.h"
#include "rankMpiCallList/CallAnalysis.h"
#include "liveDeadVarAnalysis.h"
#include "constPropAnalysis/constantPropagationAnalysis.h"
#include <boost/foreach.hpp>
#include <vector>
#include <string>
#include <set>
#include <map>

namespace MpiAnalysis
{
  int MpiCFGDebugLevel = 0;

//===================================================================================

void MPICFG::build(){
  //TODO: that is not true .... why should start_ be an mpi_project_ ....
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
void MPICFG::buildMPIICFG()
{
  buildCFG(); //no MPI related functionality in this function.
  std::vector<CFGNode> all_nodes;
  std::map<CFGNode, SgGraphNode*>::iterator it;
  for(it = all_nodes_.begin(); it != all_nodes_.end(); ++it)
    all_nodes.push_back(it->first);
//  mpic_ = MpiCommunication(vec, d_f_nodes_);
  mpic_ = MpiCommunication(all_nodes,
                           const_prop_,
                           rank_analysis_,
                           loop_analysis_,
                           call_analysis_);
//  mpic_.setDataflowNodesCPA(cpa_nodes_);
//  mpic_.setDataflowNodesRA(ra_nodes_);
//  mpic_ = MpiCommunication(all_nodes_);
  mpic_.buildFullMPIMatchSet();
  mpic_.refineConstantMatch();
//TODO: buildMPIPossibleMatchSet();
  addMPIEdgestoICFG();
}

//===================================================================================
void MPICFG::buildCFG()
{
  if(this->is_filtered_)
    MPICFG::buildFilteredCFG();
  else
    MPICFG::buildFullCFG();

  // Use buildFilteredCFG from MPICFG since the one from CFG pushes wrong CFG nodes
  // to all_nodes node collection.
}

//===================================================================================
void MPICFG::buildFullCFG()
{
  CFG::buildFullCFG();
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

#if 0
//===================================================================================
void MPICFG::addEdge(CFGNode from, CFGNode to, std::vector<CFGEdge>& result) {
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
#endif

//===================================================================================
void MPICFG::addMPIEdgestoICFG()
{
  std::multimap<CFGNode, CFGNode> mpic = mpic_.getMpiConnections();
  std::multimap<CFGNode, CFGNode>::const_iterator it;
  for(it = mpic.begin(); it != mpic.end(); ++it)
  {
    SgDirectedGraphEdge* new_graph_edge =
              new SgDirectedGraphEdge(all_nodes_[it->first],
                                      all_nodes_[it->second], "anyname");

    CFGEdge new_cfg_edge = CFGEdge((it->first), (it->second));

    if(MpiCFGDebugLevel >= 3)
      std::cerr << new_cfg_edge.toString() << endl;
    CFGEdgeAttribute<CFGEdge>* new_cfg_edge_attr
                                      = new CFGEdgeAttribute<CFGEdge>(new_cfg_edge);
    new_graph_edge->addNewAttribute("info", new_cfg_edge_attr);

    CFGEdgeAttribute<bool>* mpi_info_attr = new CFGEdgeAttribute<bool>(true);
    new_graph_edge->addNewAttribute("mpi_info", mpi_info_attr);

    //Add edge to MPI_ICFG graph
    graph_->addDirectedEdge(new_graph_edge);
    if(MpiCFGDebugLevel >= 3)
      std::cerr << "\nAdded MPI Edge...";
  }
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
bool MPICFG::hasRankInfo(const CFGNode& node) const
{
#if 1
  if(rank_analysis_ == NULL)
    return false;
  NodeState* state = NULL;

  if(MpiUtils::hasDataflowNode(node, rank_analysis_->getDFNodes()))
  {
    state = NodeState::getNodeState(
        MpiUtils::getDataflowNode(node, rank_analysis_->getDFNodes()), 0);
    if(state != NULL)
      if(state->isInitialized(rank_analysis_))
        return true;
  }
  return false;
//  DataflowNode* dfn = MpiUtils::findDataflowNode(node, ra_nodes_);
//  if(dfn != NULL)
//  {
//    NodeState* state = NodeState::getNodeState(*dfn, 0);
//    if(state != NULL)
//      if(state->isInitialized(rank_analysis_))
//        return true;
//  }
//  return false;
#else
  SgNode* n = node.getNode();
  unsigned int idx = node.getIndex();
  SgNode* n2;
  unsigned int idx2;
  NodeState* state = NULL;

  std::vector<DataflowNode>::const_iterator it;
  for(it = ra_nodes_.begin(); it != ra_nodes_.end(); ++it)
  {
    n2 = it->getNode();
    idx2 = it->getIndex();
    if(n == n2 && idx == idx2)
    {
      state = NodeState::getNodeState(*it, 0);
      if(state != NULL)
        if(state->isInitialized(rank_analysis_))
         return true;
    }
  }
  return false;
#endif
}

//===================================================================================
string MPICFG::getRankPSetString(const CFGNode& node) const
{
#if 1
  if(rank_analysis_ == NULL)
    return "";

  NodeState* state = NULL;

  if(MpiUtils::hasDataflowNode(node, rank_analysis_->getDFNodes()))
    state = NodeState::getNodeState(
        MpiUtils::getDataflowNode(node, rank_analysis_->getDFNodes()), 0);
#else
  SgNode* n = node.getNode();
  unsigned int idx = node.getIndex();
  SgNode* n2;
  unsigned int idx2;
  NodeState* state = NULL;

  std::vector<DataflowNode>::const_iterator it;
  for(it = ra_nodes_.begin(); it != ra_nodes_.end(); ++it)
  {
    n2 = it->getNode();
    idx2 = it->getIndex();
    if(n == n2 && idx == idx2)
    {
      state = NodeState::getNodeState(*it, 0);
      break;
    }
  }
#endif

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

//  std::cerr << "_";
  return "";
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
    o << escapeString((hasRankInfo(n) ? "\n" : ""));
    o << escapeString((hasRankInfo(n) ? getRankPSetString(n) : ""));
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

  mpic_.mpiCommToDot(dot_file_name);
}

} /* end of namespace MpiAnalysis */
