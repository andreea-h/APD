#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//SORTAREA cu ODD-EVEN TRANSPOSITION 
//fiecare proces are o singura valorea!!!!!
//vectorul are N valori => vor exista N procese
//fiecare proces are un vecin in stanga si un vecin in dreapta
//mai putin procesele 0 si N - 1


static int *v; //vectorul care va fi sortat este generat random in functia init()
static int N;
static int *vQSort; 

void compare_vectors(int *a, int *b) {
    int i;

    for (i = 0; i < N; i++) {
        if (a[i] != b[i]) {
            printf("Sortare incorecta\n");
            return;
        }
    }

    printf("Sortare corecta\n");
}

int cmp(const void *a, const void *b) {
    int A = *(int*)a;
    int B = *(int*)b;
    return A - B;
}


void init()
{
    int i;
    v = malloc(sizeof(int) * N);
    vQSort = malloc(sizeof(int) * N); //retine vectorul sortat pentru a putea face verificarea 

    if (v == NULL || vQSort == NULL) {
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
    int  numtasks, rank, i;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    N = numtasks;
    int process_value; //valorea din vector detinuta de procesul curent 
   

    if (rank == 0) { //genereaza vectorul si trimite catre fiecare proces valorea care ii este asignata acestuia
        init();
        for (i = 1; i < numtasks; i++) {
            MPI_Send(&v[i], 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        }   
        printf("vectorul generat: ");
        for (i = 0; i < N; i++) {
            printf("%d ", v[i]);
        }
        printf("\n");
        process_value = v[0];
    }
    else {
        //primeste vectorul de la celelalte procese
      
        MPI_Recv(&process_value, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    

    //printf("Valorea retinuta de procesul %d este %d\n", rank, process_value);

    MPI_Barrier(MPI_COMM_WORLD);

    for (i = 0; i < N; i++) {
        //fiecare proces par trimite valoarea sa catre procesul din dreapta
        //si primeste de la procesul din dreapta valoarea acestuia
        //la nivelul fiecarui proces se actualizeaza valoarea daca este cazul
    
        //faza para (procesele pare initiaza comunicarea cu procesele impare de la dreapta lor)
        if (rank % 2 == 0 && rank != numtasks - 1) {

            int recv_val;
            MPI_Send(&process_value, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
            MPI_Recv(&recv_val, 1, MPI_INT, rank + 1, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
           // printf("valoarea primita de procesul cu rankul %d este %d\n", rank, recv_val);
            if (recv_val < process_value) {
                process_value = recv_val;
            }
        }
        else if (rank % 2 == 1) {
            int recv_val;
            MPI_Recv(&recv_val, 1, MPI_INT, rank - 1, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            //trimite valoarea proprie catre procesul din stanga
            MPI_Send(&process_value, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD);

            if (recv_val > process_value) {
                process_value = recv_val;
            }
        }

        MPI_Barrier(MPI_COMM_WORLD);
       
        //faza impara (procesele impare initiaza comunicarea cu procesele pare de la dreapta lor)
        if (rank % 2 == 1 && rank != numtasks - 1) {
            int recv_val;
            MPI_Send(&process_value, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
            MPI_Recv(&recv_val, 1, MPI_INT, rank + 1, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            if (recv_val < process_value) {
                process_value = recv_val;
            }
        }
        else if (rank % 2 == 0 && rank != 0) {
            int recv_val;
            MPI_Recv(&recv_val, 1, MPI_INT, rank - 1, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            //trimite valoarea proprie catre procesul din stanga
            MPI_Send(&process_value, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD);

            if (recv_val > process_value) {
                process_value = recv_val;
            }
        }


        MPI_Barrier(MPI_COMM_WORLD);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    if (rank == 0) {
        //primeste valorile de pe toate procesele si le completeaza in vector
        int i;
        for (i = 1; i < numtasks; i++) {
            int recv_num;
            MPI_Recv(&recv_num, 1, MPI_INT, i, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            v[i] = recv_num;
        }

        v[0] = process_value;
        printf("vectorul sortat: ");
        for(i = 0; i < N; i++) {
            printf("%d ", v[i]);
        }
        printf("\n");
    }
    else {
        //fiecare proces va trimite catre cel cu rank-ul 0 valoarea pe care acesta o are
        MPI_Send(&process_value, 1, MPI_INT, 0, rank, MPI_COMM_WORLD);
    }


    MPI_Finalize();
    

}

