#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define MAX(a,b) ((a) > (b) ? a : b)

//N procese distribuite sub forma de inel
//fiecare proces are 2 valori intregi v1 si v2, valori distincte, diferite intre procese
//afla cele mai mari 2 valori dintre toate

int main (int argc, char *argv[])//se dau ca argument numtasks perechi de numere distincte
{
    int  numtasks, rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks); 
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    //memoreaza numerele fiecarui proces din argumente
    int nums[2];
    //fiecare proces memoreaza doar valorile care ii sunt asociate lui
    nums[0] = atoi(argv[2 * rank + 1]);
    nums[1] = atoi(argv[2 * rank + 2]);

    if (rank == 0) {//procesul initiator trimite valorile lui catre procesul 1
        //apoi va astepta valorile maxime gasite pe inel de la ultimul proces
        MPI_Send(&nums[0], 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
        MPI_Send(&nums[1], 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
       

        //primeste valorile maxime de la ultimul proces
        int max1, max2;
        MPI_Recv(&max1, 1, MPI_INT, numtasks - 1, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&max2, 1, MPI_INT, numtasks - 1, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        nums[0] = max1;
        nums[1] = max2;
        //la final, valorile cautate vor fi memorate la nivelul procesulul initiator
        printf("valorile maxime gasite sunt: %d %d\n", nums[0], nums[1]);
    }
    else if (rank != numtasks - 1) {
        //primeste valorile de la procesul precedent
        //le compara cu valorile actuale
        //actualizeaza valorile actuale cu cele mai mari 2 valori dintre cele 4 valori locale
        //trimite mai departe valorile proprii tocmai actualizate

        int num1, num2;
        MPI_Recv(&num1, 1, MPI_INT, rank - 1, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&num2, 1, MPI_INT, rank - 1, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        //determina maximul celor 4 valori
        
        int vect[4] = {num1, num2, nums[0], nums[1]};
        int max1 = -9999;
        int max2 = -9999;
        int i;
        for (i = 0; i < 4; i++) {
            if (vect[i] > max1) {
                max2 = max1;
                max1 = vect[i];
            }
            else if (vect[i] > max2) {
                max2 = vect[i];
            }
        }

        nums[0] = max1;
        nums[1] = max2;
        //trimite valorile locale mai departe
        MPI_Send(&nums[0], 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
        MPI_Send(&nums[1], 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
    
    }
    else { //ultimul proces va trimite valorile catre procesul 0(adica catre initiator)
        int num1, num2;
        MPI_Recv(&num1, 1, MPI_INT, rank - 1, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&num2, 1, MPI_INT, rank - 1, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        //determina maximul celor 4 valori

        int vect[4] = {num1, num2, nums[0], nums[1]};
        int max1 = -9999;
        int max2 = -9999;
        int i;
        for (i = 0; i < 4; i++) {
            if (vect[i] > max1) {
                max2 = max1;
                max1 = vect[i];
            }
            else if (vect[i] > max2) {
                max2 = vect[i];
            }
        }

        nums[0] = max1;
        nums[1] = max2;
        //trimite valorile locale mai departe
        MPI_Send(&nums[0], 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Send(&nums[1], 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

   // printf("Valorile procesului %d sunt: %d %d\n", rank, nums[0], nums[1]);

    
    MPI_Finalize();

}

