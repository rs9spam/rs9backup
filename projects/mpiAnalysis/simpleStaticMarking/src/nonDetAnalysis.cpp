#include "nonDetAnalysis.h"
#include "nodeState.h"
#include "liveDeadVarAnalysis.h"

using namespace cfgUtils;

int MPINonDetAnalysisDebugLevel=1;

//=============================================================================================
//                        MPINonDetLattice
//=============================================================================================

// returns a copy of this lattice
Lattice* MPINonDetLattice::copy() const
{
  return new MPINonDetLattice(*this);
}

// overwrites the state of this Lattice with that of that Lattice
void MPINonDetLattice::copy(Lattice* that_arg)
{
  MPINonDetLattice* that = dynamic_cast<MPINonDetLattice*>(that_arg);

  this->initialized_ = that->initialized_;
  this->dependent_ = that->dependent_;
  this->non_det_function_node_ = that->non_det_function_node_;
}


// computes the meet of this and that and saves the result in this
// returns true if this causes this to change and false otherwise
bool MPINonDetLattice::meetUpdate(Lattice* that_arg)
{
  MPINonDetLattice* that = dynamic_cast<MPINonDetLattice*>(that_arg);

//  Dbg::dbg << "MPINonDetLattice::meetUpdateate"<<endl;
//  Dbg::dbg << "this: " << str("") << endl;
//  Dbg::dbg << "that: " << that->str("") << endl;
  bool old_init = initialized_;
  bool old_dep = dependent_;
  SgNode* old_node = non_det_function_node_;

  // If that is uninitialized, keep this
  if(!(that->initialized_))
  { }
  // Else that is initialized
  // If this is not initialized just copy that
  else if(!initialized_)
    copy(that);
  // If both are initialized, perform the meet
  else
  {
    dependent_ = dependent_ || that->dependent_;
    if(this->non_det_function_node_ == NULL)
      non_det_function_node_ = that->non_det_function_node_;


    //TODO what if this and that node are != NULL

  }

  return (old_init != initialized_) ||
         (old_dep != dependent_) ||
         (old_node != non_det_function_node_);
}

bool MPINonDetLattice::operator==(Lattice* that_arg)
{
  MPINonDetLattice* that = dynamic_cast<MPINonDetLattice*>(that_arg);

  return (initialized_ == that->initialized_) &&
         (dependent_ == that->dependent_) &&
         (non_det_function_node_ == that->non_det_function_node_);
}

// returns the current state of this object
bool MPINonDetLattice::getDep() const
{
  return dependent_;
}

SgNode* MPINonDetLattice::getDepNode() const
{
  return non_det_function_node_;
}

// set the current state of this object, returning true if it causes
// the object to change and false otherwise
bool MPINonDetLattice::setDep(bool dep)
{
  bool modified = this->dependent_ != dep;
  this->dependent_ = dep;
  return modified;
}

bool MPINonDetLattice::setDepNode(SgNode* node)
{
  bool modified = this->non_det_function_node_ != node;
  this->non_det_function_node_ = node;
  return modified;
}

// Sets the state of this lattice to bottom (false, NULL)
// returns true if this causes the lattice's state to change, false otherwise
bool MPINonDetLattice::setToBottom()
{
  bool modified = initialized_!=true && dependent_!=false && non_det_function_node_!=NULL;
  initialized_ = true;
  dependent_ = false;
  non_det_function_node_ = NULL;
  return modified;
}


string MPINonDetLattice::str(string indent)
{
  ostringstream outs;
  if(initialized_)
    outs << indent << "[MPINonDetLattice: NonDetDep = "
         << (dependent_?"true ":"false ")
         << "Node = " << this->non_det_function_node_ << "]";
  else
    outs << indent << "[MPINonDetLattice: uninitialized]";
  return outs.str();
}



//=============================================================================================
//                        MPINonDetAnalysis
//=============================================================================================


//=============================================================================================
bool MPINonDetAnalysis::addNonDetNode(SgNode* node)
{
  bool found = false;
  std::vector<SgNode*>::iterator it;
  for(it = non_det_nodes_.begin(); it != non_det_nodes_.end(); it++)
    if(*it == node)
      found = true;
  if(!found)
  {
    non_det_nodes_.push_back(node);
    std::cerr << "Source of Non Det: "
              << "<node>" << node->unparseToString() << "</node>"
              << "  File name: "
              << node->get_file_info()->get_filenameString()
              << "\n  Line Number: "
              << node->get_file_info()->get_line() << endl
              << endl;
    return true;
  }
  return false;
}


//=============================================================================================
// generates the initial lattice state for the given dataflow node, in the given function,
// with the given NodeState
void MPINonDetAnalysis::genInitState(const Function& func,
                                     const DataflowNode& n,
                                     const NodeState& state,
                                     vector<Lattice*>& initLattices,
                                     vector<NodeFact*>& initFacts)
{
  map<varID, Lattice*> constVars;
//  std::cerr << "genInitState" << n.str() << endl;
//  if(isSgLocatedNode(n.getNode()))
//    std::cerr << isSgLocatedNode(n.getNode())->getFilenameString() << endl;
  FiniteVarsExprsProductLattice* prodLat =
      new FiniteVarsExprsProductLattice(new MPINonDetLattice(), constVars, NULL, NULL, n, state);
//  std::cerr << "prodLat = "<< prodLat->str("    ") << endl;
//  std::cerr << "success!!" << endl;
  initLattices.push_back(prodLat);
}


////=============================================================================================
//// First transfer function:
//// Functionality:
////   Traverse until reaching of R(*)
////     at R(*) set buff and status dependent
////   When reaching if() set all condition variables that are dependent to condition
////
////   Whenever reaching an MPI communication and one of the variables in scope is dependent and
////   condition is set => the source node is of interest.
////
////   Reset the dependent set at every node of non determinism.
////=============================================================================================
//bool MPINonDetAnalysis::transfer(const Function& func,
//                                 const DataflowNode& n,
//                                 NodeState& state,
//                                 const vector<Lattice*>& dfInfo)
//{
//  //Dbg::dbg << "MPINonDetAnalysis::transfer"<<endl;
//  bool modified=false;
//
//  FiniteVarsExprsProductLattice* prodLat =
//      dynamic_cast<FiniteVarsExprsProductLattice*>(*(dfInfo.begin()));
//
//  // make sure that all the non-constant Lattices are initialized
//  for(vector<Lattice*>::const_iterator it = prodLat->getLattices().begin();
//      it!=prodLat->getLattices().end();
//      it++)
//    (dynamic_cast<MPINonDetLattice*>(*it))->initialize();
//
//  SgNode *sgn = n.getNode();
//
//  if(isSgFunctionCallExp(sgn))
//  {
//    //Dbg::dbg << "    isSgFunctionCallExp"<<endl;
//    SgFunctionCallExp* fnCall = isSgFunctionCallExp(sgn);
//    Function calledFunc(fnCall);
//
//    if(calledFunc.get_name().getString() == "MPI_Recv")
//    {
//      std::cerr << "Traverse: At MPI_Recv Node!\n";
//
//      SgExpressionPtrList& args = fnCall->get_args()->get_expressions();
//      // args[0] ... args[5]
//      SgExpression* arg0 = *(args.begin());           //[0] mpi_buff
////      SgExpression* arg1 = *(++(args.begin()));       //[1] mpi_count
////      SgExpression* arg2 = *(args.begin()+2);         //[2] mpi_type
//      SgExpression* arg3 = *(args.begin()+3);         //[3] mpi_address
////      SgExpression* arg4 = *(args.begin()+4);         //[4] mpi_tag
////      SgExpression* arg5 = *(args.begin()+5);         //[5] mpi_comm_world
//      SgExpression* arg6 = *(args.begin()+6);         //[6] mpi_status
//
////      printf("arg1 = [%s | %s]]\n", arg3->class_name().c_str(), arg3->unparseToString().c_str());
//
//      //If MPI_ANY_SOURCE
//      if( isSgMinusOp(arg3) &&
//          isSgIntVal(isSgMinusOp(arg3)->get_operand()) &&
//          isSgIntVal(isSgMinusOp(arg3)->get_operand())->get_value() == 1 )
//      {
//        //Delete all dep and cond.
//        for(vector<Lattice*>::const_iterator it = prodLat->getLattices().begin();
//            it!=prodLat->getLattices().end();
//            it++)
//        {
//          modified = (dynamic_cast<MPINonDetLattice*>(*it))->setDep(false) || modified;
//          modified = (dynamic_cast<MPINonDetLattice*>(*it))->setCond(false) || modified;
//        }
//
//        //check argument 0 (buffer)
//        printf("arg0 = [%s | %s]]\n", arg0->class_name().c_str(), arg0->unparseToString().c_str());
//        //TO DO: This is not very general (sub tree node query).
//        SgNode* n0 = arg0;
//        while(isSgUnaryOp(n0))
//          n0=isSgUnaryOp(n0)->get_operand();
//
//        if(varID::isValidVarExp(n0))
//        {
//          std::cerr << "Traverse: At MPI_Recv Node after SgMinusOp!\n";
//          varID var_arg0 = SgExpr2Var(isSgExpression(n0));
//          MPINonDetLattice* var0_lat =
//              dynamic_cast<MPINonDetLattice*>(prodLat->getVarLattice(var_arg0));
//          std::cerr << "var_arg0=" << var_arg0 << " var0_lat=" << var0_lat << endl;
//          std::cerr << "prodLat=" << prodLat->str("    ") << endl;
//          modified = var0_lat->setDep(true) || modified;
//          modified = var0_lat->setDepNode(sgn) || modified;
//          std::cerr << "prodLat=" << prodLat->str("    ") << endl;
//        }
//
////        //check argument 4 (mpi_tag)
////        printf("arg4 = [%s | %s]]\n", arg4->class_name().c_str(), arg4->unparseToString().c_str());
////        //TO DO: not very exact (sub tree node query)
////        SgNode* n4 = arg4;
////        while(isSgUnaryOp(n4))
////          n4=isSgUnaryOp(n4)->get_operand();
////
////        if(varID::isValidVarExp(n4))
////        {
////          varID var_arg4 = SgExpr2Var(isSgExpression(n4));
////          MPINonDetLattice* var4_lat =
////              dynamic_cast<MPINonDetLattice*>(prodLat->getVarLattice(var_arg4));
////          modified = var4_lat->setDep(true) || modified;
////          modified = var4_lat->setDepNode(sgn) || modified;
////        }
//
//        //check argument 6 (mpi_status)
//        printf("arg6 = [%s | %s]]\n", arg6->class_name().c_str(), arg6->unparseToString().c_str());
//        //TO DO: not very exact (sub tree node query)
//        SgNode* n6 = arg6;
//        while(isSgUnaryOp(n6))
//          n6=isSgUnaryOp(n6)->get_operand();
//
//        if(varID::isValidVarExp(n6))
//        {
//          std::cerr << "Traverse: At MPI_Recv Node after SgMinusOp!\n";
//          varID var_arg6 = SgExpr2Var(isSgExpression(n6));
//          MPINonDetLattice* var6_lat =
//              dynamic_cast<MPINonDetLattice*>(prodLat->getVarLattice(var_arg6));
//          modified = var6_lat->setDep(true) || modified;
//          modified = var6_lat->setDepNode(sgn) || modified;
//        }
//
//      }
//      else
//      {
//        std::cerr << "[nonDetAnalysis][Traverse]:Deterministic Recv()\n";
//      }
//
//    }
//    if(calledFunc.get_name().getString() == "MPI_Send") //TO DO: we are reaching any MPI communication call
//    {
//      //check all variables if there is one dependent and was used in a conditional
//      //if yes, write SgNode info of non det Recv to a file (maybe also the send)
//      //Delete all dep and cond.
//      SgNode* non_det_comm_node = NULL;
//
//      for(vector<Lattice*>::const_iterator it = prodLat->getLattices().begin();
//          it!=prodLat->getLattices().end();
//          it++)
//      {
//        if((dynamic_cast<MPINonDetLattice*>(*it))->getCond())
//        {
//          non_det_comm_node = (dynamic_cast<MPINonDetLattice*>(*it))->getDepNode();
//        }
//        modified = (dynamic_cast<MPINonDetLattice*>(*it))->setDep(false) || modified;
//        modified = (dynamic_cast<MPINonDetLattice*>(*it))->setCond(false) || modified;
//      }
//      if(non_det_comm_node != NULL)
//      {
//        std::cerr << "Source of Non Det: "  << non_det_comm_node->unparseToString() << endl;
//      }
//    }
//
//  }
//  if(isSgIfStmt(sgn)) // set used in conditional if any dependent variable used in conditional
//  {
//    Rose_STL_Container<SgNode*> cond_vars =
//        NodeQuery::querySubTree(isSgIfStmt(sgn)->get_conditional(),V_SgVarRefExp);
//    for(Rose_STL_Container<SgNode*>::iterator it = cond_vars.begin(); it != cond_vars.end(); it++)
//    {
//      std::cerr << "  IfStmtNodes: " << isSgNode(*it)->unparseToString() << endl;
//      varID var = SgExpr2Var(isSgExpression(*it));
//      MPINonDetLattice* var_lat = dynamic_cast<MPINonDetLattice*>(prodLat->getVarLattice(var));
//      if(var_lat->getDep())
//        modified = var_lat->setCond(true) || modified;
//    }
//  }
//
//  return modified;
//}



//=============================================================================================
// Second transfer function:
// Functionality:
//   Traverse until reaching of R(*)
//     at R(*) set buff and status dependent
//   Propagate the dependency of variables
//
//   When reaching if( dep_var ) check true and false block for MPI Communication (node query)
//
//   It should never be necessary to reset the dependency (the propagation should do that)
//=============================================================================================
bool MPINonDetAnalysis::transfer(const Function& func,
                                 const DataflowNode& n,
                                 NodeState& state,
                                 const vector<Lattice*>& dfInfo)
{
  //Dbg::dbg << "MPINonDetAnalysis::transfer"<<endl;
  bool modified=false;

//  std::cerr << "********* TRANSFER"<< n.str() << endl;

  FiniteVarsExprsProductLattice* prodLat =
      dynamic_cast<FiniteVarsExprsProductLattice*>(*(dfInfo.begin()));

  // make sure that all the non-constant Lattices are initialized
  for(vector<Lattice*>::const_iterator it = prodLat->getLattices().begin();
      it!=prodLat->getLattices().end();
      it++)
    (dynamic_cast<MPINonDetLattice*>(*it))->initialize();

  SgNode *sgn = n.getNode();

//#if 0
  if(isSgFunctionCallExp(sgn))
  {
    //Dbg::dbg << "    isSgFunctionCallExp"<<endl;
    SgFunctionCallExp* fnCall = isSgFunctionCallExp(sgn);
    Function calledFunc(fnCall);

//=================================================================================================
    if(calledFunc.get_name().getString() == "MPI_Recv")
    {
      Dbg::dbg << "Traverse: At MPI_Recv Node!\n";

      SgExpressionPtrList& args = fnCall->get_args()->get_expressions();
      // args[0] ... args[5]
      SgExpression* arg0 = *(args.begin());           //[0] mpi_buff
//      SgExpression* arg1 = *(++(args.begin()));       //[1] mpi_count
//      SgExpression* arg2 = *(args.begin()+2);         //[2] mpi_type
      SgExpression* arg3 = *(args.begin()+3);         //[3] mpi_address
//      SgExpression* arg4 = *(args.begin()+4);         //[4] mpi_tag
//      SgExpression* arg5 = *(args.begin()+5);         //[5] mpi_comm_world
      SgExpression* arg6 = *(args.begin()+6);         //[6] mpi_status

//      printf("arg3 = [%s | %s]]\n", arg3->class_name().c_str(), arg3->unparseToString().c_str());


      // Find out if the MPI_Recv address is dependent on a variable from a previous non-deterministic
      // MPI communication function call.
      bool addr_dep = false;
      Rose_STL_Container<SgNode*> addr_vars =
              NodeQuery::querySubTree(arg3,V_SgVarRefExp);

      //Iterate over all variables.
      for(Rose_STL_Container<SgNode*>::iterator it = addr_vars.begin(); it != addr_vars.end(); it++)
      {
        varID var = SgExpr2Var(isSgExpression(*it));
        MPINonDetLattice* addr_lat = dynamic_cast<MPINonDetLattice*>(prodLat->getVarLattice(var));

        if(!addr_lat->getDep()) //if it is not dependent check for other var_lat
        {
          varID temp_var;
          bool found = false;
          set<varID> all_vars = prodLat->getAllVars();
          for(set<varID>::iterator v_it = all_vars.begin(); v_it != all_vars.end(); v_it++)
            if(v_it->str() == isSgNode(*it)->unparseToString())
            {
              temp_var = *v_it;
              found = true;
              v_it = all_vars.end();
              v_it--;
            }
          if(found)
           addr_lat = dynamic_cast<MPINonDetLattice*>(prodLat->getVarLattice(temp_var));
        }
        if(addr_lat->getDep())
          addr_dep = true;
      }

      if( //If MPI_ANY_SOURCE
          (isSgMinusOp(arg3) &&
           isSgIntVal(isSgMinusOp(arg3)->get_operand()) &&
           isSgIntVal(isSgMinusOp(arg3)->get_operand())->get_value() == 1)
//           ||
//          //If MPI_ANY_TAG
//          (isSgMinusOp(arg4) &&
//           isSgIntVal(isSgMinusOp(arg4)->get_operand()) &&
//           isSgIntVal(isSgMinusOp(arg4)->get_operand())->get_value() == 1)
             ||
             addr_dep
          )
      {
        // check argument 0 (buffer)
        // printf("arg0 = [%s | %s]]\n",arg0->class_name().c_str(),arg0->unparseToString().c_str());
        //TODO: This is not very general (sub tree node query).
        SgNode* n0 = arg0;
        while(isSgUnaryOp(n0))
          n0=isSgUnaryOp(n0)->get_operand();

        if(varID::isValidVarExp(n0))
        {
          Dbg::dbg << "Traverse: At MPI_Recv Node ARG 0!\n";
          varID var_arg0 = SgExpr2Var(isSgExpression(n0));
          MPINonDetLattice* var0_lat =
              dynamic_cast<MPINonDetLattice*>(prodLat->getVarLattice(var_arg0));
//          std::cerr << "var_arg0=" << var_arg0.str() << " var0_lat=" << var0_lat << endl;
          // Dbg::dbg << "prodLat=" << prodLat->str("    ") << endl;
          modified = var0_lat->setDep(true) || modified;
          modified = var0_lat->setDepNode(sgn) || modified;
//          std::cerr << "prodLat=" << prodLat->str("    ") << endl;
        }

//        //check argument 4 (mpi_tag)
//        printf("arg4 = [%s | %s]]\n", arg4->class_name().c_str(), arg4->unparseToString().c_str());
//        //TODO: not very exact (sub tree node query)
//        SgNode* n4 = arg4;
//        while(isSgUnaryOp(n4))
//          n4=isSgUnaryOp(n4)->get_operand();
//
//        if(varID::isValidVarExp(n4))
//        {
//          varID var_arg4 = SgExpr2Var(isSgExpression(n4));
//          MPINonDetLattice* var4_lat =
//              dynamic_cast<MPINonDetLattice*>(prodLat->getVarLattice(var_arg4));
//          modified = var4_lat->setDep(true) || modified;
//          modified = var4_lat->setDepNode(sgn) || modified;
//        }

        // check argument 6 (mpi_status)
        // printf("arg6 = [%s | %s]]\n",arg6->class_name().c_str(),arg6->unparseToString().c_str());
        //TODO: not very exact (sub tree node query)
        SgNode* n6 = arg6;
        while(isSgUnaryOp(n6))
          n6=isSgUnaryOp(n6)->get_operand();

        if(varID::isValidVarExp(n6))
        {
          Dbg::dbg << "Traverse: At MPI_Recv Node ARG 6!\n";
          varID var_arg6 = SgExpr2Var(isSgExpression(n6));
          MPINonDetLattice* var6_lat =
              dynamic_cast<MPINonDetLattice*>(prodLat->getVarLattice(var_arg6));
//          std::cerr << "var_arg6 = " << var_arg6.str() << " var6_lat=" << var6_lat << endl;
          modified = var6_lat->setDep(true) || modified;
          modified = var6_lat->setDepNode(sgn) || modified;
        }

      }
      else
      {
        Dbg::dbg << "[nonDetAnalysis][Traverse]:Communication independent MPI_Recv(MPI_ANY_SOURCE)\n";
      }
    }

//=================================================================================================
// MPI_Probe( MPI_ANY_SOURCE, EVENT_TYPE, MPI_COMM_WORLD, &probe_status );
// MPI_Recv(event_array, 2, MPI_INT, probe_status.MPI_SOURCE, EVENT_TYPE, MPI_COMM_WORLD, &status);
// if(event_array[0] == xx)
//   break;
    if(calledFunc.get_name().getString() == "MPI_Probe")
    {
      Dbg::dbg << "Traverse: At MPI_Probe Node!\n";

      SgExpressionPtrList& args = fnCall->get_args()->get_expressions();
      // args[0] ... args[5]
      SgExpression* arg0 = *(args.begin());           //[0] mpi_address
//      SgExpression* arg1 = *(++(args.begin()));       //[4] mpi_tag
//      SgExpression* arg2 = *(args.begin()+2);         //[5] mpi_comm_world
      SgExpression* arg3 = *(args.begin()+3);         //[6] mpi_status

//      printf("arg1 = [%s | %s]]\n", arg3->class_name().c_str(), arg3->unparseToString().c_str());

      if( //If MPI_ANY_SOURCE
          isSgMinusOp(arg0) &&
          isSgIntVal(isSgMinusOp(arg0)->get_operand()) &&
          isSgIntVal(isSgMinusOp(arg0)->get_operand())->get_value() == 1)
      {
        // check argument 3 (mpi_status)
        // printf("arg3 = [%s | %s]]\n",arg3->class_name().c_str(),arg3->unparseToString().c_str());
        //TODO: not very exact (sub tree node query)
        SgNode* n3 = arg3;
        while(isSgUnaryOp(n3))
          n3=isSgUnaryOp(n3)->get_operand();

        if(varID::isValidVarExp(n3))
        {
          Dbg::dbg << "Traverse: At MPI_Probe Node ARG 3!\n";
          varID var_arg3 = SgExpr2Var(isSgExpression(n3));
          MPINonDetLattice* var3_lat =
              dynamic_cast<MPINonDetLattice*>(prodLat->getVarLattice(var_arg3));
//          std::cerr << "var_arg3 = " << var_arg3.str() << " var3_lat=" << var3_lat << endl;
          modified = var3_lat->setDep(true) || modified;
          modified = var3_lat->setDepNode(sgn) || modified;
        }
      }
      else
      {
        Dbg::dbg << "[nonDetAnalysis][Traverse]:Communication independent MPI_Recv(MPI_ANY_SOURCE)\n";
      }
    }
  }

  else if(isSgIfStmt(sgn))
  {
    //Get all variables from the conditional statement and find out if they are dependet.
    Rose_STL_Container<SgNode*> cond_vars =
        NodeQuery::querySubTree(isSgIfStmt(sgn)->get_conditional(),V_SgVarRefExp);

//    std::vector<SgDeclarationStatement *> temp = SageInterface::getDependentDeclarations(isSgIfStmt(sgn));
//    std::cerr << "$$$$ size1: " << temp.size() << " size2: " << cond_vars.size() << endl;
//    for(std::vector<SgDeclarationStatement *>::iterator tempit = temp.begin();
//        tempit != temp.end(); tempit++ )
//    {
//      std::cerr << "  IfStmtNodes: " << isSgNode(*tempit)->unparseToString() << endl;
//    }

    //Iterate over all variables.
    for(Rose_STL_Container<SgNode*>::iterator it = cond_vars.begin(); it != cond_vars.end(); it++)
    {
      //Get The lattice value for the variable at this node.
//      std::cerr << "  IfStmtNodes: " << isSgNode(*it)->unparseToString() << endl;
      varID var = SgExpr2Var(isSgExpression(*it));
//      std::cerr << "VariableID: " << var.str() << endl;

      MPINonDetLattice* var_lat = dynamic_cast<MPINonDetLattice*>(prodLat->getVarLattice(var));

      if(!var_lat->getDep())    //if it is not dependent check for other var_lat
      {
        varID temp_var;
        bool found = false;
        set<varID> all_vars = prodLat->getAllVars();
        for(set<varID>::iterator v_it = all_vars.begin(); v_it != all_vars.end(); v_it++)
          if(v_it->str() == isSgNode(*it)->unparseToString())
          {
            temp_var = *v_it;
            found = true;
            v_it = all_vars.end();
            v_it--;
          }
        if(found)
         var_lat = dynamic_cast<MPINonDetLattice*>(prodLat->getVarLattice(temp_var));
      }

      //If the variable is dependent on an non-deterministic MPI function call check if this
      //conditional causes non-determinism in the MPI communication patterns.
      if(var_lat->getDep())
      {
        bool found_mpi_function = false;

        if(isMpiInScope(sgn))
        {
          addNonDetNode(var_lat->getDepNode());
          found_mpi_function = true;
        }
        //If there was no MPI Function in the Basic blocks check for break stmt
        if(!found_mpi_function)
        {
          //std::cerr << "Did not find MPI Function in this If/Else basic blocks! \n";
          if(isBreakInScope(isSgStatement(sgn)))
          {
            //std::cerr << "There is a BREAK statement! \n";
            SgNode* scope = getParentScope(sgn);
            if(isSgBasicBlock(scope))
              scope = getParentScope(scope);

            // If the break is in a loop. => Check the whole loop scope for MPI functions.
            if(scope == SageInterface::findEnclosingLoop(SageInterface::getEnclosingStatement(sgn)))
            {
              // If there is an MPI function call in the loop scope add the non-det function.
              if(isMpiInScope(scope))
                addNonDetNode(var_lat->getDepNode());
            }
          }
          else if(isContinueInScope(isSgStatement(sgn)))
          {
//            std::cerr << "There is a CONTINUE statement! \n";
            SgNode* scope = getParentScope(sgn);
            if(isSgBasicBlock(scope))
              scope = getParentScope(scope);

            // If the continue is in a loop. => Check the whole loop scope for MPI functions.
            if(scope == SageInterface::findEnclosingLoop(SageInterface::getEnclosingStatement(sgn)))
            {
              SgNode* cont_stmt = getFirstBreakStmtNodeFromScope(sgn);
              if(isMpiInScopeAfter(scope, cont_stmt))
                addNonDetNode(var_lat->getDepNode());
            }
          }
        }
      }
    }
  }
  else if(isSgWhileStmt(sgn))
  {
    //Get all variables from the conditional statement and find out if they are dependet.
    Rose_STL_Container<SgNode*> cond_vars =
        NodeQuery::querySubTree(isSgWhileStmt(sgn)->get_condition(),V_SgVarRefExp);
    //Iterate over all variables.
    for(Rose_STL_Container<SgNode*>::iterator it = cond_vars.begin(); it != cond_vars.end(); it++)
    {
      //Get The lattice value for the variable at this node.
//      Dbg::dbg << "SgWhileStmt: " << sgn->unparseToString() << endl
//               << "  File name: " << sgn->get_file_info()->get_filenameString()
//               << "\n  Line Number: " << sgn->get_file_info()->get_line() << endl;
      varID var = SgExpr2Var(isSgExpression(*it));
      MPINonDetLattice* var_lat = dynamic_cast<MPINonDetLattice*>(prodLat->getVarLattice(var));

      if(!var_lat->getDep()) //if it is not dependent check for other var_lat
      {
        varID temp_var;
        bool found = false;
        set<varID> all_vars = prodLat->getAllVars();
        for(set<varID>::iterator v_it = all_vars.begin(); v_it != all_vars.end(); v_it++)
          if(v_it->str() == isSgNode(*it)->unparseToString())
          {
            temp_var = *v_it;
            found = true;
            v_it = all_vars.end();
            v_it--;
          }
        if(found)
         var_lat = dynamic_cast<MPINonDetLattice*>(prodLat->getVarLattice(temp_var));
      }
      if(var_lat->getDep())
        if(isMpiInScope(sgn))
          addNonDetNode(var_lat->getDepNode());
    }

  }
  else if(isSgForStatement(sgn))
  {
    Rose_STL_Container<SgNode*> init_vars =
        NodeQuery::querySubTree(isSgForStatement(sgn)->get_for_init_stmt(),V_SgVarRefExp);
    for(Rose_STL_Container<SgNode*>::iterator it = init_vars.begin(); it != init_vars.end(); it++)
    {
      varID var = SgExpr2Var(isSgExpression(*it));
      MPINonDetLattice* var_lat = dynamic_cast<MPINonDetLattice*>(prodLat->getVarLattice(var));
      if(!var_lat->getDep()) //if it is not dependent check for other var_lat
      {
        varID temp_var;
        bool found = false;
        set<varID> all_vars = prodLat->getAllVars();
        for(set<varID>::iterator v_it = all_vars.begin(); v_it != all_vars.end(); v_it++)
          if(v_it->str() == isSgNode(*it)->unparseToString())
          {
            temp_var = *v_it;
            found = true;
            v_it = all_vars.end();
            v_it--;
          }
        if(found)
         var_lat = dynamic_cast<MPINonDetLattice*>(prodLat->getVarLattice(temp_var));
      }
      if(var_lat->getDep())
        if(isMpiInScope(sgn))
          addNonDetNode(var_lat->getDepNode());
    }
    Rose_STL_Container<SgNode*> test_vars =
        NodeQuery::querySubTree(isSgForStatement(sgn)->get_test(),V_SgVarRefExp);
    for(Rose_STL_Container<SgNode*>::iterator it = test_vars.begin(); it != test_vars.end(); it++)
    {
      varID var = SgExpr2Var(isSgExpression(*it));
      MPINonDetLattice* var_lat = dynamic_cast<MPINonDetLattice*>(prodLat->getVarLattice(var));
      if(!var_lat->getDep()) //if it is not dependent check for other var_lat
      {
        varID temp_var;
        bool found = false;
        set<varID> all_vars = prodLat->getAllVars();
        for(set<varID>::iterator v_it = all_vars.begin(); v_it != all_vars.end(); v_it++)
          if(v_it->str() == isSgNode(*it)->unparseToString())
          {
            temp_var = *v_it;
            found = true;
            v_it = all_vars.end();
            v_it--;
          }
        if(found)
         var_lat = dynamic_cast<MPINonDetLattice*>(prodLat->getVarLattice(temp_var));
      }
      if(var_lat->getDep())
        if(isMpiInScope(sgn))
          addNonDetNode(var_lat->getDepNode());
    }
    Rose_STL_Container<SgNode*> inc_vars =
        NodeQuery::querySubTree(isSgForStatement(sgn)->get_increment(),V_SgVarRefExp);
    for(Rose_STL_Container<SgNode*>::iterator it = inc_vars.begin(); it != inc_vars.end(); it++)
    {
      varID var = SgExpr2Var(isSgExpression(*it));
      MPINonDetLattice* var_lat = dynamic_cast<MPINonDetLattice*>(prodLat->getVarLattice(var));
      if(!var_lat->getDep()) //if it is not dependent check for other var_lat
      {
        varID temp_var;
        bool found = false;
        set<varID> all_vars = prodLat->getAllVars();
        for(set<varID>::iterator v_it = all_vars.begin(); v_it != all_vars.end(); v_it++)
          if(v_it->str() == isSgNode(*it)->unparseToString())
          {
            temp_var = *v_it;
            found = true;
            v_it = all_vars.end();
            v_it--;
          }
        if(found)
         var_lat = dynamic_cast<MPINonDetLattice*>(prodLat->getVarLattice(temp_var));
      }
      if(var_lat->getDep())
        if(isMpiInScope(sgn))
          addNonDetNode(var_lat->getDepNode());
    }
  }
  else if(isSgDoWhileStmt(sgn))
  {
    //Get all variables from the conditional statement and find out if they are dependet.
    Rose_STL_Container<SgNode*> cond_vars =
        NodeQuery::querySubTree(isSgDoWhileStmt(sgn)->get_condition(),V_SgVarRefExp);

    //Iterate over all variables.
    for(Rose_STL_Container<SgNode*>::iterator it = cond_vars.begin(); it != cond_vars.end(); it++)
    {
      //Get The lattice value for the variable at this node.
      Dbg::dbg << "SgDoWhileStmt: " << sgn->unparseToString() << endl
               << "  File name: " << sgn->get_file_info()->get_filenameString()
               << "\n  Line Number: " << sgn->get_file_info()->get_line() << endl;

      varID var = SgExpr2Var(isSgExpression(*it));
      MPINonDetLattice* var_lat = dynamic_cast<MPINonDetLattice*>(prodLat->getVarLattice(var));
      if(!var_lat->getDep()) //if it is not dependent check for other var_lat
      {
        varID temp_var;
        bool found = false;
        set<varID> all_vars = prodLat->getAllVars();
        for(set<varID>::iterator v_it = all_vars.begin(); v_it != all_vars.end(); v_it++)
          if(v_it->str() == isSgNode(*it)->unparseToString())
          {
            temp_var = *v_it;
            found = true;
            v_it = all_vars.end();
            v_it--;
          }
        if(found)
         var_lat = dynamic_cast<MPINonDetLattice*>(prodLat->getVarLattice(temp_var));
      }
      if(var_lat->getDep())
        if(isMpiInScope(sgn))
          addNonDetNode(var_lat->getDepNode());
    }
  }

  else if(isSgSwitchStatement(sgn))
  {
    //Get all variables from the item selector expression and find out if they are dependent.
    Rose_STL_Container<SgNode*> cond_vars =
        NodeQuery::querySubTree(isSgSwitchStatement(sgn)->get_item_selector(),V_SgVarRefExp);

    Dbg::dbg << "### SgSwitchStatement: variables count: " << cond_vars.size() << endl;

    //Iterate over all variables.
    for(Rose_STL_Container<SgNode*>::iterator it = cond_vars.begin(); it != cond_vars.end(); it++)
    {
      //Get The lattice value for the variable at this node.
      Dbg::dbg << "SgSwitchStmt: " << sgn->unparseToString() << endl
               << "  File name: " << sgn->get_file_info()->get_filenameString()
               << "\n  Line Number: " << sgn->get_file_info()->get_line() << endl;
      varID var = SgExpr2Var(isSgExpression(*it));
      MPINonDetLattice* var_lat = dynamic_cast<MPINonDetLattice*>(prodLat->getVarLattice(var));

      if(!var_lat->getDep()) //if it is not dependent check for other var_lat
      {
        varID temp_var;
        bool found = false;
        set<varID> all_vars = prodLat->getAllVars();
        for(set<varID>::iterator v_it = all_vars.begin(); v_it != all_vars.end(); v_it++)
          if(v_it->str() == isSgNode(*it)->unparseToString())
          {
            temp_var = *v_it;
            found = true;
            v_it = all_vars.end();
            v_it--;
          }
        if(found)
         var_lat = dynamic_cast<MPINonDetLattice*>(prodLat->getVarLattice(temp_var));
      }
      if(var_lat->getDep())
        if(isMpiInScope(sgn))
          addNonDetNode(var_lat->getDepNode());
    }
  }

  // Binary operations: lhs=rhs, lhs+=rhs, lhs+rhs, ...
  else if(isSgBinaryOp(sgn)) {
//#endif
//  if(isSgBinaryOp(sgn)) {
    // Memory objects denoted by the expression's left- and right-hand
    // sides as well as the SgAssignOp itself
    varID lhs = SgExpr2Var(isSgBinaryOp(sgn)->get_lhs_operand());
    varID rhs = SgExpr2Var(isSgBinaryOp(sgn)->get_rhs_operand());
    varID res = SgExpr2Var(isSgBinaryOp(sgn));

    // The lattices associated the three memory objects
    MPINonDetLattice* resLat =
            dynamic_cast<MPINonDetLattice*>(prodLat->getVarLattice(res));
    MPINonDetLattice* lhsLat =
            dynamic_cast<MPINonDetLattice*>(prodLat->getVarLattice(lhs));
    MPINonDetLattice* rhsLat =
            dynamic_cast<MPINonDetLattice*>(prodLat->getVarLattice(rhs));
    ROSE_ASSERT(rhsLat);

    // Assignment: lhs = rhs, lhs+=rhs, lhs*=rhs,  lhs/=rhs, ...
    //    dependence flows from rhs to lhs and res
    if(isSgAssignOp(sgn))
    {
      // If the lhs and/or the SgAssignOp are live, copy lattice from the rhs
      if(lhsLat){ lhsLat->copy(rhsLat); modified = true; }
      if(resLat){ resLat->copy(rhsLat); modified = true; }
    }
    else if(isSgCompoundAssignOp(sgn))
    {
      if(lhsLat) {
        lhsLat->meetUpdate(rhsLat);
        modified = true;
      }
      if(resLat) {
        // 'lhs' must be alive here, and thus provide a lattice value, beacuse 'res' depends on it
        ROSE_ASSERT(lhsLat);
        resLat->copy(lhsLat);
        modified = true;
      }
    }
    // Non-assignments that do not refer to variables: lhs+rhs
    // dependence flows from lhs and rhs to res
    else {
            if(resLat) {
                    resLat->copy(rhsLat);
                    resLat->meetUpdate(lhsLat);
                    modified = true;
            }
    }
    // NOTE: we need to deal with expressions such as a.b, a->b and a[i] specially since they refer to memory locations,
    //       especially sub-expressions (e.g. a.b for a.b.c.d or a[i] for a[i][j][k]) but we don't yet have good abstraction for this.
  // Unary operations
  } else if(isSgUnaryOp(sgn)) {
    if(!isSgAddressOfOp(sgn)) {
      // Memory objects denoted by the expression's oprand as well as the expression itself
      varID op = SgExpr2Var(isSgUnaryOp(sgn)->get_operand());
      varID res = SgExpr2Var(isSgUnaryOp(sgn));

      // The lattices associated the three memory objects
      MPINonDetLattice* opLat =
              dynamic_cast<MPINonDetLattice*>(prodLat->getVarLattice(op));
      MPINonDetLattice* resLat =
              dynamic_cast<MPINonDetLattice*>(prodLat->getVarLattice(res));

      ROSE_ASSERT(opLat);

      // Copy lattice from the operand
      resLat->copy(opLat);
      modified = true;
    }
  // Conditional Operators: (x? y: z)
  } else if(isSgConditionalExp(sgn)) {
    // Memory objects denoted by the expression's condition, true and false sub-expressions
    varID condE  = SgExpr2Var(isSgConditionalExp(sgn)->get_conditional_exp());
    varID trueE  = SgExpr2Var(isSgConditionalExp(sgn)->get_true_exp());
    varID falseE = SgExpr2Var(isSgConditionalExp(sgn)->get_false_exp());
    varID res    = SgExpr2Var(isSgConditionalExp(sgn));

    // The lattices associated the three memory objects
    MPINonDetLattice* resLat =
            dynamic_cast<MPINonDetLattice*>(prodLat->getVarLattice(res));
    MPINonDetLattice* condLat =
            dynamic_cast<MPINonDetLattice*>(prodLat->getVarLattice(condE));
    MPINonDetLattice* trueLat =
            dynamic_cast<MPINonDetLattice*>(prodLat->getVarLattice(trueE));
    MPINonDetLattice* falseLat =
            dynamic_cast<MPINonDetLattice*>(prodLat->getVarLattice(falseE));
    ROSE_ASSERT(condLat); ROSE_ASSERT(trueLat); ROSE_ASSERT(falseLat);

    // Dependence flows from the sub-expressions of the SgConditionalExp to res
    if(resLat) {
      resLat->copy(condLat);
      resLat->meetUpdate(trueLat);
      resLat->meetUpdate(falseLat);
      modified = true;
    }
  // Variable Declaration
  } else if(isSgInitializedName(sgn)) {
    varID var(isSgInitializedName(sgn));
    MPINonDetLattice* varLat = dynamic_cast<MPINonDetLattice*>(prodLat->getVarLattice(var));

    // If this variable is live
    if(varLat) {
      // If there was no initializer, initialize its lattice to Bottom
      if(isSgInitializedName(sgn)->get_initializer()==NULL)
        modified = varLat->setToBottom() || modified;
      // Otherwise, copy the lattice of the initializer to the variable
      else {
        varID init = SgExpr2Var(isSgInitializedName(sgn)->get_initializer());
        MPINonDetLattice* initLat = dynamic_cast<MPINonDetLattice*>(prodLat->getVarLattice(init));
        ROSE_ASSERT(initLat);
        varLat->copy(initLat);
        modified = true;
      }
    }
  // Initializer for a variable
  } else if(isSgAssignInitializer(sgn)) {
    // Memory objects of the initialized variable and the
    // initialization expression
    varID res = SgExpr2Var(isSgAssignInitializer(sgn));
    varID asgn = SgExpr2Var(isSgAssignInitializer(sgn)->get_operand());

    // The lattices associated both memory objects
    MPINonDetLattice* resLat = dynamic_cast<MPINonDetLattice*>(prodLat->getVarLattice(res));
    MPINonDetLattice* asgnLat = dynamic_cast<MPINonDetLattice*>(prodLat->getVarLattice(asgn));
    ROSE_ASSERT(resLat);

    // Copy lattice from the assignment
    resLat->copy(asgnLat);
    modified = true;
  }

  return modified;
}

//=============================================================================================

static MPINonDetAnalysis* nonDetAnal=NULL;
static Analysis* nonDetAnalysis=NULL;
static ContextInsensitiveInterProceduralDataflow* nonDetAnal_inter=NULL;

//=============================================================================================
MPINonDetAnalysis* runMPINonDetAnalysis(
                              SgIncidenceDirectedGraph* graph,
//                              LiveDeadVarsAnalysis* ldva,
                              string indent)
//MPINonDetAnalysis* runMPINonDetAnalysis(SgIncidenceDirectedGraph* graph, string indent)
{
//  nonDetAnal = new MPINonDetAnalysis(ldva);
  ClassHierarchyWrapper classHierarchy(SageInterface::getProject());

  nonDetAnal = new MPINonDetAnalysis(&classHierarchy);
  nonDetAnalysis = nonDetAnal;
  nonDetAnal_inter = new ContextInsensitiveInterProceduralDataflow(nonDetAnal, graph);
  nonDetAnal_inter->runAnalysis();

  //filename:
  SgFunctionDeclaration* mainDefDecl = SageInterface::findMain(SageInterface::getProject());
  ROSE_ASSERT (mainDefDecl != NULL);
  SgFunctionDefinition* mainDef = mainDefDecl->get_definition();
  ROSE_ASSERT (mainDef != NULL);
  string file_name =
      StringUtility::stripPathFromFileName(mainDef->get_file_info()->get_filenameString());
  string file_name1 = file_name + "." + mainDef->get_declaration()->get_name() +".nondet";

  //print non deterministic nodes to file
  std::ofstream ofile(file_name1.c_str(), std::ios::out);
  vector<SgNode*> non_det_nodes = nonDetAnal->getNonDetNodes();
  vector<SgNode*>::iterator it;
  for(it = non_det_nodes.begin(); it != non_det_nodes.end(); it++)
  {
    ofile << "<call> "
          << "<file>" << (*it)->get_file_info()->get_filenameString() << "</file> "
          << "<line>" << (*it)->get_file_info()->get_line() << "</line> "
          << "<node>" << (*it)->unparseToString() << "</node> "
          << "</call>\n";
  }
  ofile.close();

  if(MPINonDetAnalysisDebugLevel>0)
  {
    printMPINonDetAnalysisStates(nonDetAnal, "");
  }

  return nonDetAnal;
}

//=============================================================================================
// Prints the Lattices set by the given MPINonDetAnalysis
void printMPINonDetAnalysisStates(string indent) {
        return printMPINonDetAnalysisStates(nonDetAnal, indent);
}

//=============================================================================================
void printMPINonDetAnalysisStates(MPINonDetAnalysis* nonDetAnal, string indent)
{
  vector<int> factNames;
  vector<int> latticeNames;
  latticeNames.push_back(0);
  Dbg::enterFunc("States of MPINonDetAnalysis");
  printAnalysisStates pas(nonDetAnal, factNames, latticeNames, printAnalysisStates::above, indent);
  UnstructuredPassInterAnalysis upia_pas(pas);
  upia_pas.runAnalysis();
  Dbg::exitFunc("States of MPINonDetAnalysis");
}

//=============================================================================================
bool isMpiFunctionCall(SgNode* node)
{
  if(isSgFunctionCallExp(node))
  {
    SgFunctionCallExp* fnCall = isSgFunctionCallExp(node);
    Function calledFunc(fnCall);
    string function_name = calledFunc.get_name().getString();
    if(function_name == "MPI_Recv" ||
       function_name == "MPI_Send")
      return true;
  }
  return false;
}

//=============================================================================================
SgNode* getParentScope(SgNode* sgn)
{
  SgScopeStatement* scope;

//  std::cerr << "Looking for scope of: " << sgn->unparseToString() << endl
//            << "  File name: " << sgn->get_file_info()->get_filenameString()
//            << "\n  Line Number: " << sgn->get_file_info()->get_line() << endl;

  scope = SageInterface::getScope(sgn);
  scope = SageInterface::getEnclosingNode<SgScopeStatement>(isSgNode(scope));

//  std::cerr << "Found this scope: " << isSgNode(scope)->unparseToString() << endl
//            << "  File name: " << isSgNode(scope)->get_file_info()->get_filenameString()
//            << "\n  Line Number: " << isSgNode(scope)->get_file_info()->get_line() << endl
//            << endl;

  return isSgNode(scope);
}


//=============================================================================================
bool MPINonDetAnalysis::isMpiInScope(SgNode* scope)
{
  std::set<SgNode*> explored;   // Definition
  return isMpiInScope(scope, explored);
}

//=============================================================================================
bool MPINonDetAnalysis::isMpiInScope(SgNode* scope, set<SgNode*>& explored)
{
  Rose_STL_Container<SgNode*> nodes = NodeQuery::querySubTree(scope, V_SgFunctionCallExp);

//  std::cerr << "isMpiInScope: FunctionCallExp number: " << nodes.size() << endl;

  Rose_STL_Container<SgNode*>::iterator it;
  for(it = nodes.begin(); it != nodes.end(); ++it)
  {
//    std::cerr << "isMpiInScope: FunctionCallExp: " << isSgNode(*it)->unparseToString() << endl;
    if(isMpiFunctionCall(*it))
      return true;
  }
  vector<SgNode*> n_func;
  for(it = nodes.begin(); it != nodes.end(); ++it)
  {
    if(isSgFunctionCallExp(*it))
    {
      Rose_STL_Container<SgFunctionDefinition*> defs;
      CallTargetSet::getDefinitionsForExpression(isSgExpression(*it), classHierarchy_, defs);
//      std::cerr<< "isMpiInScope: FunctionDefs size: " << defs.size() << endl;

      Rose_STL_Container<SgFunctionDefinition*>::iterator def_it;
      for(def_it = defs.begin(); def_it != defs.end(); def_it++)
        if(!(explored.count(*def_it) > 0))
        {
//          std::cerr << "isMpiInScope: add to new: " << isSgNode(*def_it)->unparseToString() << endl;
          n_func.push_back(*def_it);
        }
    }
  }

  if(n_func.size() == 0)
    return false;

  else
  {
    bool ret = false;
    vector<SgNode*>::iterator n_it;
    for(n_it = n_func.begin(); n_it != n_func.end(); ++n_it)
      explored.insert(*n_it);

    for(n_it = n_func.begin(); n_it != n_func.end(); ++n_it)
    {
//      std::cerr << "isMpiInScope: recursive call 1" << endl;
      if(isSgScopeStatement(*n_it))
      {
//        std::cerr << "isMpiInScope: recursive call 2" << endl;
        ret = isMpiInScope(*n_it, explored);
      }
      if(ret)
      {
        n_it = n_func.end();
        n_it--;
      }
    }
    return ret;
  }
}

//=============================================================================================
bool MPINonDetAnalysis::isMpiInScopeAfter(SgNode* scope, SgNode* stmt)
{
  //TODO
//  Rose_STL_Container<SgNode*> nodes = NodeQuery::querySubTree(scope,V_SgFunctionCallExp);
//
//  Rose_STL_Container<SgNode*>::iterator t_it;
//  for(t_it = nodes.begin(); t_it != nodes.end(); ++t_it)
//  {
//    if(isMpiFunctionCall(*t_it))
//    {
//      if(addNonDetNode(non_det_node))
//      {
//        std::cerr << "Source of Non Det: "
//            << isSgNode(non_det_node)->unparseToString() << endl
//            << "  File name: "
//            << isSgNode(non_det_node)->get_file_info()->get_filenameString()
//            << "\n  Line Number: "
//            << isSgNode(non_det_node)->get_file_info()->get_line() << endl
//            << endl;
//      }
//      return true;
//    }
//  }
  return isMpiInScope(scope);
}

//=============================================================================================
bool isBreakInScope(SgStatement* scope)
{
  //Rose_STL_Container<SgNode*> break_nodes = NodeQuery::querySubTree(scope, V_SgBreakStmt);
  //std::vector< SgBreakStmt * >  findBreakStmts (SgStatement *code,
  //                                              const std::string &fortranLabel="")
  std::vector<SgBreakStmt*>  break_nodes = SageInterface::findBreakStmts(scope);

  if(break_nodes.size() > 0)
    return true;

  return false;
}

//=============================================================================================
bool isContinueInScope(SgStatement* scope)
{
  //Rose_STL_Container<SgNode*> nodes = NodeQuery::querySubTree(scope, V_SgContinueStmt);
  std::vector<SgContinueStmt*> nodes = SageInterface::findContinueStmts(scope);

  if(nodes.size() > 0)
    return true;

  return false;
}

//=============================================================================================
SgNode* getFirstBreakStmtNodeFromScope(SgNode* scope)
{
  SgNode* node = NULL;
  Rose_STL_Container<SgNode*> nodes = NodeQuery::querySubTree(scope, V_SgBreakStmt);
  if(nodes.size() > 0)
    node = *(nodes.begin());

  return node;
}

////=============================================================================================
//// Returns whether the given variable at the given DataflowNode depends on the process' rank
//bool isMPIRankVarDep(const Function& func, const DataflowNode& n, varID var)
//{
//  ROSE_ASSERT(nonDetAnal);
//
//  NodeState* state = NodeState::getNodeState(n, 0);
//  FiniteVarsExprsProductLattice* prodLat =
//      dynamic_cast<FiniteVarsExprsProductLattice*>(state->getLatticeBelow(nonDetAnalysis, 0));
//  ROSE_ASSERT(prodLat);
//  MPINonDetLattice* rnLat = dynamic_cast<MPINonDetLattice*>(prodLat->getVarLattice(var));
////  Dbg::dbg << "isMPIRankVarDep() n.getNode()=" << Dbg::escape(n.getNode()->unparseToString())
////           << " var=" << var << " rnLat=" << rnLat << endl;
////  cout << "isMPIRankVarDep() n.getNode()=" << Dbg::escape(n.getNode()->unparseToString())
////       <<" var=" << var << " rnLat=" << rnLat << endl;
//  if(!rnLat) return false;
////  Dbg::dbg << "        isMPIRankVarDep() var=" << var << " rnLat=" << rnLat->str() << endl;
////  Dbg::dbg << "isMPIRankVarDep() rnLat=" << rnLat->str() << endl;
//  return rnLat->getRankDep();
//}
//
////=============================================================================================
//// Returns whether the given variable at the given DataflowNode depends on the number of proc
//bool isMPINprocsVarDep(const Function& func, const DataflowNode& n, varID var)
//{
//  ROSE_ASSERT(nonDetAnal);
//
//  NodeState* state = NodeState::getNodeState(n, 0);
//  FiniteVarsExprsProductLattice* prodLat =
//      dynamic_cast<FiniteVarsExprsProductLattice*>(state->getLatticeBelow(nonDetAnalysis, 0));
//  MPINonDetLattice* rnLat = dynamic_cast<MPINonDetLattice*>(prodLat->getVarLattice(var));
//  return rnLat->getNprocsDep();
//}
//

////=============================================================================================
//// Sets nonDet and nprocsDep to true if some variable in the expression depends on
//// the process' rank or the number of processes, respectively. False otherwise.
//bool isCondMpiDep(const Function& func, const DataflowNode& n)
//{
//  ROSE_ASSERT(nonDetAnal);
//
////  std::cerr << "n.getNode()=" << n.getNode()->unparseToString() << endl;
//  varIDSet exprVars = getVarRefsInSubtree(n.getNode());
//
//  for(varIDSet::iterator it = exprVars.begin(); it!=exprVars.end(); it++)
//  {
//    printf("        var %s\n", (*it).str().c_str());
//  }
//
//  NodeState* state = NodeState::getNodeState(n, 0);
//  FiniteVarsExprsProductLattice* prodLat =
//      dynamic_cast<FiniteVarsExprsProductLattice*>(state->getLatticeBelow(nonDetAnalysis, 0));
//
//  // initialize both flags to false
//  bool rankDep = false;
//  bool nprocsDep = false;
//  for(varIDSet::iterator it = exprVars.begin(); it != exprVars.end(); it++)
//  {
//    if(prodLat->getVarLattice(*it))
//      Dbg::dbg << "prodLat-&lt;getVarLattice(func, " << (*it) << ") = "
//               << prodLat->getVarLattice(*it)->str() << endl;
//    else
//      Dbg::dbg << "prodLat-&lt;getVarLattice(func, " << (*it) << ") = "
//               << prodLat->getVarLattice(*it) << endl;
//    MPINonDetLattice* rnLat = dynamic_cast<MPINonDetLattice*>(prodLat->getVarLattice(*it));
//    rankDep   = rankDep   || rnLat->getRankDep();
//    nprocsDep = nprocsDep || rnLat->getNprocsDep();
//    if(rankDep && nprocsDep)
//      break;
//  }
//
//  return rankDep || nprocsDep;
//}


////=============================================================================================
//// Sets nonDet and nprocsDep to true if some variable in the expression depends on
//// the process' rank or the number of processes, respectively. False otherwise.
//bool isMPIDep(const Function& func, const DataflowNode& n, bool& rankDep, bool& nprocsDep)
//{
//  ROSE_ASSERT(nonDetAnal);
//
//  //Dbg::dbg << "n.getNode()="<<n.getNode()->unparseToString()<<endl;
//  varIDSet exprVars = getVarRefsInSubtree(n.getNode());
//
//  /*for(varIDSet::iterator it = exprVars.begin(); it!=exprVars.end(); it++)
//  { printf("        var %s\n", (*it).str().c_str()); }*/
//
//  NodeState* state = NodeState::getNodeState(n, 0);
//  FiniteVarsExprsProductLattice* prodLat =
//      dynamic_cast<FiniteVarsExprsProductLattice*>(state->getLatticeBelow(nonDetAnalysis, 0));
//
//  // initialize both flags to false
//  rankDep = false;
//  nprocsDep = false;
//  for(varIDSet::iterator it = exprVars.begin(); it != exprVars.end(); it++)
//  {
//    if(prodLat->getVarLattice(*it))
//      Dbg::dbg << "prodLat-&lt;getVarLattice(func, " << (*it) << ") = "
//               << prodLat->getVarLattice(*it)->str() << endl;
//    else
//      Dbg::dbg << "prodLat-&lt;getVarLattice(func, " << (*it) << ") = "
//               << prodLat->getVarLattice(*it) << endl;
//    MPINonDetLattice* rnLat = dynamic_cast<MPINonDetLattice*>(prodLat->getVarLattice(*it));
//    rankDep   = rankDep   || rnLat->getRankDep();
//    nprocsDep = nprocsDep || rnLat->getNprocsDep();
//    if(rankDep && nprocsDep)
//      break;
//  }
//
//  return rankDep || nprocsDep;
//}
//
////=============================================================================================
//// Returns whether some variable at the given DataflowNode depends on the process' rank
//bool isMPIRankDep(const Function& func, const DataflowNode& n)
//{
//  bool rankDep, nprocsDep;
//  isMPIDep(func, n, rankDep, nprocsDep);
//  return rankDep;
//}
//
////=============================================================================================
//// Returns whether some variable at the given DataflowNode depends on the number of processes
//bool isMPINprocsDep(const Function& func, const DataflowNode& n)
//{
//  bool rankDep, nprocsDep;
//  isMPIDep(func, n, rankDep, nprocsDep);
//  return nprocsDep;
//}
