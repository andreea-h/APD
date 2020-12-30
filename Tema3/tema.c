#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <stdbool.h>
 
#define MASTER 0
#define MASTER_THREADS 4
#define MAX_LENGTH 50

char *file_name;

/*functiile de thread executata de thread-urile master*/
void *master_f_horror(void *arg) {
	FILE *fptr = fopen(file_name, "r");
	if (fptr == NULL) {
		printf("Eroare la deschiderea fisierului de intrare\n");
		exit(-1);
	}


	char *line_buffer;
	size_t buffer_size; //dimeniunea liniei(inluzand\n)
	ssize_t result;
	char *paragraph = NULL;
	bool check_paragraph_end = false;

	while ((result = getline(&line_buffer, &buffer_size, fptr)) != -1) {
		if (strcmp(line_buffer, "horror\n") == 0) {
			//citeste continutul paragrafului si trimite-l catre workerul 1
			while (check_paragraph_end == false) {
				result = getline(&line_buffer, &buffer_size, fptr);
				if (result != -1) {
					if (paragraph == NULL) {
						paragraph = (char *)malloc(strlen(line_buffer) * sizeof(char));
						strcpy(paragraph, line_buffer);
					}
					else {
						paragraph = (char *)realloc(paragraph, strlen(paragraph) + strlen(line_buffer));
						strcat(paragraph, line_buffer);
					}
					//este identificat finalul paragrafului
					if (paragraph[strlen(paragraph) - 2] == '\n') {
						paragraph[strlen(paragraph) - 1] = '\0';
						check_paragraph_end = true;
						//s-a citit un paragraf de tipul horror
						//printf("Paragraf horror: %s", paragraph);
						MPI_Send(paragraph, strlen(paragraph), MPI_CHAR, 1, 0, MPI_COMM_WORLD);
						free(paragraph);
						paragraph = NULL;
					}
				}
			}
			if (check_paragraph_end == true) {
				check_paragraph_end = false;
			}
		}
	}

	fclose(fptr);
	pthread_exit(NULL);
}

void *master_f_comedy(void *arg) {
	FILE *fptr = fopen(file_name, "r");
	if (fptr == NULL) {
		printf("Eroare la deschiderea fisierului de intrare\n");
		exit(-1);
	}

	char *line_buffer;
	size_t buffer_size; //dimeniunea liniei(inluzand\n)
	ssize_t result;
	char *paragraph = NULL;
	bool check_paragraph_end = false;

	while ((result = getline(&line_buffer, &buffer_size, fptr)) != -1) {
		if (strcmp(line_buffer, "comedy\n") == 0) {
			//citeste continutul paragrafului si trimite-l catre workerul 1
			while (check_paragraph_end == false) {
				result = getline(&line_buffer, &buffer_size, fptr);
				if (result != -1) {
					if (paragraph == NULL) {
						paragraph = (char *)malloc(strlen(line_buffer) * sizeof(char));
						strcpy(paragraph, line_buffer);
					}
					else {
						paragraph = (char *)realloc(paragraph, strlen(paragraph) + strlen(line_buffer));
						strcat(paragraph, line_buffer);
					}

					if (paragraph[strlen(paragraph) - 2] == '\n') {
						paragraph[strlen(paragraph) - 1] = '\0';
						check_paragraph_end = true;
						//s-a citit un paragraf de tipul comedy
						//printf("Paragraf comedy: %s", paragraph);
						MPI_Send(paragraph, strlen(paragraph), MPI_CHAR, 2, 0, MPI_COMM_WORLD);
						free(paragraph);
						paragraph = NULL;
					}
				}
			}
			if (check_paragraph_end == true) {
				check_paragraph_end = false;
			}
		}
	}

	fclose(fptr);
	pthread_exit(NULL);
}

void *master_f_scifi(void *arg) {
	FILE *fptr = fopen(file_name, "r");
	if (fptr == NULL) {
		printf("Eroare la deschiderea fisierului de intrare\n");
		exit(-1);
	}

	char *line_buffer;
	size_t buffer_size; //dimeniunea liniei(inluzand\n)
	ssize_t result;
	char *paragraph = NULL;
	bool check_paragraph_end = false;

	while ((result = getline(&line_buffer, &buffer_size, fptr)) != -1) {
		if (strcmp(line_buffer, "science-fiction\n") == 0) {
			//citeste continutul paragrafului si trimite-l catre workerul 1
			while (check_paragraph_end == false) {
				result = getline(&line_buffer, &buffer_size, fptr);
				if (result != -1) {
					if (paragraph == NULL) {
						paragraph = (char *)malloc(strlen(line_buffer) * sizeof(char));
						strcpy(paragraph, line_buffer);
					}
					else {
						paragraph = (char *)realloc(paragraph, strlen(paragraph) + strlen(line_buffer));
						strcat(paragraph, line_buffer);
					}

					if (paragraph[strlen(paragraph) - 2] == '\n') {
						paragraph[strlen(paragraph) - 1] = '\0';
						check_paragraph_end = true;
						//s-a citit un paragraf de tipul scifi
						//printf("Paragraf scifi: %s", paragraph);
						MPI_Send(paragraph, strlen(paragraph), MPI_CHAR, 3, 0, MPI_COMM_WORLD);
						free(paragraph);
						paragraph = NULL;
					}
				}
			}
			if (check_paragraph_end == true) {
				check_paragraph_end = false;
			}
		}
	}
	
	fclose(fptr);
	pthread_exit(NULL);
}

void *master_f_fantasy(void *arg) {
	FILE *fptr = fopen(file_name, "r");
	if (fptr == NULL) {
		printf("Eroare la deschiderea fisierului de intrare\n");
		exit(-1);
	}

	char *line_buffer;
	size_t buffer_size; //dimeniunea liniei(inluzand\n)
	ssize_t result;
	char *paragraph = NULL;
	bool check_paragraph_end = false;

	while ((result = getline(&line_buffer, &buffer_size, fptr)) != -1) {
		if (strcmp(line_buffer, "fantasy\n") == 0) {
			//citeste continutul paragrafului si trimite-l catre workerul 1
			while (check_paragraph_end == false) {
				result = getline(&line_buffer, &buffer_size, fptr);
				if (result != -1) {
					if (paragraph == NULL) {
						paragraph = (char *)malloc(strlen(line_buffer) * sizeof(char));
						strcpy(paragraph, line_buffer);
					}
					else {
						paragraph = (char *)realloc(paragraph, strlen(paragraph) + strlen(line_buffer));
						strcat(paragraph, line_buffer);
					}

					if (paragraph[strlen(paragraph) - 2] == '\n') {
						paragraph[strlen(paragraph) - 1] = '\0';
						check_paragraph_end = true;
						//s-a citit un paragraf de tipul fantasy
						//printf("Paragraf fantasy: %s", paragraph);
						//MPI_Send(paragraph, strlen(paragraph), MPI_CHAR, 4, 0, MPI_COMM_WORLD);
						free(paragraph);
						paragraph = NULL;
					}
				}
			}
			if (check_paragraph_end == true) {
				check_paragraph_end = false;
			}
		}
	}


	fclose(fptr);
	pthread_exit(NULL);
}

/*returneaza numarul de linii pe care le are un paragraf*/
int get_num_lines(char *paragraph) {
	int result = 0;
	//numarul de linii = nr de caractere \n din paragraful primit de threadul reader al unui worker
	int i;
	for (i = 0; i < strlen(paragraph); i++) {
		if (paragraph[i] == '\n') {
			result++;
		}
	}
	return result;
}

/*functia de thread executata de thread-ul reader pe de workeri*/
void *worker_reader_f(void *arg) {
	long worker_rank = *(long*) arg;
	//face receive la paragraful care i-a fost trimis de catre thread-ul aferent tipului sau din master
	char *paragraph;//!!
	MPI_Status status;
	int buffer_size;

	MPI_Probe(0, 0, MPI_COMM_WORLD, &status);
	MPI_Get_count(&status, MPI_CHAR, &buffer_size);

	paragraph = (char *)malloc(buffer_size * sizeof(char));
	MPI_Recv(paragraph, buffer_size, MPI_CHAR, MASTER, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

	int lines_count = get_num_lines(paragraph);
	printf("Mesajul primit in worker-ul %ld este: %s; Are %d linii\n", worker_rank, paragraph, lines_count);
	//numara cate linii contine paragraful primit
	


	//porneste atatea thread-uri de cat este nevoie pt a procesa paragraful in functie de numarul de linii din paragraf

	//asteapta ca thread-urile care fac procesarea sa isi incheie treaba, apoi trimite inapoi la master paragraful intreg procesat

	pthread_exit(NULL);
}


void *worker_data_processing_horror(void *arg) {

}


int main (int argc, char *argv[]) {
	if (argc != 2) {
		printf("[!] Foloseste \"mpirun -np [nr_nodes] tema [nume_fisier_input]\"");
		exit(-1);
	}
	file_name = strdup(argv[1]);
	

	int numtasks, rank, len;
    char hostname[MPI_MAX_PROCESSOR_NAME];
 
 	int provided;
 	//MPI_THREAD_MULTIPLE - level of desired thread support
 	//(mai multe threaduri pot apela MPI library oricand fara restricitii)
 	//provided - level of provided thread support
    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);


    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    MPI_Get_processor_name(hostname, &len);

    if (rank == MASTER) {
    	//porneste cele 4 threaduri care proceseaza fisierul de intrare
    	pthread_t master_threads[MASTER_THREADS];
    	long thread_id[MASTER_THREADS];
    	long i;
    	for (i = 0; i < MASTER_THREADS; i++) {
    		thread_id[i] = i;
    		//porneste thread-urile care se ocupa de trimiterea paragrafelor la workeri
    		if (i == 0) {
    			int r = pthread_create(&master_threads[i], NULL, master_f_horror, NULL);
    			if (r != 0) {
    				printf("Eroare la crearea thread-ului 0\n din master");
    				exit(-1);
    			}
    		}
    		if (i == 1) {
    			int r = pthread_create(&master_threads[i], NULL, master_f_comedy, NULL);
    			if (r != 0) {
    				printf("Eroare la crearea thread-ului 1\n din master");
    				exit(-1);
    			}
    		}
    		if (i == 2) {
    			int r = pthread_create(&master_threads[i], NULL, master_f_scifi, NULL);
    			if (r != 0) {
    				printf("Eroare la crearea thread-ului 2\n din master");
    				exit(-1);
    			}
    		}
    		if (i == 3) {
    			int r = pthread_create(&master_threads[i], NULL, master_f_fantasy, NULL);
    			if (r != 0) {
    				printf("Eroare la crearea thread-ului 3\n din master");
    				exit(-1);
    			}
    		}
    	}
    	void *status;
    	for (i = 0; i < MASTER_THREADS; i++) {
    		thread_id[i] = i;
    		int r = pthread_join(master_threads[i], &status);
    		if (r != 0) {
    			printf("Eroare la asteptarea thread-ului %ld\n", i);
    			exit(-1);
    		}
    	}
    	//faca receive la paragrafele procesate trimite de catre workeri

    	//retine in memoria nodului master tot textul procesat, care este scris la final in fisier


    }
    else {
    	//in fiecare worker este pornit initial thread-ul 'reader' care recepteaza datele trimise de master
        pthread_t reader;
        long worker_rank = (long) rank;
        int r = pthread_create(&reader, NULL, worker_reader_f, &worker_rank);

        void *return_value;
        r = pthread_join(reader, &return_value);
    	if (r != 0) {
    		printf("Eroare la asteptarea thread-ului reader din worker-ul %d\n", rank);
    		exit(-1);
    	}

    	
    }

    MPI_Finalize();    
    
}
