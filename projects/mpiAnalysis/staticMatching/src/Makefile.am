include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SAF_SRC_ROOT   = $(top_srcdir)/src/midend/programAnalysis/genericDataflow/
#DFA_ROOT = $(top_srcdir)/tests/roseTests/programAnalysisTests/

# To keep compile commands tidy, please ONLY put required include flags here.
XXX_INCLUDES = $(ROSE_INCLUDES) \
           -I$(SAF_SRC_ROOT)/include \
           -I$(SAF_SRC_ROOT)/src/analysis \
           -I$(SAF_SRC_ROOT)/src/arrIndexLabeler \
           -I$(SAF_SRC_ROOT)/src/cfgUtils \
           -I$(SAF_SRC_ROOT)/src/chkptRangeAnalysis \
           -I$(SAF_SRC_ROOT)/src/common \
           -I$(SAF_SRC_ROOT)/src/external \
           -I$(SAF_SRC_ROOT)/src/lattice \
           -I$(SAF_SRC_ROOT)/src/mpiAnal \
           -I$(SAF_SRC_ROOT)/src/ompAnal \
           -I$(SAF_SRC_ROOT)/src/rwAccessLabeler \
           -I$(SAF_SRC_ROOT)/src/simpleAnalyses \
           -I$(SAF_SRC_ROOT)/src/state \
           -I$(SAF_SRC_ROOT)/src/unionFind \
           -I$(SAF_SRC_ROOT)/src/varBitVector \
           -I$(SAF_SRC_ROOT)/src/variables \
           -I$(SAF_SRC_ROOT)/src/varLatticeVector

INCLUDES = $(ROSE_INCLUDES) \
           -I$(srcdir) \
           -I$(SAF_SRC_ROOT) \
           -I$(SAF_SRC_ROOT)/analysis \
           -I$(SAF_SRC_ROOT)/arrIndexLabeler \
           -I$(SAF_SRC_ROOT)/cfgUtils \
           -I$(SAF_SRC_ROOT)/lattice \
           -I$(SAF_SRC_ROOT)/rwAccessLabeler \
           -I$(SAF_SRC_ROOT)/simpleAnalyses \
           -I$(SAF_SRC_ROOT)/state \
           -I$(SAF_SRC_ROOT)/variables

#\-I$(DFA_ROOT)/generalDataFlowAnalysisTests



bin_PROGRAMS = mpiAnalysis
#mpiAnalysis_SOURCES = mpiAnalysis.cpp virtualMPICFG.h virtualMPICFG.cpp staticMPICFG.h staticMPICFG.cpp mpiCFG.h mpiCFG.cpp
mpiAnalysis_SOURCES = mpiAnalysis.cpp mpiCFG.cpp mpiCFG.h constantPropagationAnalysis.cpp constantPropagationAnalysis.h rankAnalysis.cpp rankAnalysis.h pSet.cpp pSet.h RankLattice.cpp RankLattice.h
#liveDeadA_SOURCES = liveDeadA.C


#INCLUDES = $(ROSE_INCLUDES)
LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

C_TESTCODES_REQUIRED_TO_PASS = \
	../tests/btest.c \
	../tests/deadlock1.c \
	../tests/deadlock2.c \
	../tests/deadlock3.c \
	../tests/deadlock4.c \
	../tests/deadlock5.c \
	../tests/deadlock6.c \
	../tests/diffusion.c \
	../tests/diffusion2_nd.c \
	../tests/diffusion2_scaledown.c \
	../tests/diffusion2.c \
	../tests/dtg.c \
	../tests/GE.c \
	../tests/mat_mul.c \
	../tests/monte.c \
	../tests/probe-unexp.c \
	../tests/steve-mat-mul.c \
	../tests/t1.c \
	../tests/test01.c \
	../tests/test02.c \
	../tests/test03.c \
	../tests/test04.c \
	../tests/test1.c \
	../tests/test2.c \
	../tests/test3.c \
	../tests/test4.c \
	../tests/test5.c \
	../tests/test6.c \
	../tests/test7.c

CXX_TESTCODES_REQUIRED_TO_PASS = 

TESTCODE_CURRENTLY_FAILING = 


ROSE_FLAGS = --edg:no_warnings -w -rose:verbose 0 --edg:restrict #-rose:wave

PASSING_C_TEST_Objects = $(C_TESTCODES_REQUIRED_TO_PASS:.c=.o)

PASSING_CXX_TEST_Objects = $(CXX_TESTCODES_REQUIRED_TO_PASS:.cpp=.o)

$(PASSING_C_TEST_Objects): %.o: $(srcdir)/%.c mpiAnalysis
	./mpiAnalysis $(ROSE_FLAGS) $(TESTCODE_INCLUDES) -c $<

$(PASSING_CXX_TEST_Objects): %.o: $(srcdir)/%.cpp mpiAnalysis
	./mpiAnalysis ${ROSE_FLAGS} ${TESTCODE_INCLUDES} -c $<

ALL_TESTCODES = \
$(C_TESTCODES_REQUIRED_TO_PASS) \
$(CXX_TESTCODES_REQUIRED_TO_PASS) \
$(TESTCODE_CURRENTLY_FAILING)

check-local:
	@echo "Test for MPI Analysis ..."
	@$(MAKE) $(PASSING_C_TEST_Objects) 
	@$(MAKE) $(PASSING_CXX_TEST_Objects)
	@echo "***********************************************************************************************************"
	@echo "****** ROSE/projects/mpiAnalysis: make check rule complete (terminated normally) ******"
	@echo "***********************************************************************************************************"

EXTRA_DIST = $(ALL_TESTCODES) README 

clean-local:
	rm -f *.o rose_*.* *.out *.dot
	rm -rf QMTest
