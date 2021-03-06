#include <pthread.h>
#include "mpi.h"
#include <stdio.h>


int Myrank, Numprocs;

void* Work(int Myid)
{
    printf(" Hello World! From Thread:%d On Process: %d. \n", Myid, Myrank);
    return NULL;
}

int main(int Argc, char *Argv[])
{
	pthread_t  Thread1, Thread2;

	MPI_Status  Status;

	/* Initialize Mpi Environment. */

	MPI_Init(&Argc, &Argv);
	MPI_Comm_size(MPI_COMM_WORLD, &Numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &Myrank);


	//if (Myrank == 0) {
		pthread_create(&Thread1, NULL, (void *(*) (void *)) Work, (void *) 5);

		pthread_create(&Thread2, NULL, (void *(*) (void *)) Work, (void *) 6);

		pthread_join(Thread1, NULL);
		pthread_join(Thread2, NULL);
//	}
	
	MPI_Finalize();

	return 0;

}