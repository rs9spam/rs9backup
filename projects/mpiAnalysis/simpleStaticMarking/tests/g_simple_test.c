#include<mpi.h>

//=============================================================================================
//Descripton:
//=============================================================================================

//=============================================================================================
//
void simpleIf()
{
  int a,b;
  MPI_Status recvstat;

  MPI_Recv(&a, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &recvstat);  //MARK 14
  MPI_Recv(&b, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &recvstat);  //DO NOT MARK
  if(a == 0)
    MPI_Send(&b, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD);
}

//=============================================================================================

void simpleIndirectIf()  //should mark both
{
  int a,b;
  int c=1;
  MPI_Status recvstat;

  MPI_Recv(&a, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &recvstat);  //MARK  28
  MPI_Recv(&b, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &recvstat);  //MARK  //!!!!!!!!!!
 
  if(b == 0)
    a=c;

  if(a == 0)
    MPI_Send(&b, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD);
}

//=============================================================================================

void simpleIndirectIf1()  
{
  int a,b;
  int c = 1;
  MPI_Status recvstat;

  MPI_Recv(&a, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &recvstat);  //DO NOT MARK
  MPI_Recv(&b, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &recvstat);  //DO NOT MARK
  a = c;
  if(a == 0)
    MPI_Send(&b, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD);
}

//=============================================================================================
// indirect dependent in if
void simpleIndirectIf2()  
{
  int a, b;
  int c = 1;
  MPI_Status recvstat;

  MPI_Recv(&a, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &recvstat);  //MARK 61
  c = a;
  if(c == 0)
    MPI_Send(&b, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD);
}

//=============================================================================================

void simpleRecvSource()  
{
  int a, b;
  MPI_Status recvstat;

  MPI_Recv(&a, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &recvstat);  //MARK 74
  if(recvstat.MPI_SOURCE == 0)
    MPI_Send(&b, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD);
}

//=============================================================================================

void simpleRecvTag()  
{
  int a, b;
  MPI_Status recvstat;

  MPI_Recv(&a, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &recvstat);  //MARK 86
  if(recvstat.MPI_TAG == 0)
    MPI_Send(&b, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD);
}

//=============================================================================================
//=============================================================================================
int main(int argc,char *argv[]) 
{
  int c, d;
  int np, rank;
  MPI_Status recvstat;

  MPI_Init(&argc, &argv);

  MPI_Comm_size(MPI_COMM_WORLD, &np);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  simpleIf();
  simpleIndirectIf();
  simpleIndirectIf1();
  simpleIndirectIf2();
  simpleRecvSource();
  simpleRecvTag();

 
  while(1)
  {
    MPI_Recv(&c, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &recvstat);  //MARK 114

    if(c < 4)
    {
      break;
    }
    
    MPI_Send(&c, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD);
  }
  while(1)
  { 
    MPI_Send(&d, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD);
    if(d == 0)
      break;
    MPI_Recv(&d, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &recvstat);  //MARK 128
  }


  MPI_Finalize();

  return 0;
}
