#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <mpi.h>

#define write_frame write_frame_2

int np, rank;

void write_frame_2(){
  int i, j, k, m, n, from;
  int buf[10];

  if (rank != 0) {
    for (j = 1; j <=2; j++) {
      MPI_Send(buf, 10, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }
  } else {
    MPI_Status status;

    int numrecvs = 2*(np - 1);

    for (k = 0; k < numrecvs; k++) {
      int procx, procy, row;
      
      MPI_Recv(buf, 10, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG,
               MPI_COMM_WORLD, &status);
    }
  }
}

void  nd_exchange_ghost_cells() {
  int sbuf[10];
  MPI_Request req[1]; 
  MPI_Status sendstat[1], recvstat;
  if( rank != 0){ 
      MPI_Isend(sbuf, 10, MPI_INT, 0, 0, MPI_COMM_WORLD, &req[0]);
      MPI_Wait(req, sendstat);
  }
  else{
    
    int numrecvs = (np - 1);
    int k;
    for (k = 1; k < np; k++) {
      int procx, procy, row;
      MPI_Recv(sbuf, 10, MPI_INT, k, MPI_ANY_TAG, MPI_COMM_WORLD, &recvstat);
    }
  }
}


int main(int argc,char *argv[]) {

  MPI_Init(&argc, &argv);

  MPI_Comm_size(MPI_COMM_WORLD, &np);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  MPI_Barrier(MPI_COMM_WORLD);
  write_frame();
  MPI_Barrier(MPI_COMM_WORLD);
  nd_exchange_ghost_cells();
  MPI_Barrier(MPI_COMM_WORLD);

  MPI_Finalize();
}
