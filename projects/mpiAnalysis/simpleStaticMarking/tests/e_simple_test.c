#include<mpi.h>

//=============================================================================================
//Descripton:
//While with continue Statement and following MPI function call what makes R(*) non det.
//=============================================================================================

int main(int argc,char *argv[]) 
{
  int y;
  int x = 5;
  int np, rank;
  MPI_Status recvstat;

  MPI_Init(&argc, &argv);

  MPI_Comm_size(MPI_COMM_WORLD, &np);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  
  while(x > 0)
  {
    MPI_Recv(&y, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &recvstat);  // MARK

    if(y == 1)
      continue;
    
    MPI_Send(&y, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD);
  }

  MPI_Finalize();

  return 0;
}
