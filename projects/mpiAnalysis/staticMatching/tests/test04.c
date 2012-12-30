#include<mpi.h>

int main(int argc, char* argv[])
{
  int x;
  int np, rank;
  MPI_Status status;

  MPI_Comm_size(MPI_COMM_WORLD, &np);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if(rank == 0)
  {
    for( x = 1; x<10; x++ )
    {
      MPI_Send( &x, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }
  }
  else if( rank < 10 )
  {
    MPI_Recv( &x, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
  }

  MPI_Finalize();

  return 0;
}

