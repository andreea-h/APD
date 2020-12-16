#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

#define GROUP_SIZE 4

int main (int argc, char *argv[])
{
    int old_size, new_size;
    int old_rank, new_rank;
    int recv_rank;
    MPI_Comm custom_group;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &old_size); // Total number of processes.
    MPI_Comm_rank(MPI_COMM_WORLD, &old_rank); // The current process ID / Rank.

    // Split the MPI_COMM_WORLD in small groups.
    int color = old_rank / GROUP_SIZE;
    int key = old_rank % GROUP_SIZE;
    MPI_Comm_split(MPI_COMM_WORLD, color, key, &custom_group);
    //params: communicator, color, key, newcomm
    //procesele care au acelasi color sunr in acelasi nou comunicator
    
    MPI_Comm_size(custom_group, &new_size);
    MPI_Comm_rank(custom_group, &new_rank);


    printf("Rank [%d] / size [%d] in MPI_COMM_WORLD and rank [%d] / size [%d] in custom group.\n",
            old_rank, old_size, new_rank, new_size);


	int send;
	//fiecare proces comunica doar in grupul lui
	// Send the rank to the next process.
	if (new_rank == 0) { //initiatorul
		MPI_Status status;
		send = new_rank;
		MPI_Send(&send, 1, MPI_INT, new_rank + 1, 0, custom_group);
		// Receive the rank.
		MPI_Recv(&recv_rank, 1, MPI_INT, GROUP_SIZE - 1, 0, custom_group, &status);
		printf("Process [%d] from group [%d] received [%d].\n", new_rank,
           old_rank / GROUP_SIZE, recv_rank);
	}
	else if (new_rank == GROUP_SIZE - 1) { //ultimul din grup
		MPI_Status status;
		MPI_Recv(&recv_rank, 1, MPI_INT, GROUP_SIZE - 2, 0, custom_group, &status);
		printf("Process [%d] from group [%d] received [%d].\n", new_rank,
           old_rank / GROUP_SIZE, recv_rank);
        send = new_rank;
        MPI_Send(&send, 1, MPI_INT, 0, 0, custom_group);
	}
	else {//middle element
		MPI_Status status;
		MPI_Recv(&recv_rank, 1, MPI_INT, new_rank - 1, 0, custom_group, &status);
		printf("Process [%d] from group [%d] received [%d].\n", new_rank,
           old_rank / GROUP_SIZE, recv_rank);
        send = new_rank;
        
        MPI_Send(&send, 1, MPI_INT, new_rank + 1, 0, custom_group);
	}	



    MPI_Finalize();
}

