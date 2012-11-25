#include<mpi.h>

//=============================================================================================
void sendTo()
{
  int a = 5;
  MPI_Send(&a, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD);
}

//=============================================================================================
void decideSendTo(int x)
{
  if(x == 0)
    sendTo();
}

//=============================================================================================
//Descripton:
//No source of non determinism because of breaks inside of switch case.
//=============================================================================================
int main(int argc,char *argv[]) 
{
  int np, rank;
  int x = 10;
  MPI_Status status;
  
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &np);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  MPI_Recv(&x, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status); //Mark 31:
  
  decideSendTo(x);

  MPI_Finalize();

  return 0;
}
