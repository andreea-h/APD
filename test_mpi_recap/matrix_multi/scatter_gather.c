#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define MASTER 0


//inmultire de matrici
//fiecare proces are o linie din matrice
//numarul de procese = numarul de linii din matrice

int main (int argc, char **argv) {
    int rank, nrProcess;
    
    MPI_Init(&argc, &argv);
    
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nrProcess);

    
    int N = nrProcess; //matrice de dimenisune N * N
    int line[N]; //fiecare proces va avea cate o linie din matrice
    //adica fiecare proces i calculeaza linia i din matricea rezultat (i cu valorile de la 0 la N - 1 inclusiv)

    int matrix1[N][N];
    int matrix2[N][N];
    int result[N][N];

    int lines[N * N]; //contine el. din toate liniile din matrice

    //procesul root va genera cele 2 matrice 
    if (rank == MASTER) {
        
        int i, j;
        for (i = 0; i < N; i++) {
            for (j = 0; j < N; j++) {
                matrix1[i][j] = i * j + 3;
                matrix2[i][j] = 2 * i + j + 1;
            }
        }

        printf("Matrice 1:\n");
        for (i = 0; i < N; i++) {
            for (j = 0; j < N; j++) {
                printf("%d ", matrix1[i][j]);
            }
            printf("\n");
        }

        printf("Matrice 2:\n");
        for (i = 0; i < N; i++) {
            for (j = 0; j < N; j++) {
                printf("%d ", matrix2[i][j]);
            }
            printf("\n");
        }


    }

    if (rank == MASTER) {
        int i, j;
        int index = 0;
        for (j = 0; j < N; j++) {
            for (i = 0; i < N; i++) {
                lines[index] = matrix1[i][j];
                index++;
            }
        }
     /*   for (i = 0; i < N * N; i++) {
            printf("%d ", lines[i]);
        }
        printf("\n");*/
    }


    MPI_Scatter(lines, N, MPI_INT, line, N, MPI_INT, MASTER, MPI_COMM_WORLD);

    //masterul trimite catre fiecare proces, inclusiv lui insusi matricea a doua

    MPI_Bcast(matrix2, N * N, MPI_INT, MASTER, MPI_COMM_WORLD);


    printf("linia din matricea 1 primita de %d este: \n", rank);
    for (int j = 0; j < N; j++) {
        printf("%d ", line[j]);
    }

    printf("\n");
    MPI_Barrier(MPI_COMM_WORLD);

    int result_line[N];//linia rank din matricea rezultat
    int i, j;

    for (i = 0; i < N; i++) {
        result_line[i] = 0;
        for (j = 0; j < N; j++) {
            result_line[i] += line[j] * matrix2[j][i];
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
   
    MPI_Gather(result_line, N, MPI_INT, result, N, MPI_INT, MASTER, MPI_COMM_WORLD);


    if(rank == MASTER) {
        printf("resultat: \n");
        for (i = 0; i < N; i++) {
            for (j = 0; j < N; j++) {
                printf("%d ", result[i][j]);
            }
            printf("\n");
        }

    }

    MPI_Finalize();
    return 0;
}

