#include<mpi.h>

//=============================================================================================
//nothing is dependent on the MPI_RECV(MPI_ANY_SOURCE) 
void detSwitch( int x, int result, int np)
{
  MPI_Status status;
  while( result < np )
  {
    MPI_Recv(&x, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);//do not Mark

    switch(np)
    {
    case 1: MPI_Send(&result, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD); break;
    default: MPI_Send(&x, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD); break;
    }
    result++;
  }
}

//=============================================================================================
void nonDetSwitch(int x, int result)
{
  MPI_Status status;
  MPI_Recv(&x, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status); //Mark 27:
  switch(x)
  {
    case 1: MPI_Send(&result, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD); break;
    default: MPI_Send(&x, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD); break;
  }
}

//=============================================================================================
void nonDetProbe()
{
  MPI_Status probe_status;
  MPI_Status status;
  int array[2];

  while(1)
  {
    MPI_Probe( MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &probe_status );
    MPI_Recv(array, 2, MPI_INT, probe_status.MPI_SOURCE, MPI_ANY_SOURCE, MPI_COMM_WORLD, &status); //Mark: 45
    if(array[0] == 0)
      break;
  }
}

//=============================================================================================
//Descripton:
//No source of non determinism because of breaks inside of switch case.
//=============================================================================================
int main(int argc,char *argv[]) 
{
  int np, rank;
  int x = 10;
  int result = 0;
  
  MPI_Init(&argc, &argv);

  MPI_Comm_size(MPI_COMM_WORLD, &np);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  
  detSwitch(x, result, np);
  nonDetSwitch(x, result);
  nonDetProbe();

  MPI_Finalize();

  return 0;
}
