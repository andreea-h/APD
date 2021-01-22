#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

#define MASTER 0

int main (int argc, char *argv[])
{
    int procs, rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int value = rank;

    for (int i = 2; i <= procs; i *= 2) {
        if (rank % i == 0) {
            //primește la procesul cu rank-ul [rank + (pas / 2)]
            int recv_num;
            int sender = rank + i / 2;
            MPI_Recv(&recv_num, 1, MPI_INT, sender, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            value += recv_num;
        }
        else if (rank % (i / 2) == 0) {
            //trimite la procesul cu rank-ul [rank - (pas / 2)]
            int receiver = rank - i / 2;
            MPI_Send(&value, 1, MPI_INT, receiver, 0, MPI_COMM_WORLD);
        }

    }

    if (rank == MASTER) { //rezultatul este memorat in primul proces din comunicator
        printf("Result = %d\n", value);
    }

    MPI_Finalize();

}

