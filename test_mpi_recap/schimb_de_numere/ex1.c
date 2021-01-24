#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define SIZE 4

//2 procese A si B au cate un set de numere
//procesele schimba intre ele in mod repetat cate o pereche de valori
//pana cand toate valori din A sunt mai mici decat orice valoare din B


int getMax(int *vect, int nr, int *pos) {
    int max = -99999;
    int i;
    for (i = 0; i < nr; i++) {
        if (vect[i] > max) {
            max = vect[i];
            *pos = i;
        }
    }
    return max;
}


int getMin(int *vect, int nr, int *pos) {
    int min = 99999;
    int i;
    for (i = 0; i < nr; i++) {
        if (vect[i] < min) {
            min = vect[i];
            *pos = i;
        }
    }
    return min;
}


int main (int argc, char *argv[])
{
    int  numtasks, rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks); //numtasks trebuie sa fie 2
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
   
    //toate valorile din A < toate valorile din B
    //deci valoarea maxima din A < valoarea minima din B
    //B va trimite mereu catre A valoarea lui minima
    //A va compara valoarea lui maxima cu valoarea minima primita de la B 
    //si il va anunta de B daca trebuie sa ii mai trimita un numar

    //A va trimite catre B mereu maximul din multimea sa
    //B va trimite mereu catre A minimul din multimea sa
    if (rank == 0) {//A
        //multimea lui A
        int *s = (int *)malloc(sizeof(int) * SIZE);
        s[0] = 7;
        s[1] = 4;
        s[2] = 1;
        s[3] = 5;


        while(1) {
             //determina maximul din A
            int pos;
            int max = getMax(s, SIZE, &pos);
            //primeste minimul din B
            int recv;
            MPI_Recv(&recv, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
           
            if (max > recv) { //se face schimb de numere
                //trimite catre B un mesaj care sa il anunte ca urmeaza sa ii trimit un numar si ca astept un numar de la el
                char *mesg = strdup("yes");
                MPI_Send(mesg, strlen(mesg) + 1, MPI_CHAR, 1, 0, MPI_COMM_WORLD);

                MPI_Send(&max, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
                int recvb;
                MPI_Recv(&recvb, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                s[pos] = recvb;
            }
            else {
                //trimite catre B un mesaj care sa ii arate ca am incheiat transferul
                char *mesg = strdup("no");
                MPI_Send(mesg, strlen(mesg) + 1, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
                break;
            }
        }
       

        int i;
        printf("A: ");
        for (i = 0; i < SIZE; i++) {
            printf("%d ", s[i]);
        }
        printf("\n");
    }   
    else { //B
        int *t = (int *)malloc(sizeof(int) * SIZE);
        t[0] = 2;
        t[1] = 1;
        t[2] = 9;
        t[3] = 8;

        while(1) {
            int pos;
            int min = getMin(t, SIZE, &pos);
            //trimite catre A minimul
            MPI_Send(&min, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

            //primeste un mesaj de la A care anunta daca trebuie sa trimite sau nu un numar la schimb

            MPI_Status status;
            int buffer_size;

            MPI_Probe(0, 0, MPI_COMM_WORLD, &status);
                            
            MPI_Get_count(&status, MPI_CHAR, &buffer_size);
            char *response = (char *)malloc(sizeof(char) * buffer_size);

            MPI_Recv(response, buffer_size, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            if (strcmp(response, "yes") == 0) {
                //primeste numarul trimis de A
                int recva;
                MPI_Recv(&recva, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                //trimite minimul detinut catre A
                MPI_Send(&min, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
                t[pos] = recva;
            }
            else {
                break;
            }
        }
       

        int i;
        printf("B: ");
        for (i = 0; i < SIZE; i++) {
            printf("%d ", t[i]);
        }
        printf("\n");
    }

    MPI_Finalize();

}

