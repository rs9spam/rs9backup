#include <mpi.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    int rank, size;
    MPI_Init(&argc, &argv);
    int sbuff, rbuff, i, j;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    MPI_Status status;


    printf("rank : %d\n", rank);

    sbuff = 100;

    if(!rank) {
        /*for(i = 1; i < size; ++i) {
            MPI_Recv(&rbuff, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        }*/
         MPI_Recv(&rbuff, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status);
         MPI_Recv(&rbuff, 1, MPI_INT, MPI_ANY_SOURCE,1, MPI_COMM_WORLD, &status);
         MPI_Recv(&rbuff, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status);
         MPI_Recv(&rbuff, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status);

    }
    else {
        MPI_Send(&sbuff, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
        MPI_Send(&sbuff, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);

    }
    MPI_Finalize();


    if(!rank) 
        printf("rbuff : %d\n", rbuff);



    return 0;
}
