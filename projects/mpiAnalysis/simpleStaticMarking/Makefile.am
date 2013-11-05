include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
include $(top_srcdir)/projects/mpiAnalysis/common.deb.libs

#if ROSE_WITH_MPI_C
SUBDIR = src tests
#endif

CODE_DIR = src

.PHONY: all

all:
	$(MAKE) -C $(CODE_DIR)

clean-local:
	$(MAKE) -C $(CODE_DIR) clean-local
