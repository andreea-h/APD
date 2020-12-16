#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main (int argc, char *argv[])
{
    int  numtasks, rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks); 
    //numtasks-numarul de procese care ruleaza in cadrul comunicatorului

    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    //determina identificatorul precesului curent in cadrul comunicatorului
    int recv_num;


    srand(time(NULL));
    
    // First process starts the circle.
    if (rank == 0) {
    	MPI_Status status;
        // First process starts the circle.
        // Generate a random number.
        // Send the number to the next process.
    	int random_num = rand() % 100;
    	MPI_Send(&random_num, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
    	
    	//primeste o valoare de la procesul numtasks - 1
    	MPI_Recv(&recv_num, 1, MPI_INT, numtasks - 1, 0, MPI_COMM_WORLD, &status);
    	printf("Procesul cu rank-ul %d a primit numarul %d.\n", rank, recv_num);

    } else if (rank == numtasks - 1) {
        // Last process close the circle.
        // Receives the number from the previous process.
        // Increments the number.
        // Sends the number to the first process.
        MPI_Status status;
        MPI_Recv(&recv_num, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &status);
        printf("Procesul cu rank-ul %d a primit numarul %d.\n", rank, recv_num);
        recv_num = recv_num + 2;
        MPI_Send(&recv_num, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

    } else {
        // Middle process.
        // Receives the number from the previous process.
        // Increments the number.
        // Sends the number to the next process.
        MPI_Status status;
    	MPI_Recv(&recv_num, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &status);
    	printf("Procesul cu rank-ul %d a primit numarul %d.\n", rank, recv_num);
    	recv_num = recv_num + 2;
    	MPI_Send(&recv_num, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();

}

