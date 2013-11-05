#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#include $(top_srcdir)/projects/mpiAnalysis/common.deb.libs

#if ROSE_WITH_MPI_C
SUBDIRS = staticMatching simpleStaticMarking
#endif

check-local:
	@echo "*******************************************************************************"
	@echo "** ROSE/projects/mpiAnalysis: make check rule complete (terminated normally) **"
	@echo "*******************************************************************************"

clean-local:
	rm -rf *.o rose_*.[cC] *.dot *.out rose_*.cpp detail.html summary.html index.html dbg_imgs



