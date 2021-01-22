#include<mpi.h>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<stdbool.h> 
#include<time.h>

#define N 1000
#define MASTER 0

void compareVectors(int * a, int * b) {
	// DO NOT MODIFY
	int i;
	for(i = 0; i < N; i++) {
		if(a[i]!=b[i]) {
			printf("Sorted incorrectly\n");
			return;
		}
	}
	printf("Sorted correctly\n");
}

void displayVector(int * v) {
	// DO NOT MODIFY
	int i;
	int displayWidth = 2 + log10(v[N-1]);
/*	for(i = 0; i < N; i++) {
		printf("%*i", displayWidth, v[i]);
	}*/
	for(i = 0; i < N; i++) {
		printf("%d ", v[i]);
	}
	printf("\n");
}

int cmp(const void *a, const void *b) {
	// DO NOT MODIFY
	int A = *(int*)a;
	int B = *(int*)b;
	return A-B;
}
 
int main(int argc, char * argv[]) {
	int rank, i, j;
	int nProcesses;
	MPI_Init(&argc, &argv);
	int pos[N];
	int sorted = 0;
	int *v = (int*)malloc(sizeof(int)*N);
	int *vQSort = (int*)malloc(sizeof(int)*N);

	for (i = 0; i < N; i++)
		pos[i] = 0;

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nProcesses);
	printf("Hello from %i/%i\n", rank, nProcesses);

    if (rank == MASTER) { //genereaza vector cu valori unice
        // generate random vector
        srand(time(NULL));
		for(i = 0; i < N; i++) {
			
			int generated = random() % 2500;
			while(1) {
				
				generated = random() % 2500;
				bool ok = true;
				for (j = 0; j < i; j++) {
					if (generated == v[j]) {
						ok = false;
					}
				}
				if (ok == true) {
					break;
				}
			}
			v[i] = generated;				
		}
	
		displayVector(v);
	    // send the vector to all processes
	    for (i = 1; i < nProcesses; i++) {
	    	//trimite initial dimeniunea
	    	MPI_Send(v, N, MPI_INT, i, 0, MPI_COMM_WORLD);
	    }
	}

	if(rank == 0) {
		// DO NOT MODIFY
		displayVector(v);

		// make copy to check it against qsort
		// DO NOT MODIFY
		for(i = 0; i < N; i++)
			vQSort[i] = v[i];
		qsort(vQSort, N, sizeof(int), cmp);
		displayVector(vQSort);

		// sort the vector v
		
        // recv the new pozitions
        int sorted[N];
		int i;
		int index = 0;
		for (i = 1; i < nProcesses; i++) {
			int size;
			MPI_Recv(&size, 1, MPI_INT, i, 100, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			int pos_chunk[size];
			MPI_Recv(pos_chunk, size, MPI_INT, i, i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			int j;

			for (j = 0; j < size; j++) {
				sorted[pos_chunk[j]] = v[index];
				index++;
			}
		}

		v = sorted;
		displayVector(v);

		compareVectors(v, vQSort);
	} else {
		
        // compute the positions
        // send the new positions to process MASTER
		int vect[N];
		int k, j;
		MPI_Recv(vect, N, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		
		int dimen_process = N / (nProcesses - 1); //imparte la numarul de procese worker
		int pos_process[dimen_process];
		
		for (k = 0; k < dimen_process; k++) {
			pos_process[k] = 0;
		}
		for (k = 0; k < dimen_process; k++) {
			//printf("elementul analizat: %d ---", vect[k + (rank - 1) * dimen_process]);
			for (j = 0; j < N; j++) {
			//pentru fiecare element din bucata de vector se calculeaza care el. sunt mai mici decat el. curent
				if (vect[k + (rank - 1) * dimen_process] > vect[j]) {
					pos_process[k] += 1;
				}
			}
		}
		
		/*printf("elemente mai mici: \n");
		for (k = 0; k < dimen_process; k++) {
			printf("%d ", pos_process[k]);
		}*/
		
		//trimite pos catre procesul master
		MPI_Send(&dimen_process, 1, MPI_INT, 0, 100, MPI_COMM_WORLD);
		MPI_Send(pos_process, dimen_process, MPI_INT, 0, rank, MPI_COMM_WORLD);
	}

	//am rulat cu nrProcesses ales a.i. N este divizibil cu (nrProcesses - 1) (=nr de procese worker)

	MPI_Finalize();
	return 0;
}
