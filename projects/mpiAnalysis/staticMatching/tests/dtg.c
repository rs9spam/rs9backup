// multiple DTG example

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <string.h>

#define buf_size 128

int
main (int argc, char **argv)
{
  int nprocs = -1;
  int rank = -1;
  char processor_name[128];
  int namelen = 128;
  int buf0[buf_size];
  int buf1[buf_size];
  MPI_Status status, status2;
  MPI_Request req1, req2;

  /* init */
  MPI_Init (&argc, &argv);
  MPI_Comm_size (MPI_COMM_WORLD, &nprocs);
  MPI_Comm_rank (MPI_COMM_WORLD, &rank);
  MPI_Get_processor_name (processor_name, &namelen);
  printf ("(%d) is alive on %s\n", rank, processor_name);
  fflush (stdout);

  //  MPI_Barrier (MPI_COMM_WORLD);


   if (rank == 0)
    {

      MPI_Irecv (buf0, buf_size, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &req1);

      MPI_Wait(&req1, &status);
      
      MPI_Irecv (buf0, buf_size, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &req2);
      
      MPI_Wait(&req2, &status);

      
    }
  else if (rank == 1)
    {
      memset (buf0, 0, buf_size);

    //  sleep (30);

      MPI_Isend (buf0, buf_size, MPI_INT, 0, 0, MPI_COMM_WORLD, &req1);

      MPI_Wait(&req1, &status);

    }
  else if (rank == 2)
    {
      memset (buf0, 0, buf_size);

     // sleep (60);
    
      MPI_Irecv (buf0, buf_size, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &req1);
      
      MPI_Wait(&req1, &status);


      MPI_Isend (buf0, buf_size, MPI_INT, 0, 0, MPI_COMM_WORLD, &req2);
      MPI_Wait(&req2, &status);
    }
  else if(rank ==3)
    {
      memset (buf0, 0, buf_size);
      MPI_Isend (buf0, buf_size, MPI_INT, 2, 0, MPI_COMM_WORLD, &req1);
      
      MPI_Wait(&req1, &status);

    }

  MPI_Finalize ();
  printf ("(%d) Finished normally\n", rank);
}
