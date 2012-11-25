#include<mpi.h>

//=============================================================================================
// Descripton:
// Simple MPI_Send() in condition indirect dependent on MPI_Recv(*)
//=============================================================================================
int main(int argc,char *argv[]) 
{
  int y, z;
  int np, rank;
  MPI_Status recvstat;

  MPI_Init(&argc, &argv);

  MPI_Comm_size(MPI_COMM_WORLD, &np);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  
  MPI_Recv(&y, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &recvstat); //MARK

  z = y;

  if(z == 1)
  {
    MPI_Send(&z, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD);
  }

  MPI_Finalize();

  return 0;
}
