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
  this->condition_ = that->condition_;
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
  bool old_cond = condition_;
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
    condition_ = condition_ || that->condition_;
    if(this->non_det_function_node_ == NULL)
      non_det_function_node_ = that->non_det_function_node_;


    //TODO what if this and that node are != NULL

  }

  return (old_init != initialized_) ||
         (old_dep != dependent_) ||
         (old_cond != condition_) ||
         (old_node != non_det_function_node_);
}

bool MPINonDetLattice::operator==(Lattice* that_arg)
{
  MPINonDetLattice* that = dynamic_cast<MPINonDetLattice*>(that_arg);

  return (initialized_ == that->initialized_) &&
         (dependent_ == that->dependent_) &&
         (condition_ == that->condition_) &&
         (non_det_function_node_ == that->non_det_function_node_);
}

// returns the current state of this object
bool MPINonDetLattice::getDep() const
{
  return dependent_;
}

bool MPINonDetLattice::getCond() const
{
  return condition_;
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

bool MPINonDetLattice::setCond(bool cond)
{
  bool modified = this->condition_ != cond;
  this->condition_ = cond;
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
  condition_ = false;
  non_det_function_node_ = NULL;
  return modified;
}


string MPINonDetLattice::str(string indent)
{
  ostringstream outs;
  if(initialized_)
    outs << indent << "[MPINonDetLattice: NonDetDep = "
         << (dependent_?"true ":"false ")
         << " ConditionDep = " << (condition_?"true ":"false ")
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

  FiniteVarsExprsProductLattice* prodLat =
      new FiniteVarsExprsProductLattice(new MPINonDetLattice(), constVars, NULL, ldva, n, state);
//  std::cerr << "prodLat = "<<prodLat->str("    ")<<endl;
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
//
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
//
//
//
//  //if(isSgFunctionCallExp(sgn))
//  //  if( isNonDetFunctionCall )
//  //    reset function call variables
//  //
//
////  // Process any calls to MPI_Comm_rank and MPI_Comm_size to identify any new
////  // dependencies on the process' rank or the number of processes
////  if(isSgFunctionCallExp(sgn))
////  {
////
////
////
////  // Binary operations: lhs=rhs, lhs+=rhs, lhs+rhs, ...
////  } else if(isSgBinaryOp(sgn)) {
////    // Memory objects denoted by the expression�s left- and right-hand
////    // sides as well as the SgAssignOp itself
////    varID lhs = SgExpr2Var(isSgBinaryOp(sgn)->get_lhs_operand());
////    varID rhs = SgExpr2Var(isSgBinaryOp(sgn)->get_rhs_operand());
////    varID res = SgExpr2Var(isSgBinaryOp(sgn));
////
////    // The lattices associated the three memory objects
////    MPINonDetLattice* resLat =
////            dynamic_cast<MPINonDetLattice*>(prodLat->getVarLattice(res));
////    MPINonDetLattice* lhsLat =
////            dynamic_cast<MPINonDetLattice*>(prodLat->getVarLattice(lhs));
////    MPINonDetLattice* rhsLat =
////            dynamic_cast<MPINonDetLattice*>(prodLat->getVarLattice(rhs));
////    ROSE_ASSERT(rhsLat);
////
////    // Assignment: lhs = rhs, lhs+=rhs, lhs*=rhs,  lhs/=rhs, ...
////    //    dependence flows from rhs to lhs and res
////    if(isSgAssignOp(sgn))
////    {
////      // If the lhs and/or the SgAssignOp are live, copy lattice from the rhs
////      if(lhsLat){ lhsLat->copy(rhsLat); modified = true; }
////      if(resLat){ resLat->copy(rhsLat); modified = true; }
////    }
////    else if(isSgCompoundAssignOp(sgn))
////    {
////      if(lhsLat) {
////        lhsLat->meetUpdate(rhsLat);
////        modified = true;
////      }
////      if(resLat) {
////        // 'lhs' must be alive here, and thus provide a lattice value, beacuse 'res' depends on it
////        ROSE_ASSERT(lhsLat);
////        resLat->copy(lhsLat);
////        modified = true;
////      }
////    }
////    // Non-assignments that do not refer to variables: lhs+rhs
////    // dependence flows from lhs and rhs to res
////    else {
////            if(resLat) {
////                    resLat->copy(rhsLat);
////                    resLat->meetUpdate(lhsLat);
////                    modified = true;
////            }
////    }
////    // NOTE: we need to deal with expressions such as a.b, a->b and a[i] specially since they refer to memory locations,
////    //       especially sub-expressions (e.g. a.b for a.b.c.d or a[i] for a[i][j][k]) but we don't yet have good abstraction for this.
////  // Unary operations
////  } else if(isSgUnaryOp(sgn)) {
////    if(!isSgAddressOfOp(sgn)) {
////      // Memory objects denoted by the expression�s oprand as well as the expression itself
////      varID op = SgExpr2Var(isSgUnaryOp(sgn)->get_operand());
////      varID res = SgExpr2Var(isSgUnaryOp(sgn));
////
////      // The lattices associated the three memory objects
////      MPINonDetLattice* opLat =
////              dynamic_cast<MPINonDetLattice*>(prodLat->getVarLattice(op));
////      MPINonDetLattice* resLat =
////              dynamic_cast<MPINonDetLattice*>(prodLat->getVarLattice(res));
////
////      ROSE_ASSERT(opLat);
////
////      // Copy lattice from the operand
////      resLat->copy(opLat);
////      modified = true;
////    }
////  // Conditional Operators: (x? y: z)
////  } else if(isSgConditionalExp(sgn)) {
////    // Memory objects denoted by the expression�s condition, true and false sub-expressions
////    varID condE  = SgExpr2Var(isSgConditionalExp(sgn)->get_conditional_exp());
////    varID trueE  = SgExpr2Var(isSgConditionalExp(sgn)->get_true_exp());
////    varID falseE = SgExpr2Var(isSgConditionalExp(sgn)->get_false_exp());
////    varID res    = SgExpr2Var(isSgConditionalExp(sgn));
////
////    // The lattices associated the three memory objects
////    MPINonDetLattice* resLat =
////            dynamic_cast<MPINonDetLattice*>(prodLat->getVarLattice(res));
////    MPINonDetLattice* condLat =
////            dynamic_cast<MPINonDetLattice*>(prodLat->getVarLattice(condE));
////    MPINonDetLattice* trueLat =
////            dynamic_cast<MPINonDetLattice*>(prodLat->getVarLattice(trueE));
////    MPINonDetLattice* falseLat =
////            dynamic_cast<MPINonDetLattice*>(prodLat->getVarLattice(falseE));
////    ROSE_ASSERT(condLat); ROSE_ASSERT(trueLat); ROSE_ASSERT(falseLat);
////
////    // Dependence flows from the sub-expressions of the SgConditionalExp to res
////    if(resLat) {
////      resLat->copy(condLat);
////      resLat->meetUpdate(trueLat);
////      resLat->meetUpdate(falseLat);
////      modified = true;
////    }
////  // Variable Declaration
////  } else if(isSgInitializedName(sgn)) {
////    varID var(isSgInitializedName(sgn));
////    MPINonDetLattice* varLat = dynamic_cast<MPINonDetLattice*>(prodLat->getVarLattice(var));
////
////    // If this variable is live
////    if(varLat) {
////      // If there was no initializer, initialize its lattice to Bottom
////      if(isSgInitializedName(sgn)->get_initializer()==NULL)
////        modified = varLat->setToBottom() || modified;
////      // Otherwise, copy the lattice of the initializer to the variable
////      else {
////        varID init = SgExpr2Var(isSgInitializedName(sgn)->get_initializer());
////        MPINonDetLattice* initLat = dynamic_cast<MPINonDetLattice*>(prodLat->getVarLattice(init));
////        ROSE_ASSERT(initLat);
////        varLat->copy(initLat);
////        modified = true;
////      }
////    }
////  // Initializer for a variable
////  } else if(isSgAssignInitializer(sgn)) {
////    // Memory objects of the initialized variable and the
////    // initialization expression
////    varID res = SgExpr2Var(isSgAssignInitializer(sgn));
////    varID asgn = SgExpr2Var(isSgAssignInitializer(sgn)->get_operand());
////
////    // The lattices associated both memory objects
////    MPINonDetLattice* resLat = dynamic_cast<MPINonDetLattice*>(prodLat->getVarLattice(res));
////    MPINonDetLattice* asgnLat = dynamic_cast<MPINonDetLattice*>(prodLat->getVarLattice(asgn));
////    ROSE_ASSERT(resLat);
////
////    // Copy lattice from the assignment
////    resLat->copy(asgnLat);
////    modified = true;
////  }
//
////  std::cerr << "##TRANSFER## prodLat = "<<prodLat->str("    ")<<endl;
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

  FiniteVarsExprsProductLattice* prodLat =
      dynamic_cast<FiniteVarsExprsProductLattice*>(*(dfInfo.begin()));

  // make sure that all the non-constant Lattices are initialized
  for(vector<Lattice*>::const_iterator it = prodLat->getLattices().begin();
      it!=prodLat->getLattices().end();
      it++)
    (dynamic_cast<MPINonDetLattice*>(*it))->initialize();

  SgNode *sgn = n.getNode();

  if(isSgFunctionCallExp(sgn))
  {
    //Dbg::dbg << "    isSgFunctionCallExp"<<endl;
    SgFunctionCallExp* fnCall = isSgFunctionCallExp(sgn);
    Function calledFunc(fnCall);

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

//      printf("arg1 = [%s | %s]]\n", arg3->class_name().c_str(), arg3->unparseToString().c_str());

      //If MPI_ANY_SOURCE
      if( isSgMinusOp(arg3) &&
          isSgIntVal(isSgMinusOp(arg3)->get_operand()) &&
          isSgIntVal(isSgMinusOp(arg3)->get_operand())->get_value() == 1 )
      {
        // check argument 0 (buffer)
        // printf("arg0 = [%s | %s]]\n",arg0->class_name().c_str(),arg0->unparseToString().c_str());
        //TODO: This is not very general (sub tree node query).
        SgNode* n0 = arg0;
        while(isSgUnaryOp(n0))
          n0=isSgUnaryOp(n0)->get_operand();

        if(varID::isValidVarExp(n0))
        {
          Dbg::dbg << "Traverse: At MPI_Recv Node after SgMinusOp!\n";
          varID var_arg0 = SgExpr2Var(isSgExpression(n0));
          MPINonDetLattice* var0_lat =
              dynamic_cast<MPINonDetLattice*>(prodLat->getVarLattice(var_arg0));
          // Dbg::dbg << "var_arg0=" << var_arg0 << " var0_lat=" << var0_lat << endl;
          // Dbg::dbg << "prodLat=" << prodLat->str("    ") << endl;
          modified = var0_lat->setDep(true) || modified;
          modified = var0_lat->setDepNode(sgn) || modified;
          Dbg::dbg << "prodLat=" << prodLat->str("    ") << endl;
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
          Dbg::dbg << "Traverse: At MPI_Recv Node after SgMinusOp!\n";
          varID var_arg6 = SgExpr2Var(isSgExpression(n6));
          MPINonDetLattice* var6_lat =
              dynamic_cast<MPINonDetLattice*>(prodLat->getVarLattice(var_arg6));
          modified = var6_lat->setDep(true) || modified;
          modified = var6_lat->setDepNode(sgn) || modified;
        }

      }
      else
      {
        std::cerr << "[nonDetAnalysis][Traverse]:Deterministic Recv()\n";
      }
    }
  }
  else if(isSgIfStmt(sgn)) // set used in conditional if any dependent variable used in conditional
  {
    Rose_STL_Container<SgNode*> cond_vars =
        NodeQuery::querySubTree(isSgIfStmt(sgn)->get_conditional(),V_SgVarRefExp);
    for(Rose_STL_Container<SgNode*>::iterator it = cond_vars.begin(); it != cond_vars.end(); it++)
    {
      Dbg::dbg << "  IfStmtNodes: " << isSgNode(*it)->unparseToString() << endl;
      varID var = SgExpr2Var(isSgExpression(*it));
      MPINonDetLattice* var_lat = dynamic_cast<MPINonDetLattice*>(prodLat->getVarLattice(var));
      if(var_lat->getDep())
      {
        //check for MPI function calls!
        if(isSgBasicBlock(isSgIfStmt(sgn)->get_true_body()))
        {
          Rose_STL_Container<SgNode*> t_fun_nodes =
              NodeQuery::querySubTree(isSgIfStmt(sgn)->get_true_body(),V_SgFunctionCallExp);
          for( Rose_STL_Container<SgNode*>::iterator t_it = t_fun_nodes.begin();
               t_it != t_fun_nodes.end();
               ++t_it)
          {
            if(isMpiFunctionCall(*t_it))
            {
              if(addNonDetNode(var_lat->getDepNode()))
              {
                std::cerr << "Source of Non Det: "
                    << isSgNode(var_lat->getDepNode())->unparseToString() << endl
                    << "  File name: "
                    << isSgNode(var_lat->getDepNode())->get_file_info()->get_filenameString()
                    << "\n  Line Number: "
                    << isSgNode(var_lat->getDepNode())->get_file_info()->get_line() << endl
                    << endl;
              }
              t_it = t_fun_nodes.end();
              t_it--;
            }
          }
        }
        if(isSgBasicBlock(isSgIfStmt(sgn)->get_false_body()))
        {
          Rose_STL_Container<SgNode*> f_fun_nodes =
                      NodeQuery::querySubTree(isSgIfStmt(sgn)->get_false_body(),V_SgFunctionCallExp);
          for( Rose_STL_Container<SgNode*>::iterator f_it = f_fun_nodes.begin();
               f_it != f_fun_nodes.end();
               ++f_it)
          {
            if(isMpiFunctionCall(*f_it))
            {
              if(addNonDetNode(var_lat->getDepNode()))
              {
                std::cerr << "Source of Non Det: "
                    << isSgNode(var_lat->getDepNode())->unparseToString() << endl
                    << "  File name: "
                    << isSgNode(var_lat->getDepNode())->get_file_info()->get_filenameString()
                    << "\n  Line Number: "
                    << isSgNode(var_lat->getDepNode())->get_file_info()->get_line() << endl
                    << endl;
              }
              f_it = f_fun_nodes.end();
              f_it--;
            }
          }
        }
      }
    }
  }
  // Binary operations: lhs=rhs, lhs+=rhs, lhs+rhs, ...
  else if(isSgBinaryOp(sgn)) {
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
                              LiveDeadVarsAnalysis* ldva,
                              string indent)
//MPINonDetAnalysis* runMPINonDetAnalysis(SgIncidenceDirectedGraph* graph, string indent)
{
//  nonDetAnal = new MPINonDetAnalysis(ldva);
  nonDetAnal = new MPINonDetAnalysis();
  nonDetAnalysis = nonDetAnal;
  nonDetAnal_inter = new ContextInsensitiveInterProceduralDataflow(nonDetAnal, graph);
  nonDetAnal_inter->runAnalysis();

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
