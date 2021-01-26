#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//shear-sort pe o topologie de tip matrice cu N * N procese
//fiecare proces are vecini in stanga sreapta sus si jos
//se sorteaza N*N numere
//fiecare proces are initial un singur numar


static int *v;
static int N; //N este patrat perfect

void init()
{
    int i;
    v = malloc(sizeof(int) * N);
   
    if (v == NULL) {
        printf("Eroare la malloc!");
        exit(1);
    }

    srand(1000);

    for (i = 0; i < N; i++) {
        v[i] = rand() % N;
    }
}


int main (int argc, char *argv[])
{
    int  numtasks, rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks); 
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    int process_value; //valoarea din vector atribuita procesului curent

    //procesul de rank 0 genereaza vectorul si trimite catre celelalte procese valoarea aferenta lor din vector
    if (rank == 0) {
        N = numtasks;
        init();
        int i;
        for (i = 1; i < numtasks; i++) {
            MPI_Send(&v[i], 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
        process_value = v[0];

        printf("Vectorul generat: ");
        for (i = 0; i < N; i++) {
            printf("%d ", v[i]);
        }
        printf("\n");
    }
    else {
        MPI_Recv(&process_value, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);



        MPI_Send(&process_value, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Barrier(MPI_COMM_WORLD);



    //la final procesul cu rank-ul 0 primeste valorile care sunt retinute de fiecare proces
    //si le memoreaza in vector
    if (rank == 0) {
        v[0] = process_value;
        int i;
        for (i = 1; i < numtasks; i++) {
            MPI_Recv(&v[i], 1, MPI_INT, i, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }


        printf("Vectorul ordonat: ");
        for (i = 0; i < N; i++) {
            printf("%d ", v[i]);
        }
        printf("\n");
    }


    MPI_Finalize();

}

