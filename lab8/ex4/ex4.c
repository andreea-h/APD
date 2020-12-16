#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ROOT 3

int main (int argc, char *argv[])
{
    int  numtasks, rank, len;
    char hostname[MPI_MAX_PROCESSOR_NAME];

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Get_processor_name(hostname, &len);

    int value;

    if (rank == ROOT) {

        // The ROOT process receives an element from any source.
        // Prints the element and the source. HINT: MPI_Status.
        int recv;
        MPI_Status status;
        int i;
        //afiseaza valorile primite de la celelalte procese
        for (i = 0; i < numtasks - 1; i++) {
            MPI_Recv(&recv, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
            printf("Procesul cu rank-ul %d a primit numarul %d de la procesul %d\n", rank, recv, status.MPI_SOURCE);
        }
        
    } else {

        // Generate a random number.
        srand(time(NULL));
        value = rand() % (rank * 50 + 1);

        printf("Process [%d] send %d.\n", rank, value);

        // Sends the value to the ROOT process.
        MPI_Send(&value, 1, MPI_INT, ROOT, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();

}

