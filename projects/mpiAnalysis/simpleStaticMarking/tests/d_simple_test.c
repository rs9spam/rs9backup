#include<mpi.h>

//=============================================================================================
//Descripton:
//escape wihle(1) wiht break stmt
//=============================================================================================

int main(int argc,char *argv[]) 
{
  int y;
  int np, rank;
  MPI_Status recvstat;

  MPI_Init(&argc, &argv);

  MPI_Comm_size(MPI_COMM_WORLD, &np);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  
  while(1)
  {
    MPI_Recv(&y, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &recvstat);  //MARK

    if(y == 1)
    {
      break;
    }
  }

  MPI_Finalize();

  return 0;
}
