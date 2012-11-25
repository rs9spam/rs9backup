#include<mpi.h>

#define WORK_TAG 1
#define TERMINATE_TAG 2
#define RESULT_TAG 3

//=============================================================================================
int worker(int a, int f) {
  

  

  return 0;
}

//=============================================================================================
//Descripton:
//No source of non determinism because of breaks inside of switch case.
//=============================================================================================
int main(int argc,char *argv[]) 
{
  int np, rank;
  int a;
//  int x;
  int left, right, result, subintervalLength;
  int task, taskCount = 0;
  MPI_Status status;
  
  MPI_Init(&argc, &argv);

  MPI_Comm_size(MPI_COMM_WORLD, &np);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  
  while (1) {
    MPI_Recv(&task, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

 //   x =  status.MPI_TAG;

    if (status.MPI_TAG == WORK_TAG) {
      taskCount++;
      left = a+task*subintervalLength;
      right = left+subintervalLength;
      result = left+right;
      MPI_Send(&result, 1, MPI_INT, 0, RESULT_TAG, MPI_COMM_WORLD);
    } else {
      break;
    }
  }

  MPI_Finalize();

  return 0;
}
