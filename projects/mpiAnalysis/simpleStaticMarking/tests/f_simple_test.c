#include<mpi.h>

//=============================================================================================
//Descripton:
//No source of non determinism because of breaks inside of switch case.
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

    if(y < 4)
    {
      MPI_Send(&y, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD);
    }
    
    MPI_Send(&y, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD);
  }

  MPI_Finalize();

  return 0;
}
