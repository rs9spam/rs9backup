include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

if ROSE_WITH_MPI_C
SUBDIRS = staticMatching
endif

check-local:
	@echo "Test for MPI Analysis ..."
	@echo "***********************************************************************************************************"
	@echo "****** ROSE/projects/mpiAnalysis: make check rule complete (terminated normally) ******"
	@echo "***********************************************************************************************************"

clean-local:
	rm -f *.o rose_*.* *.out *.dot
	rm -rf QMTest



