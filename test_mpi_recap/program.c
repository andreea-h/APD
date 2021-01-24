#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
 
#define  MASTER 0
 
int main (int argc, char *argv[]) {
    int numtasks, rank, len;
    char hostname[MPI_MAX_PROCESSOR_NAME];
 
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Get_processor_name(hostname, &len);

    int size = 100;
    int arr[10];

    //procesul master primite un vector catre celelalte procese
    if (rank == MASTER) {
        int *array = (int *)malloc(size * sizeof(int));
        int i;
        for (i = 0; i < size; i++) {
            array[i] = i;
        }
        printf("Procesul cu rank-ul %d a generat : ", rank);
        for (i = 0; i < size; i++) {
            printf("%d ", array[i]);
        }
        printf("\n");
        //trimite array-ul catre celelalte procese
        for (i = 1; i < numtasks; i++) {
            MPI_Send(array, size, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
        //array trimis si &
        srand(42);
        for (i = 0; i < 50; i++) {
            arr[i] = rand() % 100;
        }

        printf("Procesul cu rank-ul %d a generat : ", rank);
        for (i = 0; i < 10; i++) {
            printf("%d ", arr[i]);
        }
        printf("\n");
        //trimite array-ul catre celelalte procese
        for (i = 1; i < numtasks; i++) {
            MPI_Send(arr, 10, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
    }
    else {
        //primeste array-ul de la master
        int *array = (int *)malloc(size * sizeof(int));
        MPI_Recv(array, size, MPI_INT, MASTER, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        printf("Procesul cu rank-ul %d are urmatorul array: ", rank);
        int i;
        for (i = 0; i < size; i++) {
            printf("%d ", array[i]);
        }
        printf("\n");

        MPI_Recv(arr, 50, MPI_INT, MASTER, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        printf("Procesul cu rank-ul %d are al doilea array: ", rank);
        for (i = 0; i < 10; i++) {
            printf("%d ", arr[i]);
        }
        printf("\n");
    }
        
    MPI_Finalize();
    
}