#include<mpi.h>
#include<stdio.h>
#include<stdlib.h>

int N;

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

// Use 'mpirun -np 20 --oversubscribe ./pipeline_sort' to run the application with more processes
int main(int argc, char * argv[]) {
	int rank;
	int nProcesses;
	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nProcesses);
	printf("Hello from %i/%i\n", rank, nProcesses);

	if (rank == 0) { // This code is run by a single process
		int intialValue = -1;
		int sorted = 0;
		int aux;
		int *v = (int*)malloc(sizeof(int) * (nProcesses - 1));
		int *vQSort = (int*)malloc(sizeof(int) * (nProcesses - 1));
		int i, val;

		// generate the vector v with random values
		// DO NOT MODIFY
		srandom(42);
		for(i = 0; i < nProcesses - 1; i++)
			v[i] = random() % 200;
		N = nProcesses - 1;
		displayVector(v);

		// make copy to check it against qsort
		// DO NOT MODIFY
		for(i = 0; i < nProcesses - 1; i++)
			vQSort[i] = v[i];
		qsort(vQSort, nProcesses - 1, sizeof(int), cmp);

		// TODO sort the vector v
		//trimite valoarea initiala asignata catre procesele worker
		for (i = 1; i < nProcesses; i++) {
			MPI_Send(&intialValue, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
		}

		//mai intai este trimis numarul de elemente al vectorul 
		for (i = 1; i < nProcesses; i++) {
			MPI_Send(&N, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
		}
		

		//trimite valorile din vector catre primul proces worker
		for (i = 0; i < N; i++) {
			int value = v[i];
			MPI_Send(&value, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
			//printf("Valoarea trimisa catre primul worker: %d\n", value);
		}

		//primeste valorile de la workeri si le salveaza in vectorul sortat
		for (i = 1; i < N + 1; i++) {
			int recv_value;
			MPI_Recv(&recv_value, 1, MPI_INT, i, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			v[i - 1] = recv_value;
		}


		displayVector(v);
		compareVectors(v, vQSort);
	} else {
		// TODO sort the vector v
		int saved_value;
		int N;

		//primeste de la master valoarea initiala salvata de fiecare worker
		MPI_Recv(&saved_value, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		//primeste dim. vector
		MPI_Recv(&N, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		//fiecare worker primeste (n-rank) numere de la rankul anterior
		int i;
		for (i = 0; i < N - rank + 1; i++) {
			int recv_num;
			MPI_Recv(&recv_num, 1, MPI_INT, rank - 1, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			
			if (saved_value == -1) {
				saved_value = recv_num;
			}
			else if (recv_num < saved_value) {	
				//trimite vechea valoarea catre urmatorul proces
				MPI_Send(&saved_value, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
				saved_value = recv_num; //actualizam valoarea
			}
			else if (recv_num >= saved_value) {
				//trimite valoarea catre procesul urmator
				MPI_Send(&recv_num, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
			}
		}

		//trimite catre master valoarea
		MPI_Send(&saved_value, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
	}

	MPI_Finalize();
	return 0;
}
