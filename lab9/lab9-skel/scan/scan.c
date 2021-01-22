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

    for (int i = 1; i < procs; i *= 2) {
        if (rank + i < procs) {
            //trimite la procesul cu rank-ul [rank + pas]
            int receiver = rank + i;
            MPI_Send(&value, 1, MPI_INT, receiver, 0, MPI_COMM_WORLD);
        }
        if (rank - i >= 0) {
            //primește de la procesul cu rank-ul [rank - pas]
            int sender = rank - i;
            int recv_num;
            MPI_Recv(&recv_num, 1, MPI_INT, sender, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            value += recv_num;
        }
    }

    printf("Process [%d] has result = %d\n", rank, value);
    //ultimul proces din comunicator retine rezultatul final
    //ficare proces retine un rezultat partial

    MPI_Finalize();

}

