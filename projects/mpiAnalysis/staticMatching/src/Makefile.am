include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
include $(top_srcdir)/projects/mpiAnalysis/common.deb.libs

#SUBDIRS = \
#	constPropAnalysis \
#	loopAnalysis \
#	mpiCFG \
#	mpiUtils \
#	rankAnalysis

bin_PROGRAMS = mpiAnalysis rankAnalysisTest loopAnalysisTest rankMpiCallListTest

mpiAnalysis_SOURCES = \
	mpiAnalysis.cpp \
	mpiCFG/mpiCFG.cpp mpiCFG/mpiCFG.h \
	constPropAnalysis/constantPropagationAnalysis.cpp constPropAnalysis/constantPropagationAnalysis.h \
	rankAnalysis/rankAnalysis.cpp rankAnalysis/rankAnalysis.h \
	rankAnalysis/pSet.cpp rankAnalysis/pSet.h \
	rankAnalysis/RankLattice.cpp rankAnalysis/RankLattice.h \
	mpiUtils/mpiUtils.cpp mpiUtils/mpiUtils.h

rankAnalysisTest_SOURCES = \
	rankAnalysisTest.cpp \
	rankAnalysis/rankAnalysis.cpp rankAnalysis/rankAnalysis.h \
	rankAnalysis/pSet.cpp rankAnalysis/pSet.h \
	rankAnalysis/RankLattice.cpp rankAnalysis/RankLattice.h \
	rankAnalysis/FinestPSetGranularity.cpp rankAnalysis/FinestPSetGranularity.h \
	mpiUtils/mpiUtils.cpp mpiUtils/mpiUtils.h
	
loopAnalysisTest_SOURCES = \
	loopAnalysisTest.cpp \
	mpiCFG/mpiCFG.cpp mpiCFG/mpiCFG.h \
	constPropAnalysis/constantPropagationAnalysis.cpp constPropAnalysis/constantPropagationAnalysis.h \
	loopAnalysis/loopAnalysis.cpp loopAnalysis/loopAnalysis.h \
	loopAnalysis/LoopLattice.cpp loopAnalysis/LoopLattice.h \
	rankAnalysis/pSet.cpp rankAnalysis/pSet.h \
    rankAnalysis/RankLattice.cpp rankAnalysis/RankLattice.h \
    rankAnalysis/rankAnalysis.cpp rankAnalysis/rankAnalysis.h \
	mpiUtils/mpiUtils.cpp mpiUtils/mpiUtils.h	

rankMpiCallListTest_SOURCES = \
	rankMpiCallListTest.cpp \
	rankAnalysis/pSet.cpp rankAnalysis/pSet.h \
	rankAnalysis/RankLattice.cpp rankAnalysis/RankLattice.h \
	rankAnalysis/rankAnalysis.cpp rankAnalysis/rankAnalysis.h \
	loopAnalysis/LoopLattice.cpp loopAnalysis/LoopLattice.h \
	loopAnalysis/loopAnalysis.cpp loopAnalysis/loopAnalysis.h \
	rankAnalysis/FinestPSetGranularity.cpp rankAnalysis/FinestPSetGranularity.h \
	rankMpiCallList/CallAnalysis.cpp rankMpiCallList/CallAnalysis.h \
	rankMpiCallList/CallLattice.cpp rankMpiCallList/CallLattice.h \
	mpiUtils/mpiUtils.cpp mpiUtils/mpiUtils.h
	
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


#ROSE_FLAGS = --edg:no_warnings -w -rose:verbose 0 --edg:restrict #-rose:wave

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
