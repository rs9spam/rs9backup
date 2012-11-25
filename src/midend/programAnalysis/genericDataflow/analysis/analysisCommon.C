#include "analysisCommon.h"

using namespace cfgUtils;

//static SgProject* project;
static SgIncidenceDirectedGraph* callGraph;

void initAnalysis(SgProject* project)
{
        //project = p;
        initCFGUtils(project);
        
        // Create the Call Graph
        CallGraphBuilder cgb(project);
        //std::cerr << "3.3*************************************************************\n";
        cgb.buildCallGraph();
        //std::cerr << "3.4*************************************************************\n";
        callGraph = cgb.getGraph(); 
        //std::cerr << "3.5*************************************************************\n";
        //GenerateDotGraph(graph, "test_example.callgraph.dot");
        
        // Create unique annotations on each expression to make it possible to assign each expression a unique variable name
        SageInterface::annotateExpressionsWithUniqueNames(project);
        //std::cerr << "3.6*************************************************************\n";
}

/*SgProject* getProject()
{
        return project;
}
*/
SgIncidenceDirectedGraph* getCallGraph()
{
  //std::cerr << "[analysisCommon.C]********* get Call Graph() *****************\n";
        return callGraph;
}
