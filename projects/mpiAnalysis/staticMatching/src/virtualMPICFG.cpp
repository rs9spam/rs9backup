// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "virtualMPICFG.h"
#ifndef _MSC_VER
#include <err.h>
#endif

// DQ (10/14/2010):  This should only be included by source files that require it.
// This fixed a reported bug which caused conflicts with autoconf macros (e.g. PACKAGE_BUGREPORT).
#include "rose_config.h"

using namespace std;

namespace VirtualMPICFG {

//#if !CXX_IS_ROSE_CODE_GENERATION

//  unsigned int cfgIndexForEndWrapper(SgNode* n) {
//    return n->cfgIndexForEnd();
//  }


//  CFGNode::CFGNode(SgNode* node, unsigned int index): node(node), index(index) {
//#ifndef _MSC_VER
//    assert (!node || isSgStatement(node) || isSgExpression(node) || isSgInitializedName(node) || isSgOmpClause(node) || isSgOmpVariablesClause(node)  );
//
//    // Liao 11/8/2010, defUseAnalysis/DefUseAnalysis_perFunction.cpp calls CFGNode(NULL, 0), which triggers the warning unnecessarily
//    //if (!(node && index <= node->cfgIndexForEnd())) {
//    if (node && (index > node->cfgIndexForEnd())) {
//      warnx ("created CFGNode with illegal index:");
//      cout<<"index = " << index <<" while max index is:"<< node->cfgIndexForEnd() <<endl;
//      SageInterface::dumpInfo(node);
//      assert (false);
//    }
//#endif
//  }



//  string CFGEdge::toString() const {
//    return toStringForDebugging();
//  }

//  string CFGEdge::toStringForDebugging() const {
//    ostringstream s;
//    // s << src.id() << " -> " << tgt.id();
//    bool anyNonEmpty = false;
//    EdgeConditionKind cond = condition();
//    if (cond != eckUnconditional) {
//      if (anyNonEmpty) s << " "; // For consistency
//      s << "key(";
//      switch (cond) {
//        case eckTrue:
//          s << "true";
//          break;
//        case eckFalse:
//          s << "false";
//          break;
//        case eckCaseLabel:
//          s << caseLabel()->unparseToString();
//          break;
//        case eckDefault:
//          s << "default";
//          break;
//        case eckDoConditionPassed:
//          s << "do passed";
//          break;
//        case eckDoConditionFailed:
//          s << "do failed";
//          break;
//        case eckForallIndicesInRange:
//          s << "indices in range";
//          break;
//        case eckForallIndicesNotInRange:
//          s << "indices not in range";
//          break;
//        case eckComputedGotoCaseLabel:
//          s << computedGotoCaseIndex();
//          break;
//        case eckArithmeticIfLess:
//          s << "less";
//          break;
//        case eckArithmeticIfEqual:
//          s << "equal";
//          break;
//        case eckArithmeticIfGreater:
//          s << "greater";
//          break;
//        case eckInterprocedural:  //interprocedural
//          s << "interprocedural";
//          break;
//        case eckMPI:              // mpi edges
//          s << "mpi_call";
//          break;
//        default:
//          s << "unknown";
//          break;
//      }
//      s << ")";
//      anyNonEmpty = true;
//    }
//    return s.str();
//  }







//  static vector<SgInitializedName*>
//  findVariablesDirectlyInScope(SgStatement* sc) {
//    // This can't be done with a traversal because it needs to avoid entering
//    // inner scope statements
//    vector<SgInitializedName*> variablesInScope;
//    vector<SgNode*> succs = sc->get_traversalSuccessorContainer();
//    for (int i = (int)succs.size() - 1; i > -1; --i) {
//      if (isSgInitializedName(succs[i])) {
//        variablesInScope.push_back(isSgInitializedName(succs[i]));
//      } else if (isSgVariableDeclaration(succs[i])) {
//        SgVariableDeclaration* vd = isSgVariableDeclaration(succs[i]);
//        const SgInitializedNamePtrList& vars = vd->get_variables();
//        variablesInScope.insert(variablesInScope.end(), vars.rbegin(), vars.rend());
//      } else if (isSgForInitStatement(succs[i])) {
//        vector<SgInitializedName*> initVars = findVariablesDirectlyInScope(isSgForInitStatement(succs[i]));
//        variablesInScope.insert(variablesInScope.end(), initVars.begin(), initVars.end());
//      } else if (isSgFunctionParameterList(succs[i])) {
//        SgFunctionParameterList* pl = isSgFunctionParameterList(succs[i]);
//        const SgInitializedNamePtrList& params = pl->get_args();
//        variablesInScope.insert(variablesInScope.end(), params.begin(), params.end());
//      }
//    }
//    return variablesInScope;
//  }



//  EdgeConditionKind CFGEdge::condition() const {
//    SgNode* srcNode = src.getNode();
//    unsigned int srcIndex = src.getIndex();
//    SgNode* tgtNode = tgt.getNode();
//    unsigned int tgtIndex = tgt.getIndex();
//    if (isSgIfStmt(srcNode) && srcIndex == 1) {
//      SgIfStmt* ifs = isSgIfStmt(srcNode);
//      if (ifs->get_true_body() == tgtNode) {
//        return eckTrue;
//      } else if (tgtNode != NULL && ifs->get_false_body() == tgtNode) {
//        return eckFalse;
//      } else if (ifs->get_false_body() == NULL && tgtNode == ifs && tgtIndex == 2) {
//        return eckFalse;
//      } else ROSE_ASSERT (!"Bad successor in if statement");
//    } else if (isSgArithmeticIfStatement(srcNode) && srcIndex == 1) {
//      SgArithmeticIfStatement* aif = isSgArithmeticIfStatement(srcNode);
//      if (getCFGTargetOfFortranLabelRef(aif->get_less_label()) == tgt) {
//        return eckArithmeticIfLess;
//      } else if (getCFGTargetOfFortranLabelRef(aif->get_equal_label()) == tgt) {
//        return eckArithmeticIfEqual;
//      } else if (getCFGTargetOfFortranLabelRef(aif->get_greater_label()) == tgt) {
//        return eckArithmeticIfGreater;
//      } else ROSE_ASSERT (!"Bad successor in arithmetic if statement");
//    } else if (isSgWhileStmt(srcNode) && srcIndex == 1) {
//      if (srcNode == tgtNode) {
//        // False case for while test
//        return eckFalse;
//      } else {
//        return eckTrue;
//      }
//    } else if (isSgDoWhileStmt(srcNode) && srcIndex == 2) {
//      // tgtIndex values are 0 for true branch and 3 for false branch
//      if (tgtIndex == 0) {
//        return eckTrue;
//      } else {
//        return eckFalse;
//      }
//    } else if (isSgForStatement(srcNode) && srcIndex == 2) {
//      if (srcNode == tgtNode) {
//        // False case for test
//        return eckFalse;
//      } else {
//        return eckTrue;
//      }
//    } else if (isSgFortranDo(srcNode) && srcIndex == 3) {
//      if (tgtIndex == 4) {
//        return eckDoConditionPassed;
//      } else if (tgtIndex == 6) {
//        return eckDoConditionFailed;
//      } else ROSE_ASSERT (!"Bad successor in do loop");
//    } else if (isSgForAllStatement(srcNode) && srcIndex == 1) {
//      if (tgtIndex == 2) {
//        return eckForallIndicesInRange;
//      } else if (tgtIndex == 7) {
//        return eckForallIndicesNotInRange;
//      } else ROSE_ASSERT (!"Bad successor in forall loop");
//    } else if (isSgForAllStatement(srcNode) && srcIndex == 3) {
//      if (SageInterface::forallMaskExpression(isSgForAllStatement(srcNode))) {
//        if (tgtIndex == 4) {
//          return eckTrue;
//        } else if (tgtIndex == 6) {
//          return eckFalse;
//        } else ROSE_ASSERT (!"Bad successor in forall loop");
//      } else {
//        return eckUnconditional;
//      }
//    } else if (isSgSwitchStatement(srcNode) && isSgCaseOptionStmt(tgtNode)) {
//      return eckCaseLabel;
//    } else if (isSgSwitchStatement(srcNode) && isSgDefaultOptionStmt(tgtNode)){
//      return eckDefault;
//    } else if (isSgComputedGotoStatement(srcNode) && srcIndex == 1) {
//      if (tgtNode == srcNode) {
//        return eckDefault;
//      } else {
//        return eckCaseLabel;
//      }
//    } else if (isSgConditionalExp(srcNode) && srcIndex == 1) {
//      SgConditionalExp* ce = isSgConditionalExp(srcNode);
//      if (ce->get_true_exp() == tgtNode) {
//        return eckTrue;
//      } else if (ce->get_false_exp() == tgtNode) {
//        return eckFalse;
//      } else ROSE_ASSERT (!"Bad successor in conditional expression");
//    } else if (isSgAndOp(srcNode) && srcIndex == 1) {
//      if (srcNode == tgtNode) {
//        // Short-circuited false case
//        return eckFalse;
//      } else {
//        return eckTrue;
//      }
//    } else if (isSgOrOp(srcNode) && srcIndex == 1) {
//      if (srcNode == tgtNode) {
//        // Short-circuited true case
//        return eckTrue;
//      } else {
//        return eckFalse;
//      }
//    } else if (isSgFunctionCallExp(srcNode) &&
//               srcIndex == SGFUNCTIONCALLEXP_INTERPROCEDURAL_INDEX &&
//               !isSgFunctionCallExp(tgtNode)) {
//        return eckInterprocedural;
//    } else if (isSgFunctionCallExp(tgtNode) &&
//               tgtIndex == SGFUNCTIONCALLEXP_INTERPROCEDURAL_INDEX+1 &&
//               !isSgFunctionCallExp(srcNode)) {
//        return eckInterprocedural;
//    } else if (isSgConstructorInitializer(srcNode) &&
//               srcIndex == SGCONSTRUCTORINITIALIZER_INTERPROCEDURAL_INDEX &&
//               !isSgConstructorInitializer(tgtNode)) {
//        return eckInterprocedural;
//    } else if (isSgConstructorInitializer(tgtNode) &&
//               tgtIndex == SGCONSTRUCTORINITIALIZER_INTERPROCEDURAL_INDEX+1 &&
//               !isSgConstructorInitializer(srcNode)) {
//        return eckInterprocedural;
//    } else if (isSgFunctionRefExp(srcNode) && isSgFunctionRefExp(tgtNode) &&
//               (isSgFunctionRefExp(srcNode)->get_symbol()->get_name() == "MPI_Send") &&
//               (isSgFunctionRefExp(tgtNode)->get_symbol()->get_name() == "MPI_Recv")) {
//        return eckMPI;
//    } else {
//      // No key
//      return eckUnconditional;
//    }
//
// // DQ (11/29/2009): It should be an error to reach this point in the function.
//    ROSE_ASSERT(false);
// // DQ (11/29/2009): Avoid MSVC warning.
//    return eckFalse;
//  }






//  vector<CFGEdge> CFGNode::outEdges() const {
//    ROSE_ASSERT (node);
//    vector<CFGEdge> result = node->cfgOutEdges(index);
//    for ( vector<CFGEdge>::const_iterator i = result.begin(); i!= result.end(); i++)
//   {
//      CFGEdge e = *i;
//      assert (e.source().getNode() != NULL && e.target().getNode() != NULL);
//    }
//
//    return result;
//   // return node->cfgOutEdges(index);
//  }
//
//  vector<CFGEdge> CFGNode::inEdges() const {
//    ROSE_ASSERT (node);
//
//    vector<CFGEdge> result = node->cfgInEdges(index);
//   for ( vector<CFGEdge>::const_iterator i = result.begin(); i!= result.end(); i++)
//   {
//      CFGEdge e = *i;
//      assert (e.source().getNode() != NULL && e.target().getNode() != NULL);
//    }
//
//
//    //return node->cfgInEdges(index);
//    return result;
//  }
//









// if !CXX_IS_ROSE_CODE_GENERATION
//#endif

}
