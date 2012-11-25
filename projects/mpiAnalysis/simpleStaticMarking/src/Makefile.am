include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SAF_SRC_ROOT   = $(top_srcdir)/src/midend/programAnalysis/genericDataflow/

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


bin_PROGRAMS = simpleMpiA simpleInterMpiA
simpleMpiA_SOURCES = simpleMpiA.cpp nonDetAnalysis.cpp nonDetAnalysis.h
simpleInterMpiA_SOURCES = simpleInterMpiA.cpp nonDetAnalysis.cpp nonDetAnalysis.h

#INCLUDES = $(ROSE_INCLUDES)
LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

C_TESTCODES_REQUIRED_TO_PASS = \
    ../tests/a_simple_test.c \
    ../tests/b_simple_test.c \
    ../tests/c_simple_test.c \
    ../tests/d_simple_test.c \
    ../tests/e_simple_test.c \
    ../tests/f_simple_test.c \
    ../tests/g_simple_test.c \
    ../tests/h_simple_test.c \
    ../tests/i_simple_test.c \
    ../tests/j_simple_test.c \
    ../tests/k_simple_test.c \
    ../tests/l_simple_test.c 


	
CXX_TESTCODES_REQUIRED_TO_PASS = 

TESTCODE_CURRENTLY_FAILING = 


ROSE_FLAGS = --edg:no_warnings -w -rose:verbose 0 --edg:restrict #-rose:wave

PASSING_C_TEST_Objects = $(C_TESTCODES_REQUIRED_TO_PASS:.c=.o)

PASSING_CXX_TEST_Objects = $(CXX_TESTCODES_REQUIRED_TO_PASS:.cpp=.o)

$(PASSING_C_TEST_Objects): %.o: $(srcdir)/%.c simpleMpiA
	./simpleMpiA $(ROSE_FLAGS) $(TESTCODE_INCLUDES) -c $<

$(PASSING_CXX_TEST_Objects): %.o: $(srcdir)/%.cpp simpleMpiA
	./simpleMpiA ${ROSE_FLAGS} ${TESTCODE_INCLUDES} -c $<

ALL_TESTCODES = \
$(C_TESTCODES_REQUIRED_TO_PASS) \
$(CXX_TESTCODES_REQUIRED_TO_PASS) \
$(TESTCODE_CURRENTLY_FAILING)

check-local:
	@echo "Test for MPI Analysis ..."
	@$(MAKE) $(PASSING_C_TEST_Objects) 
	@$(MAKE) $(PASSING_CXX_TEST_Objects)
	@echo "***********************************************************************************************************"
	@echo "****** ROSE/projects/simpleStaticMarking: make check rule complete (terminated normally) ******"
	@echo "***********************************************************************************************************"

EXTRA_DIST = $(ALL_TESTCODES) README 

clean-local:
	rm -f *.o rose_*.* *.out *.dot
	rm -rf QMTest
