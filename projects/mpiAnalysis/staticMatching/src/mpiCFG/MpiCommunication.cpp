/*
 * RefineMpiCommunication.cpp
 *
 *  Created on: Nov 18, 2013
 *      Author: ro
 */


#include "sage3basic.h"
#include "CallGraph.h"
#include "DataflowCFG.h"
#include "mpiCFG/MpiCommunication.h"
#include "rankAnalysis/RankLattice.h"
#include "loopAnalysis/LoopLattice.h"
#include "liveDeadVarAnalysis.h"
#include "constPropAnalysis/constantPropagationAnalysis.h"
#include <vector>
#include <map>
#include <set>
#include <string>

namespace MpiAnalysis
{

extern int MpiCFGDebugLevel;

//=======================================================================================
bool MpiCommunication::buildFullMPIMatchSet()
{
  determineMPIRecv();
  determineMPISend();

  std::vector<CFGNode>::iterator send_it;
  std::vector<CFGNode>::iterator recv_it;
  for(send_it = mpi_send_nodes_.begin(); send_it != mpi_send_nodes_.end(); ++send_it)
    for(recv_it = mpi_recv_nodes_.begin(); recv_it != mpi_recv_nodes_.end(); ++recv_it)
    {
//      CFGNode from = *send_it;
//      CFGNode to = *recv_it;
//      std::pair<CFGNode, CFGNode> p(from,to);
//      mpi_connections_.insert(p);
      mpi_connections_.insert( std::make_pair(*send_it, *recv_it));
    }


  return true;
}

//=======================================================================================
void MpiCommunication::determineMPISend()
{
  std::vector<CFGNode>::const_iterator n_it;
  for(n_it = all_nodes_.begin(); n_it != all_nodes_.end(); ++n_it)
    if(isSgFunctionCallExp(n_it->getNode()))
      if((n_it->isInteresting()) && MpiUtils::isMPISend(n_it->getNode()))
      {
        mpi_send_nodes_.push_back(*n_it);
        if(MpiCFGDebugLevel >= 3)
          std::cerr << "\nSIZE MPI SEND NODES: " << mpi_send_nodes_.size();
      }
}

//=======================================================================================
void MpiCommunication::determineMPIRecv()
{
  std::vector<CFGNode>::const_iterator n_it;
  for(n_it = all_nodes_.begin(); n_it != all_nodes_.end(); ++n_it)
    if(isSgFunctionCallExp(n_it->getNode()))
      if((n_it->isInteresting()) && MpiUtils::isMPIRecv(n_it->getNode()))
      {
        mpi_recv_nodes_.push_back(*n_it);
        if(MpiCFGDebugLevel >= 3)
          std::cerr << "\nSIZE MPI RECV NODES: " << mpi_recv_nodes_.size();
      }
}

//=======================================================================================
void MpiCommunication::refineConstantMatch()
{
  if(MpiCFGDebugLevel >= 2)
    std::cerr << "\n# Refine: Constant Argument Matching";

  std::multimap<CFGNode, CFGNode>::iterator e_it = mpi_connections_.begin();
  bool remove = false;
  while(e_it != mpi_connections_.end()) {
    remove = !constTypeMatch(e_it->first, e_it->second);
    remove = (remove ? remove : !constSizeMatch(e_it->first, e_it->second));
    remove = (remove ? remove : !constTagMatch(e_it->first, e_it->second));
    remove = (remove ? remove : !constCommWorldMatch(e_it->first, e_it->second));

    std::cerr << "\nREMOVE = " << (remove?  "true": "false");
//    if( !constTypeMatch(e_it->first, e_it->second)
//        || !constSizeMatch(e_it->first, e_it->second)
//        || !constTagMatch(e_it->first, e_it->second)
//        || !constCommWorldMatch(e_it->first, e_it->second))
    if(remove)
    {
      remove = false;
      std::multimap<CFGNode, CFGNode>::iterator save_it = e_it;
      e_it++;
      mpi_connections_.erase(save_it);

      if(MpiCFGDebugLevel >= 3)
        std::cerr << "\nOOO removing MPI Edge because of miss match";
    }
    else
      e_it++;
  }
}

//=======================================================================================
bool MpiCommunication::constTypeMatch(const CFGNode& send_node,
                                      const CFGNode& recv_node) const
{
//  if ((send_node == NULL) || (recv_node == NULL))
//    ROSE_ASSERT (!"Cannot find the SgGraphNode in map");

  if(    isSgFunctionCallExp(send_node.getNode())
      && isSgFunctionCallExp(recv_node.getNode()))
  {
    SgExpressionPtrList fromargsptr =
        isSgFunctionCallExp(send_node.getNode())->get_args()->get_expressions();
    SgExpressionPtrList toargsptr =
        isSgFunctionCallExp(recv_node.getNode())->get_args()->get_expressions();
    SgExpression* from_exp = MpiUtils::getExpAt(fromargsptr ,MPI_TYPE_ARG);
    SgExpression* to_exp = MpiUtils::getExpAt(toargsptr ,MPI_TYPE_ARG);
    if((from_exp == NULL) || (to_exp == NULL))
      ROSE_ASSERT (!"Cannot find type SgExpression*");
    while(isSgUnaryOp(from_exp))
      from_exp = isSgUnaryOp(from_exp)->get_operand();
    while(isSgUnaryOp(to_exp))
      to_exp = isSgUnaryOp(to_exp)->get_operand();
    if(isSgVarRefExp(from_exp) && isSgVarRefExp(to_exp))
    {
      std::string ompi = "ompi_mpi";
      std::string from_s = isSgVarRefExp(from_exp)->get_symbol()->get_name().str();
      std::string to_s = isSgVarRefExp(to_exp)->get_symbol()->get_name().str();
      if(from_s == to_s && from_s.substr(0,ompi.size()) == ompi)
        return true;
      if(from_s.substr(0,ompi.size()) == ompi && to_s.substr(0,ompi.size()) == ompi)
        return false;
    }
  }
  return true;
}

//=======================================================================================
bool MpiCommunication::constSizeMatch(const CFGNode& send_node,
                                      const CFGNode& recv_node) const
{
//  if ((from == NULL) || (to == NULL))
//    ROSE_ASSERT (!"Cannot find the SgGraphNode in the map");

  if(    isSgFunctionCallExp(send_node.getNode())
      && isSgFunctionCallExp(recv_node.getNode()))
  {
    SgExpressionPtrList fromargsptr =
        isSgFunctionCallExp(send_node.getNode())->get_args()->get_expressions();
    SgExpressionPtrList toargsptr =
        isSgFunctionCallExp(recv_node.getNode())->get_args()->get_expressions();

    SgExpression* from_exp = MpiUtils::getExpAt(fromargsptr ,MPI_SIZE_ARG);
    SgExpression* to_exp = MpiUtils::getExpAt(toargsptr ,MPI_SIZE_ARG);
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


//=======================================================================================
bool MpiCommunication::constTagMatch(const CFGNode& send_node,
                                     const CFGNode& recv_node) const
{
//  if ((from == NULL) || (to == NULL))
//    ROSE_ASSERT (!"Cannot find the SgGraphNode in the map");

  if(    isSgFunctionCallExp(send_node.getNode())
      && isSgFunctionCallExp(recv_node.getNode()))
  {
    SgExpressionPtrList fromargsptr =
        isSgFunctionCallExp(send_node.getNode())->get_args()->get_expressions();
    SgExpressionPtrList toargsptr =
        isSgFunctionCallExp(recv_node.getNode())->get_args()->get_expressions();

    SgExpression* from_exp = MpiUtils::getExpAt(fromargsptr ,MPI_TAG_ARG);
    SgExpression* to_exp = MpiUtils::getExpAt(toargsptr ,MPI_TAG_ARG);
    if( (from_exp == NULL) || (to_exp == NULL) )
      ROSE_ASSERT (!"Cannot find tag SgExpression*");

    std::cerr << "\n\n CHECKING FOR CONST TAG MATCH!";
    //Both values are Integer Constants.
    if(isSgIntVal(from_exp) && isSgIntVal(to_exp))
    {
      std::cerr << "\n Both int";
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

//=======================================================================================
bool MpiCommunication::constCommWorldMatch(const CFGNode& send_node,
                                           const CFGNode& recv_node) const
{
//  if ((from == NULL) || (to == NULL))
//    ROSE_ASSERT (!"Cannot find the SgGraphNode in map");

  if(    isSgFunctionCallExp(send_node.getNode())
      && isSgFunctionCallExp(recv_node.getNode()))
  {
    SgExpressionPtrList fromargsptr =
        isSgFunctionCallExp(send_node.getNode())->get_args()->get_expressions();
    SgExpressionPtrList toargsptr =
        isSgFunctionCallExp(recv_node.getNode())->get_args()->get_expressions();

    SgExpression* from_exp = MpiUtils::getExpAt(fromargsptr ,MPI_COMM_WORLD_ARG);
    SgExpression* to_exp = MpiUtils::getExpAt(toargsptr ,MPI_COMM_WORLD_ARG);
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






//=======================================================================================
//
//                  Constant Propagation Lattice helper functions
//
//=======================================================================================

//=======================================================================================
bool MpiCommunication::hasConstValue(/*const*/ SgNode* node, int idx) const
{
  ROSE_ASSERT(node != NULL && idx >= 0);

  if(const_prop_ == NULL)
    return false;

  if(MpiCFGDebugLevel >= 1)
    std::cerr <<"\n_hasConstValue:";

  //  if(isSgVarRefExp(node) && MpiUtils::hasDataflowNode(node, idx, cpa_nodes_))
  if(isSgVarRefExp(node)
     && MpiUtils::hasDataflowNode(node, idx, const_prop_->getDataFlowNodeVector()))
  {
    const vector<NodeState*> nodeStates =
        NodeState::getNodeStates(
            MpiUtils::getDataflowNode(node, idx, const_prop_->getDataFlowNodeVector()));
    //NodeState* state = NodeState::getNodeState(dfn, 0);
    //There is only one state??
    NodeState* state = nodeStates.back();

    if(MpiCFGDebugLevel >=3)
      std::cerr << "  GOT A STATE: " << state->str(const_prop_);

    if(state->isInitialized(const_prop_))
    {
      std::vector<Lattice*> latt_vec = state->getLatticeAbove(const_prop_);
      VarsExprsProductLattice* lattice =
          dynamic_cast<VarsExprsProductLattice*> (latt_vec[0]);

      if(lattice != NULL)
      {
        ConstantPropagationLattice* varlattice =
            dynamic_cast <ConstantPropagationLattice*>
                                 (lattice->getVarLattice(varID(node)));

        if(varlattice != NULL)
        {
          if(varlattice->getLevel() == (short)2) //TODO 2 is not a const value to be here
          {
            if(MpiCFGDebugLevel >=3)
              std::cerr << "  A REAL CONST VALUE HERE: " << varlattice->getValue();
            return true;
          }
          else
            std::cerr << "\nFAIL AT VAR LATTICE != NULL\n";
        }
        else
          std::cerr << "\nFAIL AT LATTICE != NULL\n";
      }
    }
    else
      std::cerr << "STATE IS NOT INITIALIZED HERE!";
  }
  return false;

///SECOND TRY ***************************************************************************

//  ROSE_ASSERT(node != NULL);
//
//  std::cerr <<"\n_hasConstValue:";
//  if(isSgVarRefExp(node))
//  {
//    CFGNode cfg = CFGNode(node, idx);
//    DataflowNode dfn = DataflowNode(cfg, defaultFilter);
//    const vector<NodeState*> nodeStates = NodeState::getNodeStates(dfn);
//    //NodeState* state = NodeState::getNodeState(dfn, 0);
//    NodeState* state = nodeStates.back();
//
//    std::cerr << "  GOT A STATE: " << state->str(const_prop_);
//
//    if(state->isInitialized(const_prop_))
//    {
//      std::vector<Lattice*> latt_vec = state->getLatticeAbove(const_prop_);
//      VarsExprsProductLattice* lattice =
//          dynamic_cast<VarsExprsProductLattice*> (latt_vec[0]);
//
//      if(lattice != NULL)
//      {
//        ConstantPropagationLattice* varlattice =
//            dynamic_cast <ConstantPropagationLattice*>
//                                 (lattice->getVarLattice(varID(node)));
//
//        if(varlattice != NULL)
//        {
//          if(varlattice->getLevel() == (short)2) //TODO 2 is not a const value to be here
//          {
//            std::cerr << "  A REAL CONST VALUE HERE: " << varlattice->getValue();
//            return true;
//          }
//          else
//            std::cerr << "\nFAIL AT VAR LATTICE != NULL\n";
//        }
//        else
//          std::cerr << "\nFAIL AT LATTICE != NULL\n";
//      }
//    }
//    else
//      std::cerr << "STATE IS NOT NOT NOT INITIALIZED HERE!";
//  }
//  return false;

///FIRST TRY ****************************************************************************

//    if(!(NodeState::getLatticeAbove(const_prop_, node, 0).size() > 0))
//    {
//      std::cerr<< "....exit on error.....";
//      return false;
//    }
//    std::cerr << "\n\n\nHAS POSSIBLE CONSTANT VALUE \n";
//    VarsExprsProductLattice* lattice =
//      dynamic_cast <VarsExprsProductLattice *>
//        (NodeState::getLatticeAbove(const_prop_, node, 0)[0]);
//
//    ConstantPropagationLattice* varlattice =
//        dynamic_cast <ConstantPropagationLattice*>
//                              (lattice->getVarLattice(varID(node)));
//
//    if(varlattice != NULL)
//    {
//      if(varlattice->getLevel() == (short)2) //TODO 2 is not a const value to be here
//        return true;
//    }
//  }
//  return false;
}

//=======================================================================================
int MpiCommunication::getConstPropValue(/*const*/ SgNode* node, int idx) const
{
  ROSE_ASSERT(node != NULL && idx >= 0 && const_prop_ != NULL);

  if(isSgVarRefExp(node)
     && MpiUtils::hasDataflowNode(node, idx, const_prop_->getDataFlowNodeVector()))
  {
    const vector<NodeState*> nodeStates =
        NodeState::getNodeStates(
            MpiUtils::getDataflowNode(node, idx, const_prop_->getDataFlowNodeVector()));
//    DataflowNode* dfnp = MpiUtils::findDataflowNode(node, idx, cpa_nodes_);
//    const vector<NodeState*> nodeStates = NodeState::getNodeStates(*dfnp);
    NodeState* state = nodeStates.back();

    if(state->isInitialized(const_prop_))
    {
      std::vector<Lattice*> latt_vec = state->getLatticeAbove(const_prop_);
      VarsExprsProductLattice* lattice =
          dynamic_cast<VarsExprsProductLattice*> (latt_vec[0]);
      if(lattice != NULL)
      {
        ConstantPropagationLattice* varlattice =
            dynamic_cast <ConstantPropagationLattice*>
                                (lattice->getVarLattice(varID(node)));
        if(varlattice != NULL)
          if(varlattice->getLevel() == (short)2) //TODO 2 is not a const value to be here
            return varlattice->getValue();
      }
    }
  }
  return 0;
///FIRST TRY ****************************************************************************
//  ROSE_ASSERT(node != NULL);
//
//  if(isSgVarRefExp(node))
//  {
//    string var = node->unparseToString();
//    VarsExprsProductLattice* lattice =
//      dynamic_cast <VarsExprsProductLattice *>
//        (NodeState::getLatticeAbove(const_prop_, node, idx)[0]);
//
//    ConstantPropagationLattice* varlattice =
//        dynamic_cast <ConstantPropagationLattice*>(lattice->getVarLattice(varID(node)));
//
//    if(varlattice != NULL)
//      if(varlattice->getLevel() == (short)2) //TODO: 2 is not a value to be hard coded.
//        return varlattice->getValue();
//  }
//  return 0;
}










//=======================================================================================
//
//                  Debug Comm to Dot outptu
//
//=======================================================================================
//=======================================================================================
void MpiCommunication::mpiCommToDot(const std::string& file_name) const
{
  std::ofstream ofile(file_name.c_str(), std::ios::out);
  mpiPrintDotHeader(ofile);
  mpiPrintNodes(ofile);
  mpiPrintEdges(ofile);
  ofile << "}\n";
}

//=======================================================================================
void MpiCommunication::mpiPrintDotHeader(std::ostream& o) const
{
  o << "digraph MPI_COMMUNCATION_GRAPH {\n"
    << "rankdir=TB\n"
    << "node[style=\"filled\", color=\"white\", fontcolor=\"white\"]\n"
    << "edge[color=\"white\"]\n";

  //go through all send nodes and receive nodes and enter their line
  //numbers in a sorted integer vector.
  std::set<int> lines;
  SgLocatedNode* node;
  std::vector<CFGNode>::const_iterator it;
  for(it = mpi_send_nodes_.begin(); it != mpi_send_nodes_.end(); ++it)
  {
    if(isSgLocatedNode(it->getNode()))
    {
      node = (SgLocatedNode*)(it->getNode());
      if(node->get_file_info() != NULL)
        lines.insert((node->get_file_info())->get_line());
      else
        std::cerr << "Node without file info!";
    }
    else
    {
      std::cerr << "MPI Send SgNode is not an SgLocatedNode!";
    }
  }
  for(it = mpi_recv_nodes_.begin(); it != mpi_recv_nodes_.end(); ++it)
  {
    if(isSgLocatedNode(it->getNode()))
    {
      node = (SgLocatedNode*)(it->getNode());
      if(node->get_file_info() != NULL)
        lines.insert((node->get_file_info())->get_line());
      else
        std::cerr << "Node without file info!";
    }
    else
    {
      std::cerr << "MPI Send SgNode is not an SgLocatedNode!";
    }
  }

  //Time-line in dot graph, to force the nodes to be placed in sequence.
  std::set<int>::const_iterator lines_it;
  for(lines_it = lines.begin(); lines_it != lines.end(); ++lines_it)
    o << *lines_it << " -> ";

  o << "end;\n";
  o << "node[fontcolor=\"black\"]\n";
  o << "edge[color=\"black\"]\n";
}

//=======================================================================================
void MpiCommunication::mpiPrintNodes(std::ostream& o) const
{
  std::vector<CFGNode>::const_iterator it;
  for(it = mpi_send_nodes_.begin(); it != mpi_send_nodes_.end(); ++it)
    mpiPrintNode(o, *it);

  for(it = mpi_recv_nodes_.begin(); it != mpi_recv_nodes_.end(); ++it)
    mpiPrintNode(o, *it);
}

//=======================================================================================
void MpiCommunication::mpiPrintEdges(std::ostream& o) const
{
  std::multimap<CFGNode, CFGNode>::const_iterator it;
  for(it = mpi_connections_.begin(); it != mpi_connections_.end(); ++it)
    mpiPrintEdge(o, it->first, it->second);
}

//=======================================================================================
void MpiCommunication::mpiPrintNode(std::ostream& o,
                                    const VirtualCFG::CFGNode& node) const
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

//=======================================================================================
void MpiCommunication::mpiPrintEdge(std::ostream& o,
                                                 const VirtualCFG::CFGNode& x,
                                                 const VirtualCFG::CFGNode& y) const
{
  std::string id_x = x.id();
  std::string id_y = y.id();

  o << id_x << " -> " << id_y
    << " [label=\"\""
    << ", color=\"black\""
    << "];\n";
}

} /* end of namespace MpiAnalysis */



