#include<mpi.h>

//=============================================================================================
// Descripton:
// ...
//=============================================================================================
int main(int argc,char *argv[]) 
{
  int i ,x, z;
  int np, rank;
  MPI_Status recvstat;

  MPI_Init(&argc, &argv);

  MPI_Comm_size(MPI_COMM_WORLD, &np);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  
  if(rank < 7)
  {
    i = rank;
    MPI_Send(&i, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD);
    MPI_Recv(&z, 1, MPI_INT, -1, 1, MPI_COMM_WORLD, &recvstat);
  }
  else
  {
    MPI_Recv(&i, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &recvstat);
    MPI_Recv(&x, 1, MPI_INT, -1, 1, MPI_COMM_WORLD, &recvstat);
  }

  np = i;
  
  if(rank < z || x < np)
  {
    i = rank;
 //   MPI_Recv(&y, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &recvstat);
    MPI_Send(&x, 1, MPI_INT, recvstat.MPI_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD);
  }

  MPI_Send(&x, 1, MPI_INT, recvstat.MPI_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD);

  MPI_Finalize();

  return 0;
}
