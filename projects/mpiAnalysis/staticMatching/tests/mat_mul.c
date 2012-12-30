#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <assert.h>
//#include <isp.h>

#define MAX_AROWS 8//1024
#define MAX_ACOLS 8//2048
#define MAX_BROWS MAX_ACOLS
#define MAX_BCOLS 8//1024
#define MAX_CROWS MAX_AROWS
#define MAX_CCOLS MAX_BCOLS
#define MAX_A_ENTRIES (MAX_AROWS * MAX_ACOLS)
#define MAX_B_ENTRIES (MAX_BROWS * MAX_BCOLS)
#define MAX_C_ENTRIES (MAX_AROWS * MAX_BCOLS)

int main(int argc,char *argv[]) {
  float *a, *b, *c, **d, **e, **f;
  float *buffer, *ans;
  int myid, master, numprocs, ierr;
  int i, j, k, numsent, sender;
  int anstype, row, arows, acols, brows, bcols, crows, ccols;
  MPI_Status status;
  char namefile[10];
	
  master = 0;
  arows  = MAX_AROWS;
  acols  = MAX_ACOLS;
  brows  = MAX_BROWS;
  bcols  = MAX_BCOLS;
  crows  = arows;
  ccols  = bcols;	

  {
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);
    
    a = (float *) malloc(MAX_A_ENTRIES * sizeof(float));
    b = (float *) malloc(MAX_B_ENTRIES * sizeof(float));
    c = (float *) malloc(MAX_C_ENTRIES * sizeof(float));
    buffer = (float *) malloc(MAX_ACOLS * sizeof(float));
    ans = (float *) malloc(MAX_ACOLS * sizeof(float));
    
    if (myid == master) {
      
      /* read a, b */
      
      for (i = 0; i < MAX_AROWS; i++) 
	for (j = 0; j < MAX_ACOLS; j++) 
	  a[i*MAX_ACOLS + j] = (float) (i*10 + j);
      
      for (i = 0; i < MAX_BROWS; i++) 
	for (j = 0; j < MAX_BCOLS; j++) 
	  b[i*MAX_BCOLS + j] = (float) (i*10 + j);
      
      /* finished reading */
      numsent = 0;
      
      MPI_Bcast(b, brows*bcols, MPI_FLOAT, master, MPI_COMM_WORLD);
      for (i = 0; i < numprocs-1; i++) {
	for (j = 0; j < acols; j++) {
	  buffer[j] = a[i*acols+j];
	}
	
	MPI_Send(buffer, acols, MPI_FLOAT, i+1, i+1, MPI_COMM_WORLD);
	numsent++;
      }
      for (i = 0; i < crows; i++) {
	MPI_Recv(ans, ccols, MPI_FLOAT, MPI_ANY_SOURCE, MPI_ANY_TAG, 
		 MPI_COMM_WORLD, &status);
	
	/* Writes results */
	sender = status.MPI_SOURCE;
	
	anstype = status.MPI_TAG - 1;
	for (j = 0; j < ccols; j++) {
	  c[anstype*ccols+j] = ans[j];
	}
	if (numsent < arows) {
	  for (j = 0; j < acols; j++) {
	    buffer[j] = a[numsent*acols+j];
	  }
	  MPI_Send(buffer, acols, MPI_FLOAT, sender, numsent+1, MPI_COMM_WORLD);
	  numsent++;
	}
	else {
	  MPI_Send(buffer, 1, MPI_FLOAT, sender, 0, MPI_COMM_WORLD);
	}      
      }

    }
    else {
      MPI_Bcast(b, brows*bcols, MPI_FLOAT, master, MPI_COMM_WORLD);
      while (1) {
	MPI_Recv(buffer, acols, MPI_FLOAT, master, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
	if (status.MPI_TAG == 0) break;
	row = status.MPI_TAG - 1;
	for (i = 0; i < bcols; i++) {
	  ans[i] = 0.0;
	  for (j = 0; j < acols; j++) {
	    ans[i] += buffer[j]*b[j*bcols+i];
	  }
	}
	MPI_Send(ans, bcols, MPI_FLOAT, master, row+1, MPI_COMM_WORLD);
      }
    }
    MPI_Finalize();
  }
}
