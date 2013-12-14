#include "rose.h"
#include "constPropAnalysis/constantPropagationAnalysis.h"
#include "liveDeadVarAnalysis.h"
#include "mpiCFG/mpiCFG.h"

int ConstPropAnalysisTestDebugLevel = 3;

//=======================================================================================
bool cfgNodeFilter (CFGNode cfgn)
{
  SgNode * node = cfgn.getNode();
  assert (node != NULL) ;
  bool ret = true;
  if (isSgInitializedName(node))
    ret = (cfgn == node->cfgForEnd());
  else
    ret = (cfgn.isInteresting());

  if(ConstPropAnalysisTestDebugLevel > 3)
  {
    if(ret)
      std::cerr << "#";
    else
      std::cerr << "_";
  }

  //TODO: remove that!
  ret = true;
  //..................
  return ret;
}

//=======================================================================================
bool cfgNodeFilterAltered (CFGNode cfgn)
{
  SgNode * node = cfgn.getNode();
  assert (node != NULL) ;
  bool ret = true;
  if (isSgInitializedName(node))
    ret = (cfgn == node->cfgForEnd());
  else
    ret = (cfgn.isInteresting());

  if(ConstPropAnalysisTestDebugLevel > 3)
  {
    if(ret)
      std::cerr << "#";
    else
      std::cerr << "_";
  }

  //TODO: remove that!
  ret = false;
  //..................
  return ret;
}

//=======================================================================================
class EvaluationPass: public UnstructuredPassIntraAnalysis
{
public:
    ConstantPropagationAnalysis *cpa;
    int pass, fail;
    bool failure;
    bool (*filter) (CFGNode cfgn);

//    EvaluationPass(LoopAnalysis* that_la, bool (*f)(CFGNode))
    EvaluationPass(ConstantPropagationAnalysis* that_cpa, bool (*f)(CFGNode))
        : Analysis(f), cpa(that_cpa), filter(f)
    {
      pass = 0;
      fail = 0;
      failure = false;
    }

    void visit(const Function& func, const DataflowNode& n, NodeState& state);
};

//=======================================================================================
void EvaluationPass::visit(const Function& func, const DataflowNode& n, NodeState& state)
{
  if(ConstPropAnalysisTestDebugLevel >= 3)
    std::cerr << "\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++";
  bool visit_fail = false;

//  CFGNode cfg = CFGNode(n.n.getNode(),(unsigned int)n.n.getIndex());
//  DataflowNode dfn = DataflowNode(cfg, cfgNodeFilterAltered);
//  NodeState* new_state = NodeState::getNodeState(dfn, 0);
//  NodeState* new_state1;
//  NodeState* new_state2;
//  if(n.getIndex() >= 1)
//  {
//    CFGNode cfg1 = CFGNode(n.getNode(),(unsigned int)n.getIndex());
//    DataflowNode dfn1 = DataflowNode(cfg1, cfgNodeFilterAltered);
//    new_state1 = NodeState::getNodeState(dfn1, 1);
//  }
//  else
//    new_state1 = NodeState::getNodeState(n, n.getIndex());
//
//  if(n.getIndex() >= 2)
//    new_state2 = NodeState::getNodeState(n, 2);
//  else
//    new_state2 = NodeState::getNodeState(n, n.getIndex());

#if 1
  SgNode* node = n.n.getNode();

  //int idx = n.n.getIndex();
  //0 is the only index working for certain cfg nodes....
//  NodeState* nstate = NodeState::getNodeState(node, 0);
  CFGNode cfg = CFGNode(n.n.getNode(),(unsigned int)n.n.getIndex());
  DataflowNode dfn = DataflowNode(cfg, cfgNodeFilterAltered);
  std::cerr << "\n Number of NodeStates associated with the DFN: "
            << NodeState::numNodeStates(dfn);
  const vector<NodeState*> state_v = NodeState::getNodeStates(n);
  std::cerr << "  and " << state_v.size();

  NodeState* nstate = NodeState::getNodeState(dfn, 0);

//  if(&state == nstate)
//  {
//    std::cerr << "\nJUHU ALL GOOD: " << n.getIndex() << " ";
//    std::cerr << "  STATEptr: " << &state
//              << "  nSTATEptr: " << nstate
//              << "  newSTATEptr0: " << new_state
//              << "  newSTATEptr1: " << new_state1
//              << "  newSTATEptr2: " << new_state2;
//  }
//  else
//  {
//    std::cerr << "\nPROBLEMO, Index: " << n.getIndex() << " ";
//    std::cerr << "  STATEptr: " << &state
//              << "  nSTATEptr: " << nstate
//              << "  newSTATEptr0: " << new_state
//              << "  newSTATEptr1: " << new_state1
//              << "  newSTATEptr2: " << new_state2;
//  }



  if(nstate->isInitialized(cpa))
  {
    if(isSgVarRefExp(node))
    {
      if(ConstPropAnalysisTestDebugLevel >= 2)
      {
        std::cerr << "\nisSgVarRefExp:  <"
                  << n.n.toString()
                  << " col: " << isSgLocatedNode(n.n.getNode())->get_file_info()->get_col()
                  << " >";
      }

      if(!(NodeState::getLatticeAbove(cpa, node,0).size() > 0))
      {
       std::cerr<< "....exit on error.....";
       visit_fail = true;
      }
      else
      {
        std::cerr << "\nHAS POSSIBLE CONSTANT VALUE";
        VarsExprsProductLattice* lattice =
         dynamic_cast <VarsExprsProductLattice *>
           (NodeState::getLatticeAbove(cpa, node,0)[0]);

        if(lattice != NULL)
        {
          ConstantPropagationLattice* varlattice =
              dynamic_cast <ConstantPropagationLattice*>
                                    (lattice->getVarLattice(varID(n.n.getNode())));

          if(varlattice != NULL)
          {
            if(varlattice->getLevel() == (short)2) //TODO 2 is not a const value to be here
              if(ConstPropAnalysisTestDebugLevel >= 1)
                std::cerr << "  A REAL CONST VALUE HERE: " << varlattice->getValue();
            this->pass++;
          }
          else
          {
            std::cerr << "\nFAIL AT VAR LATTICE != NULL\n";
            visit_fail = true;
          }
        }
        else
        {
          std::cerr << "\nFAIL AT LATTICE != NULL\n";
          visit_fail = true;
        }
      }
    }
    else
    {
      std::cerr << "\nJustPass";
      this->pass++;
    }
  }
#else
  if(state.isInitialized(cpa))
  {
    // I: access lattice directly trough the node state
      //and
    //II: access lattice vial DataflowNode -> CFGNode -> SgNode -> CFGNode -> NodeState

    if(isSgVarRefExp(n.n.getNode()))
    {
      if(ConstPropAnalysisTestDebugLevel >= 2)
        std::cerr << "\nisSgVarRefExp:  <" << n.n.toString()
        << " col: " << isSgLocatedNode(n.n.getNode())->get_file_info()->get_col() << " >";

      //    if(!(NodeState::getLatticeAbove(const_prop_, node,0).size() > 0))
      //    {
      //      std::cerr<< "....exit on error.....";
      //      return false;
      //    }

      //    std::cerr << "\n\n\nHAS POSSIBLE CONSTANT VALUE \n";
      std::vector<Lattice*> latt_vec = state.getLatticeAbove(cpa);
      VarsExprsProductLattice* lattice =
//          dynamic_cast<VarsExprsProductLattice*> latt;
          dynamic_cast<VarsExprsProductLattice*> (latt_vec[0]);
//          dynamic_cast <VarsExprsProductLattice*> (state.getLatticeAbove(cpa));

      if(lattice != NULL)
      {
        ConstantPropagationLattice* varlattice =
            dynamic_cast <ConstantPropagationLattice*>
                                  (lattice->getVarLattice(varID(n.n.getNode())));

        if(varlattice != NULL)
        {
          if(varlattice->getLevel() == (short)2) //TODO 2 is not a const value to be here
            if(ConstPropAnalysisTestDebugLevel >= 1)
              std::cerr << "  A REAL CONST VALUE HERE: " << varlattice->getValue();
          this->pass++;
        }
        else
        {
          std::cerr << "\nFAIL AT VAR LATTICE != NULL\n";
          visit_fail = true;
        }
      }
      else
      {
        std::cerr << "\nFAIL AT LATTICE != NULL\n";
        visit_fail = true;
      }
    }
    else
    {
      std::cerr << "\nJustPass";
      this->pass++;
    }
  }
#endif
  else
  {
    std::cerr << "\nFAIL AT state.CPA is not Initialized\n";
    visit_fail = true;
  }

  if(visit_fail)
  {
    this->failure = true;
    this->fail++;
  }
  if(ConstPropAnalysisTestDebugLevel >= 3)
    std::cerr << "\n==============================================================";
}

//=======================================================================================
int main(int argc, char *argv[])
{
  // Build the AST used by ROSE
  SgProject* project = frontend(argc,argv);
  ROSE_ASSERT (project != NULL);

  std::cerr << "\n## Init Analysis: ...";
  initAnalysis(project);
  Dbg::init("Constant Propagation ANALYSIS TEST", ".", "index.html");
//  loopAnalysisDebugLevel = 0;
  liveDeadAnalysisDebugLevel = 0;
  analysisDebugLevel = 0;
//  rrankAnalysisDebugLevel = 2;

  std::cerr << "\n## Call Graph Builder;";
  CallGraphBuilder cgb(project);
  cgb.buildCallGraph();
  SgIncidenceDirectedGraph* graph = cgb.getGraph();

  std::cerr << "\n## LiveDeadVars Analysis";
  LiveDeadVarsAnalysis ldva(project);
  UnstructuredPassInterDataflow ciipd_ldva(&ldva);
  ciipd_ldva.runAnalysis();

  std::cerr << "\n## Constant Propagation Analysis";
  ConstantPropagationAnalysis cpA(&ldva);
  ContextInsensitiveInterProceduralDataflow cpInter(&cpA, graph);
  cpInter.runAnalysis();

  if(ConstPropAnalysisTestDebugLevel >= 1)
  {
    std::cerr << "\n## Evaluation Pass:";
    EvaluationPass ep(&cpA, cfgNodeFilter);
    UnstructuredPassInterAnalysis upia_ep(ep);
    upia_ep.runAnalysis();

    if(ep.failure)
    {
        std::cerr << "\nTEST FAIL\n";
        std::cerr << ep.fail << " lattice was not defined at certain nodes.\n";
        std::cerr << ep.pass << " Passe!\n";
    }
    else
    {
        std::cerr << "\nTEST PASSED\n";
        std::cerr << ep.pass << " nodes passed successfully!\n";
        std::cerr << ep.fail << " Fails!\n";
    }
  }

  return 0;
}
