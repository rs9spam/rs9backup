/*The Parallel simple Program*/
#include <stdio.h>
#include "mpi.h"

int inc(int x)
{
  x = x+1;
  if( x < -5 )
  {
    x = inc(x);
  }
  return x;
}

main(int argc, char **argv)
{ 
  int rank, size, number;

  MPI_Init(&argc,&argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
 
  if (rank == 0) 
  {
    number = -2;
    number = inc(number);
    MPI_Send(&number, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
  } 
  else if (rank == 1) 
  {
    MPI_Recv(&number, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    number += 2;
  }

  MPI_Finalize();
}
