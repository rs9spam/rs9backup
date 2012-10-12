include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

bin_PROGRAMS = mpiAnalysis
#mpiAnalysis_SOURCES = mpiAnalysis.cpp virtualMPICFG.h virtualMPICFG.cpp staticMPICFG.h staticMPICFG.cpp mpiCFG.h mpiCFG.cpp
mpiAnalysis_SOURCES = mpiAnalysis.cpp mpiCFG.cpp mpiCFG.h


INCLUDES = $(ROSE_INCLUDES)
LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

C_TESTCODES_REQUIRED_TO_PASS = \
	../tests/stest1.c

CXX_TESTCODES_REQUIRED_TO_PASS = 

TESTCODE_CURRENTLY_FAILING = 


ROSE_FLAGS = --edg:no_warnings -w -rose:verbose 0 --edg:restrict #-rose:wave

PASSING_C_TEST_Objects = $(C_TESTCODES_REQUIRED_TO_PASS:.c=.o)

PASSING_CXX_TEST_Objects = $(CXX_TESTCODES_REQUIRED_TO_PASS:.cpp=.o)

$(PASSING_C_TEST_Objects): %.o: $(srcdir)/%.c graphColoringMain
	./graphColoringMain $(ROSE_FLAGS) $(TESTCODE_INCLUDES) -c $<

$(PASSING_CXX_TEST_Objects): %.o: $(srcdir)/%.cpp graphColoringMain
	./graphColoringMain ${ROSE_FLAGS} ${TESTCODE_INCLUDES} -c $<

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
