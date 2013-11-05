include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
include $(top_srcdir)/projects/mpiAnalysis/common.deb.libs
	
	
	
#######################################################
####                 Bin Programs                  ####                          
#######################################################
bin_PROGRAMS = simpleMpiA simpleInterMpiA
simpleMpiA_SOURCES = simpleMpiA.cpp nonDetAnalysis.cpp nonDetAnalysis.h
simpleInterMpiA_SOURCES = simpleInterMpiA.cpp nonDetAnalysis.cpp nonDetAnalysis.h

#INCLUDES = $(ROSE_INCLUDES)
LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

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


	
#CXX_TESTCODES_REQUIRED_TO_PASS = 

#TESTCODE_CURRENTLY_FAILING = 

#ROSE_FLAGS = --edg:no_warnings -w -rose:verbose 0 --edg:restrict #-rose:wave

#PASSING_C_TEST_Objects = $(C_TESTCODES_REQUIRED_TO_PASS:.c=.o)

#PASSING_CXX_TEST_Objects = $(CXX_TESTCODES_REQUIRED_TO_PASS:.cpp=.o)

#$(PASSING_C_TEST_Objects): %.o: $(srcdir)/%.c simpleMpiA
#	./simpleMpiA $(ROSE_FLAGS) $(TESTCODE_INCLUDES) -c $<

#$(PASSING_CXX_TEST_Objects): %.o: $(srcdir)/%.cpp simpleMpiA
#	./simpleMpiA ${ROSE_FLAGS} ${TESTCODE_INCLUDES} -c $<

#ALL_TESTCODES = \
#$(C_TESTCODES_REQUIRED_TO_PASS) \
#$(CXX_TESTCODES_REQUIRED_TO_PASS) \
#$(TESTCODE_CURRENTLY_FAILING)

check-local:
	@echo "Test for MPI Analysis ..."
	@$(MAKE) $(PASSING_C_TEST_Objects) 
	@$(MAKE) $(PASSING_CXX_TEST_Objects)
	@echo "*****************************************************************************************"
	@echo "*** ROSE/projects/simpleStaticMarking: make check rule complete (terminated normally) ***"
	@echo "*****************************************************************************************"

EXTRA_DIST = $(ALL_TESTCODES) README 

clean-local:
	rm -rf *.o rose_*.[cC] *.dot *.out rose_*.cpp detail.html summary.html index.html dbg_imgs
	rm -rf *main.nondet debug