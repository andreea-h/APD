#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ROOT 0

int main (int argc, char *argv[])
{
    int  numtasks, rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    // Checks the number of processes allowed.
    if (numtasks != 2) {
        printf("Wrong number of processes. Only 2 allowed!\n");
        MPI_Finalize();
        return 0;
    }

    // How many numbers will be sent.
    int send_numbers = 10;

    if (rank == 0) {

        // Generate the random numbers.
        // Generate the random tags.
        // Sends the numbers with the tags to the second process.
        int i, value, tag;
        srand(time(NULL));
        for (i = 0; i < send_numbers; i++) {
            //genereaza valoare
            value = rand() % 100 + 1; 
            //genereaza tag
            tag = rand() % 100 + 1;
            MPI_Send(&value, 1, MPI_INT, 1, tag, MPI_COMM_WORLD);
            printf("Procesul cu rank-ul %d a trimis valoarea %d, cu tag-ul %d\n", rank, value, tag);

        }


    } else {

        // Receives the information from the first process.
        // Prints the numbers with their corresponding tags.
        int i;
        for (i = 0; i < send_numbers; i++) {
            int recv;
            MPI_Status status;
            MPI_Recv(&recv, 1, MPI_INT, ROOT, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            printf("Procesul cu rank-ul %d a primit valoarea %d, cu tag-ul %d\n", rank, recv, status.MPI_TAG);
        }
    }

    MPI_Finalize();

}

