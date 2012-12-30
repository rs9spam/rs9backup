/*The Parallel simple Program*/
#include <stdio.h>
#include <mpi.h>

main(int argc, char **argv)
{ 
  int rank, size, number;

  MPI_Init(&argc,&argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
 
  if (rank == 0) 
  {
    number = -1;
    MPI_Send(&number, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
  } 
  else if (rank == 1) 
  {
    MPI_Recv(&number, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    number += 2;
  }

  MPI_Finalize();
}
