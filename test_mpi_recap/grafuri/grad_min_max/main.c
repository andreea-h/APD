#include<mpi.h>
#include<stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#define CONVERGENCE_COEF 100

/**
 * Run: mpirun -np 12 ./a.out
 */

static int num_neigh;
static int *neigh;

//citeste din fisier numarul de vecini si vecinii pentru fiecare nod 
void read_neighbours(int rank) {
    FILE *fp;
    char file_name[15];
    sprintf(file_name, "./files/%d.in", rank);

    fp = fopen(file_name, "r");
	fscanf(fp, "%d", &num_neigh);

	neigh = malloc(sizeof(int) * num_neigh);

	for (size_t i = 0; i < num_neigh; i++)
		fscanf(fp, "%d", &neigh[i]);
}

int* get_dst(int rank, int numProcs, int leader) {
	MPI_Status status;
	MPI_Request request;

	/* Vectori de parinti */
	int *v = malloc(sizeof(int) * numProcs);
	int *vRecv = malloc(sizeof(int) * numProcs);

	memset(v, -1, sizeof(int) * numProcs);
	memset(vRecv, -1, sizeof(int) * numProcs);
	
	if (rank == leader)
		v[rank] = -1;
	else {
		/* Daca procesul curent nu este liderul, inseamna ca va astepta un mesaj de la un parinte */
		MPI_Recv(vRecv, numProcs, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
		v[rank] = status.MPI_SOURCE; //v[rank] este parintele nodului curent
	}


	/*
	*  TODO2: Pentru fiecare proces vecin care nu este parintele procesului curent,
	*		  voi trimite vectorul de parinti propriu. 
	*/

	int i;
	for (i = 0; i < num_neigh; i++) {
		if (v[rank] != neigh[i]) {
			MPI_Send(v, numProcs, MPI_INT, neigh[i], 0, MPI_COMM_WORLD);
		}
	}

	/*
	*  TODO2: Vom astepta de la fiecare proces vecin care nu este parintele procesului curent 
	*		  vectorul de parinti si actualizam vectorul propriu de parinti daca exista informatii aditionale
	*/

	for (i = 0; i < num_neigh; i++) {
		if (v[rank] != neigh[i]) {
			MPI_Recv(vRecv, numProcs, MPI_INT, neigh[i], 0, MPI_COMM_WORLD, &status);
			//actualizare v cu valori din vRecv
			int j;
			for (j = 0; j < numProcs; j++) {
				if (vRecv[j] != -1 && v[j] == -1) {
					v[j] = vRecv[j];
				}
			}
		}
	}

	/*
	*  TODO2: Topologia fiind deja stabilita, orice proces ce nu este lider va propaga
	* 		  vectorul de vecini parintelui lui si va astepta topologia completa de la acesta
	*/

	if (rank != leader) {
		MPI_Send(v, numProcs, MPI_INT, v[rank], 0, MPI_COMM_WORLD);
		//asteapta topologia completa de la parintele lui
		MPI_Recv(v, numProcs, MPI_INT, v[rank], 0, MPI_COMM_WORLD, &status);
	}

	/*
	*  TODO2: Procesul curent va trimite doar copiilor lui topologia completa
	*/
	for (i = 0; i < numProcs; i++) {
		if (v[i] == rank) { //rank este parintele nodului i
			MPI_Send(v, numProcs, MPI_INT, i, 0, MPI_COMM_WORLD);
		}
	}


	for (int i = 0; i < numProcs && rank == leader; i++) {
		printf("The node %d has the parent %d\n", i, v[i]);
	}

	return v;
}

void leader_chosing(int rank, int nProcesses, int *grad_min, int *grad_max) { //algoritmul heartbeat
	*grad_min = num_neigh;
	*grad_max = num_neigh;
	
	/* Executam acest pas pana ajungem la convergenta */
	for (int k = 0; k < CONVERGENCE_COEF; k++) {
		/* TODO1: Pentru fiecare vecin, vom trimite liderul pe care il cunosc 
		* 		 si voi astepta un mesaj de la orice vecin
		* 		 Daca liderul e mai mare decat al meu, il actualizez pe al meu
		*/
		//trimite leader-ul actual catre vecini
		int i;

		//trimite catre vecini gradul minim local, respectiv gradul maxim local
		for (i = 0; i < num_neigh; i++) {
			MPI_Send(grad_min, 1, MPI_INT, neigh[i], 0, MPI_COMM_WORLD);
			MPI_Send(grad_max, 1, MPI_INT, neigh[i], 0, MPI_COMM_WORLD);
		
			//primeste grad_min si max de la vecini si actualiazeaza valorile
			int recv_min;
			int recv_max;
			MPI_Recv(&recv_min, 1, MPI_INT, neigh[i], MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			if (recv_min < *grad_min) {
				*grad_min = recv_min;
			}
			MPI_Recv(&recv_max, 1, MPI_INT, neigh[i], MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			if (recv_max > *grad_max) {
				*grad_max = recv_max;
			}
		}
	}

	MPI_Barrier(MPI_COMM_WORLD);
	printf("%i/%i: grad_max %d | grad_min %d\n", rank, nProcesses, *grad_max, *grad_min);

	return;
}

double get_number_of_nodes(int rank, int leader) {
	
	double val;
	if (leader == rank) {
		val = 1.0;
	} else {
		val = 0.0;
	}

	double recvd = 0;
	/* Executam acest pas pana ajungem la convergenta */
	for (int k = 0; k < CONVERGENCE_COEF; k++) {
		/* TODO3: Pentru fiecare vecin, vom trimite valoarea pe care o cunosc
		* 		 si voi astepta un mesaj de la el
		* 		 Cu valoarea primita, actualizam valoarea cunoscuta ca fiind
		* 		 media dintre cele 2
		*/
		int i;
		for (i = 0; i < num_neigh; i++) {
			//trimite val catre fiecare vecin
			MPI_Send(&val, 1, MPI_DOUBLE, neigh[i], 0, MPI_COMM_WORLD);
			MPI_Recv(&recvd, 1, MPI_DOUBLE, neigh[i], 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			val += recvd;
			val = val / 2.0;
		}
		
	}
	
	MPI_Barrier(MPI_COMM_WORLD);
	//printf("valoarea: %lf rezultat: %lf\n", val, (int)1/val);
	double result = (int)1 / val;
	return result;
}

int ** get_topology(int rank, int nProcesses, int * parents, int leader) {
	int ** topology = malloc(sizeof(int*) * nProcesses);
	int ** vTopology = malloc(sizeof(int*) * nProcesses);
	
	for (size_t i = 0; i < nProcesses; i++) {
		topology[i] = calloc(sizeof(int), nProcesses);
		vTopology[i] = calloc(sizeof(int), nProcesses);
	}

	for (size_t i = 0; i < num_neigh; i++) {
		topology[rank][neigh[i]] = 1;
	}

	/* TODO4: Primim informatii de la toti copiii si actualizam matricea de topologie */
	int i;
	for (i = 0; i < nProcesses; i++) {
		if (parents[i] == rank) { //i este copilul nodului rank
			//primeste pe rand liniile matricei
			int j;
			for (j = 0; j < nProcesses; j++) {
				int *line = (int *)malloc(sizeof(int) * nProcesses);
				MPI_Recv(line, nProcesses, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				int l, c;
				for (l = 0; l < nProcesses; l++) {
					if (line[l] == 1) {
						topology[j][l] = 1;
					}
				}
				free(line);
			}
		}
	}
	

	/* TODO4: Propagam matricea proprie catre parinte */
	if (parents[rank] != -1) {
		//sunt trimise pe rand liniile matricei
		int l, c;
		for (l = 0; l < nProcesses; l++) {
			int *line = (int *)malloc(nProcesses * sizeof(int));
			int e;
			for (e = 0; e < nProcesses; e++) {
				line[e] = topology[l][e];
			}
			MPI_Send(line, nProcesses, MPI_INT, parents[rank], 0, MPI_COMM_WORLD);
		}
		
	}

	/* TODO4: Daca nu suntem liderul, asteptam topologia completa de la parinte  */
	if (rank != leader) {
		int j;
		for (j = 0; j < nProcesses; j++) {
			int *line = (int *)malloc(sizeof(int) * nProcesses);
			MPI_Recv(line, nProcesses, MPI_INT, parents[rank], 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			int l, c;
			for (l = 0; l < nProcesses; l++) {
				topology[j][l] = line[l];
				
			}
			free(line);
		}
	}
	
	/* TODO4: Trimitem topologia completa copiilor */
	for (i = 0; i < nProcesses; i++) {
		if (parents[i] == rank) {
			int l, c;
			for (l = 0; l < nProcesses; l++) {
				int *line = (int *)malloc(nProcesses * sizeof(int));
				int e;
				for (e = 0; e < nProcesses; e++) {
					line[e] = topology[l][e];
				}
				MPI_Send(line, nProcesses, MPI_INT, i, 0, MPI_COMM_WORLD);
			}
		}
	}

	return topology;
}

int main(int argc, char * argv[]) {
	int rank, nProcesses, num_procs, leader;
	int *parents, **topology;

	MPI_Init(&argc, &argv);
	MPI_Status status;
	MPI_Request request;

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nProcesses);

	if (nProcesses != 12) {
		printf("please run with: mpirun --oversubscribe -np 12 %s\n", argv[0]);
		MPI_Finalize();	
		exit(0);
	}
	 
	read_neighbours(rank);
	//gasirea nodului leader
	int min_grad; //gradul maxim din graf
	int max_grad; //gradul minim din graf
	leader_chosing(rank, nProcesses, &min_grad, &max_grad);

	
/*	MPI_Barrier(MPI_COMM_WORLD);

	//gaseste arborele de acoperire (vector de tati)
	parents = get_dst(rank, nProcesses, leader);


	MPI_Barrier(MPI_COMM_WORLD);

	//verificarea arborelui de acoperire prin verificarea numarului de noduri
	//cu alg. epidemic
	double num_procs_result = get_number_of_nodes(rank, leader);
	num_procs = round(num_procs_result);
	
	printf("%d/%d There are %d processes\n", rank, nProcesses, num_procs);

	topology = get_topology(rank, nProcesses, parents, leader);

	for (size_t i = 0; i < nProcesses && rank == 0; i++)
	{
		for (size_t j = 0; j < nProcesses; j++)
		{
			printf("%2d ", topology[i][j]);	
		}
		printf("\n");
	}
	*/
	MPI_Finalize();
	return 0;
}