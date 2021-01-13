#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <unistd.h>
#include <math.h>
 
#define MASTER 0
#define MASTER_THREADS 4
#define MAX_LENGTH 50
#define HORROR_WORKER 1
#define COMEDY_WORKER 2
#define SCIFI_WORKER 3
#define FANTASY_WORKER 4
#define PARAGRAPH_NR 5
#define MAX_SIZE 600000000
#define NR_LINES 20 
//#define P sysconf(_SC_NPROCESSORS_CONF) /*numarul de fire de executie disponibile*/

pthread_mutex_t mutex;

char *file_name;
/*retine tipurile tuturor paragrafelor care apar in fisier in ordine*/
int *paragraphs_order;
int nr_total_paragraphs;
int capacity;
int nr_paragraphs_comedy;
int nr_paragraphs_sci_fi;
int nr_paragraphs_fantasy;
int nr_paragraphs_horror;

pthread_barrier_t barrier;

/*functia de thread executata in thread-ul master care trimite paragrafele horror*/
/*este determinat si trimis catre worker numarul de paragrafe de tip horror, apoi se trimit pe rand paragrafele*/
/*functiile de thread executata de thread-urile master*/
void *master_f_horror(void *arg) {
	FILE *fptr = fopen(file_name, "r");
	if (fptr == NULL) {
		printf("Eroare la deschiderea fisierului de intrare\n");
		exit(-1);
	}

	char *line_buffer;
	size_t buffer_size; //dimeniunea liniei(incluzand\n)
	ssize_t result;
	char *paragraph = NULL;
	bool check_paragraph_end = false;

	//pthread_barrier_wait(&barrier);
	//trimite numarul de paragrafe catre workerul aferent tipului de paragraf
	//MPI_Send(&nr_paragraphs_horror, 1, MPI_INT, HORROR_WORKER, HORROR_WORKER, MPI_COMM_WORLD);

	//reseteaza fptr la inceputul fisierului
	//fseek(fptr, 0, SEEK_SET);

	while ((result = getline(&line_buffer, &buffer_size, fptr)) != -1) {
		if (strcmp(line_buffer, "horror\n") == 0) {
			//citeste continutul paragrafului si trimite-l catre workerul 1
			while (check_paragraph_end == false) {
				result = getline(&line_buffer, &buffer_size, fptr);
				if (result != -1) {
					if (paragraph == NULL) {
						paragraph = (char *)malloc((strlen(line_buffer) + 1)* sizeof(char));
						strcpy(paragraph, line_buffer);
					}
					else {
						int size = strlen(paragraph) + strlen(line_buffer) + 2;
						char *paragraph_aux = (char *)realloc(paragraph, size);
						if (!paragraph_aux) {
							exit(-1);
						}
						paragraph = paragraph_aux;
						strcat(paragraph, line_buffer);
					}


					//este identificat finalul paragrafului
					if (paragraph[strlen(paragraph) - 2] == '\n') {
						paragraph[strlen(paragraph) - 1] = '\0';
						check_paragraph_end = true;
						//s-a citit un paragraf de tipul horror
						//printf("[MASTER]Paragraf horror: %s", paragraph);
			
						//int buffer_dim = MAX_SIZE + MPI_BSEND_OVERHEAD;
					   // char *buffer_memory = (char *)malloc(buffer_dim * sizeof(char));
					   // MPI_Buffer_attach(buffer_memory, buffer_dim);
						//MPI_Request request = MPI_REQUEST_NULL;
						//printf("Dimensiune paragraf trimis: %ld\n", strlen(paragraph));
						MPI_Bsend(paragraph, strlen(paragraph) + 1, MPI_CHAR, HORROR_WORKER, HORROR_WORKER, MPI_COMM_WORLD);

						//MPI_Buffer_detach(&buffer_memory, &buffer_dim);

						free(paragraph);
						paragraph = NULL;

						//primeste paragraful procesat de la worker
					/*	char *processed_paragraph;
						MPI_Status status;
						int buffer_size;
						MPI_Probe(HORROR_WORKER, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
							
						MPI_Get_count(&status, MPI_CHAR, &buffer_size);
						
						processed_paragraph = (char *)malloc(buffer_size * sizeof(char));
						MPI_Recv(processed_paragraph, buffer_size, MPI_CHAR, HORROR_WORKER, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);*/
						//printf("[HORROR-MASTER]\n%s\n", processed_paragraph);
					}
				} 
				else {

					//int buffer_dim = MAX_SIZE + MPI_BSEND_OVERHEAD;
					//char *buffer_memory = (char *)malloc(buffer_dim * sizeof(char));
					//MPI_Buffer_attach(buffer_memory, buffer_dim);
				//	MPI_Request request = MPI_REQUEST_NULL;

				//	printf("Dimensiune paragraf trimis: %ld\n", strlen(paragraph));
					MPI_Bsend(paragraph, strlen(paragraph) + 1, MPI_CHAR, HORROR_WORKER, HORROR_WORKER, MPI_COMM_WORLD);
					//MPI_Buffer_detach(&buffer_memory, &buffer_dim);

					free(paragraph);
					paragraph = NULL;

					//primeste paragraful procesat de la worker
					//primeste paragraful procesat de la worker
				/*	char *processed_paragraph;
					MPI_Status status;
					int buffer_size;
					MPI_Probe(HORROR_WORKER, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
					
					MPI_Get_count(&status, MPI_CHAR, &buffer_size);
						
					processed_paragraph = (char *)malloc(buffer_size * sizeof(char));
					MPI_Recv(processed_paragraph, buffer_size, MPI_CHAR, HORROR_WORKER, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);*/	
					//printf("[HORROR-MASTER]\n%s\n", processed_paragraph);

					break;
				}
				
			}
			if (check_paragraph_end == true) {
				check_paragraph_end = false;
			}
		}
	}
	
	//trimite un mesaj care anunta ca s-au trimis toate paragrafele de tipul horror
	char *message = strdup("DONE");
	//MPI_Request request = MPI_REQUEST_NULL;

	MPI_Bsend(message, strlen(message) + 1, MPI_CHAR, HORROR_WORKER, HORROR_WORKER, MPI_COMM_WORLD);
	//printf("DONE A FOST TRIMIS DIN MASTER HORROR\n");
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

	/*int nr_paragraphs = 0;
	while ((result = getline(&line_buffer, &buffer_size, fptr)) != -1) {
		if (strcmp(line_buffer, "comedy\n") == 0) {
			nr_paragraphs_comedy++;
		}
	}*/
	//pthread_barrier_wait(&barrier);
	//MPI_Send(&nr_paragraphs_comedy, 1, MPI_INT, COMEDY_WORKER, COMEDY_WORKER, MPI_COMM_WORLD);


	//reseteaza fptr la inceputul fisierului
	//fseek(fptr, 0, SEEK_SET);

	while ((result = getline(&line_buffer, &buffer_size, fptr)) != -1) {
		if (strcmp(line_buffer, "comedy\n") == 0) {
			//citeste continutul paragrafului si trimite-l catre workerul 1
			while (check_paragraph_end == false) {
				result = getline(&line_buffer, &buffer_size, fptr);
				if (result != -1) {
					if (paragraph == NULL) {
						paragraph = (char *)malloc((strlen(line_buffer) + 1)* sizeof(char));
						strcpy(paragraph, line_buffer);
					}
					else {
						int size = strlen(paragraph) + strlen(line_buffer) + 2;
						char *paragraph_aux = (char *)realloc(paragraph, size);
						if (!paragraph_aux) {
							exit(-1);
						}
						paragraph = paragraph_aux;
						strcat(paragraph, line_buffer);
					}

					if (paragraph[strlen(paragraph) - 2] == '\n') {
						paragraph[strlen(paragraph) - 1] = '\0';
						check_paragraph_end = true;
						//s-a citit un paragraf de tipul comedy

						//int buffer_dim = strlen(paragraph) + 1 + MPI_BSEND_OVERHEAD * nr_total_paragraphs;
						//int buffer_dim = MAX_SIZE + MPI_BSEND_OVERHEAD;
					   // char *buffer_memory = (char *)malloc(buffer_dim * sizeof(char));
					   // MPI_Buffer_attach(buffer_memory, buffer_dim);

						//MPI_Request request = MPI_REQUEST_NULL;

					//	printf("Dimensiune paragraf trimis: %ld\n", strlen(paragraph));
						MPI_Bsend(paragraph, strlen(paragraph) + 1, MPI_CHAR, COMEDY_WORKER, COMEDY_WORKER, MPI_COMM_WORLD);
						//MPI_Buffer_detach(&buffer_memory, &buffer_dim);
						free(paragraph);
						paragraph = NULL;
						//primeste paragraful procesat de la worker
					/*	char *processed_paragraph;
						MPI_Status status;
						int buffer_size;
						MPI_Probe(COMEDY_WORKER, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
								
						MPI_Get_count(&status, MPI_CHAR, &buffer_size);
							
						processed_paragraph = (char *)malloc(buffer_size * sizeof(char));
						MPI_Recv(processed_paragraph, buffer_size, MPI_CHAR, COMEDY_WORKER, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);*/
						//printf("[COMEDY-MASTER]\n%s\n", processed_paragraph);
					}

				}
				else {

					//int buffer_dim = MAX_SIZE + MPI_BSEND_OVERHEAD;
				//	char *buffer_memory = (char *)malloc(buffer_dim * sizeof(char));
					//MPI_Buffer_attach(buffer_memory, buffer_dim);

					//MPI_Request request = MPI_REQUEST_NULL;
				//	printf("Dimensiune paragraf trimis: %ld\n", strlen(paragraph));
					MPI_Bsend(paragraph, strlen(paragraph) + 1, MPI_CHAR, COMEDY_WORKER, COMEDY_WORKER, MPI_COMM_WORLD);
					//MPI_Buffer_detach(&buffer_memory, &buffer_dim);
					free(paragraph);
					paragraph = NULL;

					//primeste paragraful procesat de la worker
				/*	char *processed_paragraph;
					MPI_Status status;
					int buffer_size;
					MPI_Probe(COMEDY_WORKER, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
							
					MPI_Get_count(&status, MPI_CHAR, &buffer_size);
						
					processed_paragraph = (char *)malloc(buffer_size * sizeof(char));
					MPI_Recv(processed_paragraph, buffer_size, MPI_CHAR, COMEDY_WORKER, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);*/
					//printf("[COMEDY-MASTER]\n%s\n", processed_paragraph);

					break;
				}
			}
			if (check_paragraph_end == true) {
				check_paragraph_end = false;
			}
		}
	}

	//trimite un mesaj care anunta ca s-au trimis toate paragrafele de tipul horror
	//MPI_Request request = MPI_REQUEST_NULL;
	char *message = strdup("DONE");
	MPI_Bsend(message, strlen(message) + 1, MPI_CHAR, COMEDY_WORKER, COMEDY_WORKER, MPI_COMM_WORLD);
	//printf("DONE A FOST TRIMIS DIN MASTER COMEDY\n");
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

	/*int nr_paragraphs = 0;
	while ((result = getline(&line_buffer, &buffer_size, fptr)) != -1) {
		if (strcmp(line_buffer, "science-fiction\n") == 0) {
			nr_paragraphs++;
		}
	}*/
	//pthread_barrier_wait(&barrier);
	//MPI_Send(&nr_paragraphs_sci_fi, 1, MPI_INT, SCIFI_WORKER, SCIFI_WORKER, MPI_COMM_WORLD);

	//reseteaza fptr la inceputul fisierului
	//fseek(fptr, 0, SEEK_SET);

	while ((result = getline(&line_buffer, &buffer_size, fptr)) != -1) {
		if (strcmp(line_buffer, "science-fiction\n") == 0) {
			//citeste continutul paragrafului si trimite-l catre workerul 1
			while (check_paragraph_end == false) {
				result = getline(&line_buffer, &buffer_size, fptr);
				if (result != -1) {
					if (paragraph == NULL) {
						paragraph = (char *)malloc((strlen(line_buffer) + 1)* sizeof(char));
						strcpy(paragraph, line_buffer);
					}
					else {
						int size = strlen(paragraph) + strlen(line_buffer) + 2;
						char *paragraph_aux = (char *)realloc(paragraph, size);
						if (!paragraph_aux) {
							exit(-1);
						}
						paragraph = paragraph_aux;
						strcat(paragraph, line_buffer);
					}

					if (paragraph[strlen(paragraph) - 2] == '\n') {
						paragraph[strlen(paragraph) - 1] = '\0';
						check_paragraph_end = true;
						//s-a citit un paragraf de tipul scifi
						//printf("[MASTER]Paragraf scifi: %s", paragraph);
					
					//	int buffer_dim = MAX_SIZE + MPI_BSEND_OVERHEAD;
					  //  char *buffer_memory = (char *)malloc(buffer_dim * sizeof(char));
					 //   MPI_Buffer_attach(buffer_memory, buffer_dim);

					//	MPI_Request request = MPI_REQUEST_NULL;

					//	printf("Dimensiune paragraf trimis: %ld\n", strlen(paragraph));
						MPI_Bsend(paragraph, strlen(paragraph) + 1, MPI_CHAR, SCIFI_WORKER, SCIFI_WORKER, MPI_COMM_WORLD);
						//MPI_Buffer_detach(&buffer_memory, &buffer_dim);


						free(paragraph);
						paragraph = NULL;

						//primeste paragraful procesat de la worker
					/*	char *processed_paragraph;
						MPI_Status status;
						int buffer_size;
						MPI_Probe(SCIFI_WORKER, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
								
						MPI_Get_count(&status, MPI_CHAR, &buffer_size);
							
						processed_paragraph = (char *)malloc(buffer_size * sizeof(char));
						MPI_Recv(processed_paragraph, buffer_size, MPI_CHAR, SCIFI_WORKER, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);*/
						//printf("[SCI-FI MASTER]\n%s\n", processed_paragraph);
					}
				}
				else {
					//printf("PARAGRAF scifi:%s", paragraph);

					//int buffer_dim = MAX_SIZE + MPI_BSEND_OVERHEAD;
					//char *buffer_memory = (char *)malloc(buffer_dim * sizeof(char));
					//MPI_Buffer_attach(buffer_memory, buffer_dim);

					//MPI_Request request = MPI_REQUEST_NULL;

				//	printf("Dimensiune paragraf trimis: %ld\n", strlen(paragraph));
					MPI_Bsend(paragraph, strlen(paragraph) + 1, MPI_CHAR, FANTASY_WORKER, FANTASY_WORKER, MPI_COMM_WORLD);
					//MPI_Buffer_detach(&buffer_memory, &buffer_dim);

					free(paragraph);
					paragraph = NULL;

					//primeste paragraful procesat de la worker
				/*	char *processed_paragraph;
					MPI_Status status;
					int buffer_size;
					MPI_Probe(SCIFI_WORKER, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
							
					MPI_Get_count(&status, MPI_CHAR, &buffer_size);
						
					processed_paragraph = (char *)malloc(buffer_size * sizeof(char));
					MPI_Recv(processed_paragraph, buffer_size, MPI_CHAR, SCIFI_WORKER, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);*/
					//printf("[SCI-FI MASTER]\n%s\n", processed_paragraph);

					break;
				}
			}
			if (check_paragraph_end == true) {
				check_paragraph_end = false;
			}
		}
	}
	
	//trimite un mesaj care anunta ca s-au trimis toate paragrafele de tipul horror
	char *message = strdup("DONE");
	//MPI_Request request = MPI_REQUEST_NULL;
	MPI_Bsend(message, strlen(message) + 1, MPI_CHAR, SCIFI_WORKER, SCIFI_WORKER, MPI_COMM_WORLD);
	
	//printf("DONE A FOST TRIMIS DIN MASTER SCIFI\n");
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

	/*int nr_paragraphs = 0;
	while ((result = getline(&line_buffer, &buffer_size, fptr)) != -1) {
		if (strcmp(line_buffer, "fantasy\n") == 0) {
			nr_paragraphs++;
		}
	}*/
	//pthread_barrier_wait(&barrier);
	//MPI_Send(&nr_paragraphs_fantasy, 1, MPI_INT, FANTASY_WORKER, FANTASY_WORKER, MPI_COMM_WORLD);

	//reseteaza fptr la inceputul fisierului
	//fseek(fptr, 0, SEEK_SET);


	while ((result = getline(&line_buffer, &buffer_size, fptr)) != -1) {
		if (strcmp(line_buffer, "fantasy\n") == 0) {
			//citeste continutul paragrafului si trimite-l catre workerul 1
			while (check_paragraph_end == false) {
				result = getline(&line_buffer, &buffer_size, fptr);
				if (result != -1) {
					if (paragraph == NULL) {
						paragraph = (char *)malloc((strlen(line_buffer) + 1)* sizeof(char));
						strcpy(paragraph, line_buffer);
					}
					else {
						int size = strlen(paragraph) + strlen(line_buffer) + 2;
						char *paragraph_aux = (char *)realloc(paragraph, size);
						if (!paragraph_aux) {
							exit(-1);
						}
						paragraph = paragraph_aux;
						strcat(paragraph, line_buffer);
					}

					if (paragraph[strlen(paragraph) - 2] == '\n') {
						paragraph[strlen(paragraph) - 1] = '\0';
						check_paragraph_end = true;

						//printf("PARAGRAF FANTASY: %s\n", paragraph);
						//s-a citit un paragraf de tipul fantasy

					//	int buffer_dim = MAX_SIZE + MPI_BSEND_OVERHEAD;
					//	char *buffer_memory = (char *)malloc(buffer_dim * sizeof(char));
					//	MPI_Buffer_attach(buffer_memory, buffer_dim);

						//MPI_Request request = MPI_REQUEST_NULL;
					//	printf("Dimensiune paragraf trimis: %ld\n", strlen(paragraph));
						MPI_Bsend(paragraph, strlen(paragraph) + 1, MPI_CHAR, FANTASY_WORKER, FANTASY_WORKER, MPI_COMM_WORLD);
						//MPI_Buffer_detach(&buffer_memory, &buffer_dim);

						free(paragraph);
						paragraph = NULL;

						//primeste paragraful procesat de la worker
					/*	char *processed_paragraph;
						MPI_Status status;
						int buffer_size;
						MPI_Probe(FANTASY_WORKER, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
								
						MPI_Get_count(&status, MPI_CHAR, &buffer_size);
							
						processed_paragraph = (char *)malloc(buffer_size * sizeof(char));
						MPI_Recv(processed_paragraph, buffer_size, MPI_CHAR, FANTASY_WORKER, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);*/
						//printf("[FANTASY-MASTER]\n%s\n", processed_paragraph);	
					}
				}
				else {

					//printf("PARAGRAF:%s", paragraph);
				//	int buffer_dim = MAX_SIZE + MPI_BSEND_OVERHEAD;
				//	char *buffer_memory = (char *)malloc(buffer_dim * sizeof(char));
				//	MPI_Buffer_attach(buffer_memory, buffer_dim);

				//	MPI_Request request = MPI_REQUEST_NULL;
				//	printf("Dimensiune paragraf trimis: %ld\n", strlen(paragraph));
					MPI_Bsend(paragraph, strlen(paragraph) + 1, MPI_CHAR, FANTASY_WORKER, FANTASY_WORKER, MPI_COMM_WORLD);

					//MPI_Buffer_detach(&buffer_memory, &buffer_dim);

					free(paragraph);
					paragraph = NULL;

					//primeste paragraful procesat de la worker
				/*	char *processed_paragraph;
					MPI_Status status;
					int buffer_size;
					MPI_Probe(FANTASY_WORKER, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
								
					MPI_Get_count(&status, MPI_CHAR, &buffer_size);
							
					processed_paragraph = (char *)malloc(buffer_size * sizeof(char));
					MPI_Recv(processed_paragraph, buffer_size, MPI_CHAR, FANTASY_WORKER, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);*/
					//printf("[FANTASY-MASTER]\n%s\n", processed_paragraph);

					break;
				}
			}
			if (check_paragraph_end == true) {
				check_paragraph_end = false;
			}
		}
	}

	//trimite un mesaj care anunta ca s-au trimis toate paragrafele de tipul horror
	char *message = strdup("DONE");
	//MPI_Request request = MPI_REQUEST_NULL;
	MPI_Bsend(message, strlen(message) + 1, MPI_CHAR, FANTASY_WORKER, FANTASY_WORKER, MPI_COMM_WORLD);

	//printf("DONE A FOST TRIMIS DIN MASTER FANTASY\n");
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

/*returneaza true daca c este litera*/
bool isLetter(char c) {
	return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
}

bool isVowel(char c) {
	char copy = tolower(c);
	return (copy == 'a' || copy == 'e' || copy == 'i' || copy == 'o' || copy == 'u');
}

/*functia de thread executata de thread-ul de procesare de pe worker-ul horror*/
void *worker_data_processing_horror(void *arg) {
	char *paragraph = (char*) arg;
	char *processed_paragraph;
	int i;
	int k = 0; //numarul de consoane gasite in paragraful initial
	int size = strlen(paragraph);
	for (i = 0; i < size; i++) {

		if (isLetter(paragraph[i]) == true) {
			//verifaca daca litera este consoana
			if (isVowel(paragraph[i]) == false) {
				k++;
			}
		}
	}

	processed_paragraph = (char *)malloc((size + k + 2) * sizeof(char));
	for (i = 0; i <= size; i++) {
		processed_paragraph[strlen(processed_paragraph)] = paragraph[i];
		if (isLetter(paragraph[i]) == true) {
			//verifaca daca litera este consoana
			if (isVowel(paragraph[i]) == false) {
				//aloca un char in plus pentru consoana care va fi introdusa
				processed_paragraph[strlen(processed_paragraph)] = tolower(paragraph[i]);
			}
		}
	}
	
	return processed_paragraph;
}

/*functia de thread executata de thread-ul de procesare de pe worker-ul horror*/
void *worker_data_processing_comedy(void *arg) {
	char *paragraph = (char*) arg;
	char *processed_paragraph = (char *)malloc((strlen(paragraph) + 1) * sizeof(char));

	char *copy = strdup(paragraph);
	char *res = strdup(paragraph);

	char *token = strtok_r(paragraph, " \n", &res);
	bool ok = false;
	while (token != NULL) {
		int i;
		for (i = 1; i < strlen(token); i = i + 2) {
			token[i] = toupper(token[i]);
		}
		strcat(processed_paragraph, token);
		if (copy[token - paragraph + strlen(token)] == ' ') {
			strcat(processed_paragraph, " ");
		}
		else {
			strcat(processed_paragraph, "\n");
		}
		token = strtok_r(NULL, " \n", &res);
	}

	//strcat(processed_paragraph, "\n");
	return processed_paragraph;
}

/*functia de thread executata de thread-ul de procesare de pe worker-ul horror*/
void *worker_data_processing_SciFi(void *arg) {
	//al saptelea cuvant de pe fiecare rand este inversat
	char *paragraph = (char *) arg;
	char *processed_paragraph = (char *)malloc((strlen(paragraph) + 2) * sizeof(char));

	char *rest = paragraph;
	//reentrant version of strtok
	char *line = strtok_r(paragraph, "\n", &rest);
	int count_words = 0;
	
	while (line != NULL) {
		//printf("LINIE PROCESATA:%s\n", line);
		count_words = 1;
		//line va reprezenta o linie din paragraf
		//desparte linia in cuvinte si gaseste al saptelea cuvant de pe linie
		char *line_copy = (char *)malloc((strlen(line) + 2) * sizeof(char));
		strcpy(line_copy, line);
		char *copy = line_copy;
		char *word = strtok_r(line_copy, " ", &copy);
		strcat(processed_paragraph, word);
		strcat(processed_paragraph, " ");
		while (word != NULL) {
			word = strtok_r(NULL, " ", &copy);
			count_words++;
			if (word != NULL) {
				if (count_words != 2) {
					strcat(processed_paragraph, " ");
				}
				if (count_words % 7 == 0) {
					int i, length = strlen(word) + 1;
					char *reverse_word = (char *)malloc((length + 1)* sizeof(char));

					int end = strlen(word) - 1;
					int begin;
   					for (begin = 0; begin < length; begin++) {
      					reverse_word[begin] = word[end];
      					end--;
   					}

   					reverse_word[begin] = '\0';
					strcat(processed_paragraph, reverse_word);
				}
				else {
					strcat(processed_paragraph, word);
				}
			}
		}
		line = strtok_r(NULL, "\n", &rest);
		if (line != NULL) {
			strcat(processed_paragraph, "\n");
		}
	}
	strcat(processed_paragraph, "\n");


	//void *result = processed_paragraph;
	//printf("PARAGRAF PROCESAT: %s", processed_paragraph);
	return processed_paragraph;
}

/*functia de thread executata de thread-ul de procesare de pe worker-ul fantasy*/
void *worker_data_processing_fantasy(void *arg) {
	char *paragraph = (char *) arg;
	//printf("CHUNCK PRIMIT: %s\n", paragraph);
	
	//printf("DIMENIUNEA CHUNKULUI PRIMIT ESTE: %ld\n", strlen(paragraph));
	int size = strlen(paragraph) + 1;
	//printf("DIMENSIUNE CHUNK PRIMIT: %ld\n", strlen(paragraph));
	char *processed_paragraph = (char *)malloc((strlen(paragraph) + 1) * sizeof(char));

	char *paragraph_copy = (char *)malloc(size * sizeof(char));
	strcpy(paragraph_copy, paragraph);
	//printf("COPIE CHUNK: %s\n", paragraph_copy);
	//prima litera a fiecarui cuvant trebuie facuta majuscula
	char *copy = (char *)malloc(size * sizeof(char));
	char *token_fan = strtok_r(paragraph_copy, " \n", &copy);
	bool ok = false;
	int position_fan = 0;
	while (token_fan != NULL) {
		char first = toupper(token_fan[0]);
		token_fan[0] = first;
		position_fan += strlen(token_fan);
		int size = strlen(processed_paragraph);
	
		strcat(processed_paragraph, token_fan);

		if (paragraph[position_fan] == '\n') {
			strcat(processed_paragraph, "\n");
		}
		else if(paragraph[position_fan] == ' ') {
			strcat(processed_paragraph, " ");
		}
		position_fan++;
		token_fan = strtok_r(NULL, " \n", &copy);
	}

//	printf("CHUNCK PROCESAT FANTASY: %s\n", processed_paragraph);
	return processed_paragraph;
}


bool check_thread_start(int r, long worker_rank) {
	if (r != 0) {
    	printf("Eroare la crearea thread-ului de procesare in worker-ul %ld\n", worker_rank);
    	return false;
    }
    return true;
}


char *extract_and_process_chuncks(char *paragraph, int worker, int lines_count) {
	int P = sysconf(_SC_NPROCESSORS_CONF); //numarul de threaduri disponibile
	//printf("Se vor folosit mai multe threaduri pentru procesare\n");
	//se pornesc maxim P-1 threaduri pentru procesare(o data sau de mai multe ori in functie de numarul de linii)
	//printf("NUMARUL DE LINII: %d\n", lines_count);
	//int threads_num = lines_count / NR_LINES + ceil((lines_count % NR_LINES) / (float)NR_LINES); //numarul de thread-uri de care ar fi nevoie pt a imparti procesarea liniilor
	//printf("numarul de threaduri de care avem nevoie este: %d\n", threads_num);
	//printf("numar disponibile: %d\n", P - 1);

	//trebuie sa folosim un thread de mai multe ori pentru a procesa paragraful
	
	char *final_paragraph = malloc(1);//va retine paragraful final procesat
	*final_paragraph = '\0';
	pthread_t processing_threads[P - 1]; //fiecare thread va procesa initial maxim 20 de linii

	char *paragraph_chunk = malloc(1);
	*paragraph_chunk = '\0';
	char *line = strtok(paragraph, "\n");
	int current_line = 0;
	int thread_index = 0;
	char *next_line = malloc(1);
	*next_line = '\0';

	int threads_num = 0;
	int chunck_lines = 0;
	int last_lines = 0; //numarul de linii procesate de un nr multiplu de P - 1 threaduri anterior
				
	int start_line;
	int end_line;			
	while(1) {
		while (line != NULL) {
			start_line = NR_LINES * thread_index + last_lines;
			end_line = (thread_index + 1) * NR_LINES - 1 + last_lines;
			if (current_line >= start_line && current_line <= end_line) {
				char *complete_line = (char *)malloc(strlen(line) + 2);
				sprintf(complete_line, "%s\n", line);
				int new_size = strlen(complete_line) + 2 + strlen(paragraph_chunk);
				char *aux = (char *)realloc(paragraph_chunk, new_size);
				if (!aux) {
					exit(-1);
				}
				paragraph_chunk = aux;
				strcat(paragraph_chunk, complete_line);
						chunck_lines++;
				}
				else {
						//linia curenta va face parte din urmatorul chunck
						break;
				}
				line = strtok(NULL, "\n");
				current_line++;
				}
				if (thread_index <= P - 2) {
					//porneste cele P - 1 threaduri de procesare
					//printf("START_LINE: %d\n", start_line);
					//printf("END_LINE: %d\n", end_line);
					//printf("THREAD INDEX: %d\n", thread_index);
					//printf("chunk trimis catre procesare: %s\n", paragraph_chunk);
					if (threads_num + 1 <= P - 1) {
						threads_num++;
					}
				//printf("INDEX THREAD: %d\n", thread_index);
				//printf("CHUNCK TRIMIS CATRE PROCESARE: %s\n", paragraph_chunk);
					int r = 0;
					if (worker == HORROR_WORKER) {
						char *chunck_horror = (char *)malloc(strlen(paragraph_chunk) + 1);
						strcpy(chunck_horror, paragraph_chunk);
						r = pthread_create(&processing_threads[thread_index], NULL, worker_data_processing_horror, (void *)chunck_horror);
					}
					else if (worker == COMEDY_WORKER) {
						char *chunck_comedy = (char *)malloc(strlen(paragraph_chunk) + 1);
						strcpy(chunck_comedy, paragraph_chunk);
						r = pthread_create(&processing_threads[thread_index], NULL, worker_data_processing_comedy, (void *)chunck_comedy);
					}
					else if (worker == FANTASY_WORKER) {
						char *chunck_fantasy = (char *)malloc(strlen(paragraph_chunk) + 1);
						strcpy(chunck_fantasy, paragraph_chunk);
						r = pthread_create(&processing_threads[thread_index], NULL, worker_data_processing_fantasy, (void *)chunck_fantasy);
					}
					else if (worker == SCIFI_WORKER) {
						char *chunck_scifi = (char *)malloc(strlen(paragraph_chunk) + 1);
						strcpy(chunck_scifi, paragraph_chunk);
						r = pthread_create(&processing_threads[thread_index], NULL, worker_data_processing_SciFi, (void *)chunck_scifi);
					}
					
					if (check_thread_start(r, worker) == false) {
						printf("Eroare la pornire thread\n");
						exit(-1);
					}
					thread_index++;
					//printf("NUMARUL DE THREADURI PORNITE: %d\n", thread_index);
				}
				//se face join pe threadurile de procesare anterior pornite, si se porneste un nou thread 0
				if (thread_index > P - 2) {
					//printf("SE AJUNGE LA JOIN PENTRU CELE %d THREADURI DE PROCESARE\n", threads_num);
					int i;
					for (i = 0; i < threads_num; i++) {
						char *processed_paragraph = NULL;
					    int r = pthread_join(processing_threads[i], (void**)&processed_paragraph);
					    if (r) {
					        printf("Eroare la asteptarea thread-ului %d\n", i);
					        exit(-1);
					    }

					    char *aux = (char *)realloc(final_paragraph, strlen(processed_paragraph) + 1 + strlen(final_paragraph));
					        if (!aux) {
					        	exit(-1);
					    }
					    final_paragraph = aux;
					    strcat(final_paragraph, processed_paragraph);
			   		}   
			   		 	
			   		last_lines += chunck_lines;
			   		chunck_lines = 0;
			   		threads_num = 1;

					thread_index = 0;
				}

				free(paragraph_chunk);
				paragraph_chunk = malloc(1);
				*paragraph_chunk = '\0';	

				if (line == NULL) {
					if (threads_num != 0) {
						int i;
						for (i = 0; i < threads_num - 1; i++) {
						char *processed_paragraph = NULL;
					    int r = pthread_join(processing_threads[i], (void**)&processed_paragraph);
					    if (r) {
					        printf("Eroare la asteptarea thread-ului %d\n", i);
					        exit(-1);
					    }

					    char *aux = (char *)realloc(final_paragraph, strlen(processed_paragraph) + 1 + strlen(final_paragraph));
					    if (!aux) {
					        	exit(-1);
					    }
					    final_paragraph = aux;
					    strcat(final_paragraph, processed_paragraph);
			   		}   
				}
				break;
			}	
	}			
	
	return final_paragraph;
}


/*functia de thread executata de thread-ul reader din worker-ul horror*/
void *worker_horror_reader_f(void *arg) {
	/*in thread-ul receiver de pe fiecare worker este primit mai intai de la master numarul de paragrafe 
	de tipul asociat lui pe care urmeaza sa le primeasca de la master*/

	//int num_paragraphs;
	//MPI_Recv(&num_paragraphs, 1, MPI_INT, MASTER, HORROR_WORKER, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	//printf("[HORROR] Numar de paragrafe asteptate in worker: %d\n", num_paragraphs);
	char **paragraphs = NULL;
	int index = 0;
	int nr = 0;
	while(1) {
	//for (index = 0; index < num_paragraphs; index++) {
		//analizeaza numarul de linii din fiecare paragraf si porneste threadurile care fac procesarea paragrafului curent i
		//face receive la paragraful care i-a fost trimis de catre thread-ul aferent tipului sau din master
		char *paragraph;
		MPI_Status status;
		int buffer_size;
		MPI_Probe(MASTER, HORROR_WORKER, MPI_COMM_WORLD, &status);
			
		MPI_Get_count(&status, MPI_CHAR, &buffer_size);
		
		MPI_Status status_recv;
		paragraph = (char *)malloc(buffer_size * sizeof(char));

		MPI_Request request = MPI_REQUEST_NULL;
		MPI_Recv(paragraph, buffer_size, MPI_CHAR, MASTER, HORROR_WORKER, MPI_COMM_WORLD, &status_recv);
			
		if (strcmp(paragraph, "DONE") == 0) {
			//printf("A ajuns done in horror\n");
			break;
		}
		if (nr == 0) {
			paragraphs = (char **)malloc(sizeof(char *));
			paragraphs[0] = (char *)malloc(buffer_size * sizeof(char));
			strcpy(paragraphs[0], paragraph);
			printf("first value: %s\n", paragraphs[0]);
		}
		else {

			char **aux = (char **)realloc(paragraphs, sizeof(char *) * (nr + 1));
			if (!aux) {
				exit(-1);
			}
			paragraphs = aux;
			paragraphs[nr] = (char *)malloc(buffer_size * sizeof(char));
			strcpy(paragraphs[nr], paragraph);
			printf("second value...: %s\n", paragraphs[nr]);
		}
		nr++;
		free(paragraph);
		paragraph = NULL;
	}
		
	for (index = 0; index < nr; index++) {
		char *paragraph = (char *)malloc(strlen(paragraphs[index]) + 1);


		strcpy(paragraph, paragraphs[index]);
		printf("text procesat: %s\n", paragraph);
		int lines_count = get_num_lines(paragraph);
		//printf("Paragraful care ajunge la worker reader: %s\n", paragraph);
		//daca paragraful are mai putin de 20 de linii, se porneste un singur thread de procesare
		if (lines_count <= NR_LINES) {
			int r;
			pthread_t processing_thread;
			r = pthread_create(&processing_thread, NULL, worker_data_processing_horror, (void *)paragraph);
			if (check_thread_start(r, HORROR_WORKER) == false) {
				exit(-1);
			}
			char *processed_paragraph_horror = NULL;
		   	r = pthread_join(processing_thread, (void**)&processed_paragraph_horror);
		    if (r != 0) {
		    	printf("Eroare la asteptarea thread-ului de procesare in worker-ul %d\n", HORROR_WORKER);
		    	exit(-1);
		    }
		   
		    
		    //trimite paragraful procesat catre master
		//    int buffer_dim = MAX_SIZE + MPI_BSEND_OVERHEAD;
		//    char *buffer_memory = (char *)malloc(buffer_dim * sizeof(char));
		//    MPI_Buffer_attach(buffer_memory, buffer_dim);

		    MPI_Bsend(processed_paragraph_horror, strlen(processed_paragraph_horror) + 1, MPI_CHAR, MASTER, index, MPI_COMM_WORLD);	

		    printf("Textul procesat in worker-ul HORROR este:%s\n", (char*)processed_paragraph_horror);
		 	//MPI_Buffer_detach(&buffer_memory, &buffer_dim);
		 	//free(paragraph);
		 	//paragraph = NULL;
		 	//index++;
		}
		else {
			int P = sysconf(_SC_NPROCESSORS_CONF); //numarul de threaduri disponibile
			//printf("Se vor folosit mai multe threaduri pentru procesare\n");
			//se pornesc maxim P-1 threaduri pentru procesare(o data sau de mai multe ori in functie de numarul de linii)
			//printf("NUMARUL DE LINII: %d\n", lines_count);
			int threads_num = lines_count / NR_LINES + ceil((lines_count % NR_LINES) / (float)NR_LINES); //numarul de thread-uri de care ar fi nevoie pt a imparti procesarea liniilor
			//printf("numarul de threaduri de care avem nevoie este: %d\n", threads_num);
			//printf("numar disponibile: %d\n", P - 1);

			//trebuie sa folosim un thread de mai multe ori pentru a procesa paragraful
			
			if (threads_num > P - 1) {
				char *final_paragraph = extract_and_process_chuncks(paragraph, HORROR_WORKER, lines_count);

				//trimite paragraful final procesat catre MASTER
				//printf("!!!Paragaraful final procesat: %s\n", final_paragraph);
				

			    MPI_Bsend(final_paragraph, strlen(final_paragraph) + 1, MPI_CHAR, MASTER, index, MPI_COMM_WORLD);
			   // index++;
			}
			//threadurile disponibile sunt suficiente pentru a procesa paragraful pornindu-le o singura data
			else if (threads_num <= P - 1) {
				
				pthread_t processing_threads[threads_num]; //fiecare thread va procesa maxim 20 de linii

				char *line = strtok(paragraph, "\n");
				int current_line = 0;
				char *next_line = malloc(1);
				*next_line = '\0';

				//obtine chunck-ul din paragraf format din liniile care vor fi procesate de fiecare thread i
				int i;
				for (i = 0; i < threads_num; i++) {
					char *paragraph_chunk = malloc(1);
					*paragraph_chunk = '\0';
					int start_line = NR_LINES * i;
					int end_line;
					if (lines_count < NR_LINES * (i + 1) - 1) { //unul din threaduri va primi mai putin de 20 de linii
						end_line = lines_count;
					}
					else {
						end_line = NR_LINES * (i + 1) - 1;
					}
					if (strlen(next_line) != 0) {
						char *complete_line = (char *)malloc(strlen(next_line) + 2);
						sprintf(complete_line, "%s\n", next_line);
						int new_size = strlen(complete_line) + 1 + strlen(paragraph_chunk);
						char *aux = (char *)realloc(paragraph_chunk, new_size);
						if (!aux) {
							exit(-1);
						}
						paragraph_chunk = aux;
						strcat(paragraph_chunk, complete_line);
					}
					while (line != NULL) {
						if (current_line >= start_line && current_line <= end_line) {
							char *complete_line = (char *)malloc(strlen(line) + 2);
							sprintf(complete_line, "%s\n", line);
							int new_size = strlen(complete_line) + 1 + strlen(paragraph_chunk);
							char *aux = (char *)realloc(paragraph_chunk, new_size);
							if (!aux) {
								exit(-1);
							}
							paragraph_chunk = aux;
							strcat(paragraph_chunk, complete_line);
						}
						else {
							//linia curenta va face parte din urmatorul chunk
							char *aux = (char *)realloc(next_line, strlen(line) + 1);
							if (!aux) {
								exit(-1);
							}
							strcpy(aux, line);
							next_line = aux;
							current_line++;
							break;
						}
						line = strtok(NULL, "\n");
						current_line++;
					}

					int r = pthread_create(&processing_threads[i], NULL, worker_data_processing_horror, (void *)paragraph_chunk);
					if (check_thread_start(r, HORROR_WORKER) == false) {
						exit(-1);
					}
					if (line != NULL) {
						line = strtok(NULL, "\n");
					}
				}

				//paragraful final procesat care va fi trimis catre master
				char *final_paragraph = malloc(1);
				*final_paragraph = '\0';
				for (i = 0; i < threads_num; i++) {
					char *processed_paragraph_horror = NULL;
			        int r = pthread_join(processing_threads[i], (void**)&processed_paragraph_horror);
			        if (r) {
			            printf("Eroare la asteptarea thread-ului %d\n", i);
			            exit(-1);
			        }
			       // printf("Chunck procesat: %s\n", processed_paragraph_horror);
			        char *aux = (char *)realloc(final_paragraph, strlen(processed_paragraph_horror) + 1 + strlen(final_paragraph));
			        if (!aux) {
			        	exit(-1);
			        }
			        final_paragraph = aux;
			        strcat(final_paragraph, processed_paragraph_horror);
	   		 	}   
		   	//	int buffer_dim = MAX_SIZE + MPI_BSEND_OVERHEAD;
			//    char *buffer_memory = (char *)malloc(buffer_dim * sizeof(char));
			 //   MPI_Buffer_attach(buffer_memory, buffer_dim);

			    	MPI_Bsend(final_paragraph, strlen(final_paragraph) + 1, MPI_CHAR, MASTER, index, MPI_COMM_WORLD);

			   // printf("!!!!!!!!!!!!!!!!!!!!Paragaraful final procesat: %s\n", final_paragraph);
			   // free(paragraph);
		 		//paragraph = NULL;
		 		//index++;
			}
		}
	}

	//printf("S-au procesat si trimis toate paragrafele horror\n");
	pthread_exit(NULL);
}

/*functia de thread executata de thread-ul reader din worker-ul comedy*/
void *worker_comedy_reader_f(void *arg) {
	
	/*in thread-ul receiver de pe fiecare worker este primit mai intai de la master numarul de paragrafe 
	de tipul asociat lui pe care urmeaza sa le primeasca de la master*/
	//int num_paragraphs;
	//MPI_Recv(&num_paragraphs, 1, MPI_INT, MASTER, COMEDY_WORKER, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	//printf("[COMEDY] Numar de paragrafe asteptate in worker: %d\n", num_paragraphs);

	int index = 0;
	while(1) {
	//for (index = 0; index < num_paragraphs; index++) {
		//analizeaza numarul de linii din fiecare paragraf si porneste threadurile care fac procesarea paragrafului curent i
				//face receive la paragraful care i-a fost trimis de catre thread-ul aferent tipului sau din master
		char *paragraph;
		MPI_Status status;
		int buffer_size;

		MPI_Probe(MASTER, COMEDY_WORKER, MPI_COMM_WORLD, &status);
			
		MPI_Get_count(&status, MPI_CHAR, &buffer_size);
		
		MPI_Status status_recv;
		paragraph = (char *)malloc(buffer_size * sizeof(char));
		MPI_Request request = MPI_REQUEST_NULL;
		MPI_Recv(paragraph, buffer_size, MPI_CHAR, MASTER, COMEDY_WORKER, MPI_COMM_WORLD, &status_recv);
	

		//printf("Paragraful care ajunge la worker coemdy: %s\n", paragraph);
		if (strlen(paragraph) == 4) {
			if (strcmp(paragraph, "DONE") == 0) {
			//printf("A ajuns done in comedy\n");
				break;
			}
		}
		

		int lines_count = get_num_lines(paragraph);
		//printf("Mesajul primit in worker-ul %ld este: %s; Are %d linii\n", worker_rank, paragraph, lines_count);
		//numara cate linii contine paragraful primit

		//porneste atatea thread-uri de cat este nevoie pt a procesa paragraful in functie de numarul de linii din paragraf

		//valoarea intoarsa la join reprezinta paragraful procesat conform cu tipul acestuia
		//ulterior paragraful procesat conform cu tipul acestuia este trimis catre Master
		//daca paragraful are mai putin de 20 de linii, se porneste un singur thread de procesare
		if (lines_count <= NR_LINES) {
			int r;
			pthread_t processing_thread;
			r = pthread_create(&processing_thread, NULL, worker_data_processing_comedy, (void *)paragraph);
			if (check_thread_start(r, HORROR_WORKER) == false) {
				exit(-1);
			}
			char *processed_paragraph_horror = NULL;
		   	r = pthread_join(processing_thread, (void**)&processed_paragraph_horror);
		    if (r != 0) {
		    	printf("Eroare la asteptarea thread-ului de procesare in worker-ul %d\n", COMEDY_WORKER);
		    	exit(-1);
		    }
		   
		    
		    //trimite paragraful procesat catre master
		 //   int buffer_dim = MAX_SIZE + MPI_BSEND_OVERHEAD;
		 //   char *buffer_memory = (char *)malloc(buffer_dim * sizeof(char));
		//    MPI_Buffer_attach(buffer_memory, buffer_dim);

		    MPI_Bsend(processed_paragraph_horror, strlen(processed_paragraph_horror) + 1, MPI_CHAR, MASTER, index, MPI_COMM_WORLD);	
		    index++;
		    //printf("Textul procesat in worker-ul COMEDY este:%s\n", (char*)processed_paragraph_horror);
		 	//MPI_Buffer_detach(&buffer_memory, &buffer_dim);
		 	free(paragraph);
		 	paragraph = NULL;
		}
		else {
			int P = sysconf(_SC_NPROCESSORS_CONF); //numarul de threaduri disponibile
			//printf("Se vor folosit mai multe threaduri pentru procesare\n");
			//se pornesc maxim P-1 threaduri pentru procesare(o data sau de mai multe ori in functie de numarul de linii)
			//printf("NUMARUL DE LINII: %d\n", lines_count);
			int threads_num = lines_count / NR_LINES + ceil((lines_count % NR_LINES) / (float)NR_LINES); //numarul de thread-uri de care ar fi nevoie pt a imparti procesarea liniilor
			//printf("numarul de threaduri de care avem nevoie este: %d\n", threads_num);
			//printf("numar disponibile: %d\n", P - 1);

			//trebuie sa folosim un thread de mai multe ori pentru a procesa paragraful
			
			if (threads_num > P - 1) {
				char *final_paragraph = extract_and_process_chuncks(paragraph, COMEDY_WORKER, lines_count);

				//trimite paragraful final procesat catre MASTER
				//printf("!!!Paragaraful final procesat: %s\n", final_paragraph);
			//	int buffer_dim = MAX_SIZE + MPI_BSEND_OVERHEAD;
			 //   char *buffer_memory = (char *)malloc(buffer_dim * sizeof(char));
			//    MPI_Buffer_attach(buffer_memory, buffer_dim);

			    MPI_Bsend(final_paragraph, strlen(final_paragraph) + 1, MPI_CHAR, MASTER, index, MPI_COMM_WORLD);
			    index++;
			}
			//threadurile disponibile sunt suficiente pentru a procesa paragraful pornindu-le o singura data
			else if (threads_num <= P - 1) {
				
				pthread_t processing_threads[threads_num]; //fiecare thread va procesa maxim 20 de linii

				char *line = strtok(paragraph, "\n");
				int current_line = 0;
				char *next_line = malloc(1);
				*next_line = '\0';

				//obtine chunck-ul din paragraf format din liniile care vor fi procesate de fiecare thread i
				int i;
				for (i = 0; i < threads_num; i++) {
					char *paragraph_chunk = malloc(1);
					*paragraph_chunk = '\0';
					int start_line = NR_LINES * i;
					int end_line;
					if (lines_count < NR_LINES * (i + 1) - 1) { //unul din threaduri va primi mai putin de 20 de linii
						end_line = lines_count;
					}
					else {
						end_line = NR_LINES * (i + 1) - 1;
					}
					if (strlen(next_line) != 0) {
						char *complete_line = (char *)malloc(strlen(next_line) + 2);
						sprintf(complete_line, "%s\n", next_line);
						int new_size = strlen(complete_line) + 1 + strlen(paragraph_chunk);
						char *aux = (char *)realloc(paragraph_chunk, new_size);
						if (!aux) {
							exit(-1);
						}
						paragraph_chunk = aux;
						strcat(paragraph_chunk, complete_line);
					}
					while (line != NULL) {
						if (current_line >= start_line && current_line <= end_line) {
							char *complete_line = (char *)malloc(strlen(line) + 2);
							sprintf(complete_line, "%s\n", line);
							int new_size = strlen(complete_line) + 1 + strlen(paragraph_chunk);
							char *aux = (char *)realloc(paragraph_chunk, new_size);
							if (!aux) {
								exit(-1);
							}
							paragraph_chunk = aux;
							strcat(paragraph_chunk, complete_line);
						}
						else {
							//linia curenta va face parte din urmatorul chunk
							char *aux = (char *)realloc(next_line, strlen(line) + 1);
							if (!aux) {
								exit(-1);
							}
							strcpy(aux, line);
							next_line = aux;
							current_line++;
							break;
						}
						line = strtok(NULL, "\n");
						current_line++;
					}

					int r = pthread_create(&processing_threads[i], NULL, worker_data_processing_comedy, (void *)paragraph_chunk);
					if (check_thread_start(r, HORROR_WORKER) == false) {
						exit(-1);
					}
					if (line != NULL) {
						line = strtok(NULL, "\n");
					}
				}

				//paragraful final procesat care va fi trimis catre master
				char *final_paragraph = malloc(1);
				*final_paragraph = '\0';
				for (i = 0; i < threads_num; i++) {
					char *processed_paragraph_horror = NULL;
			        int r = pthread_join(processing_threads[i], (void**)&processed_paragraph_horror);
			        if (r) {
			            printf("Eroare la asteptarea thread-ului %d\n", i);
			            exit(-1);
			        }
			       // printf("Chunck procesat: %s\n", processed_paragraph_horror);
			        char *aux = (char *)realloc(final_paragraph, strlen(processed_paragraph_horror) + 1 + strlen(final_paragraph));
			        if (!aux) {
			        	exit(-1);
			        }
			        final_paragraph = aux;
			        strcat(final_paragraph, processed_paragraph_horror);
	   		 	}   
		   	//	int buffer_dim = MAX_SIZE + MPI_BSEND_OVERHEAD;
			//    char *buffer_memory = (char *)malloc(buffer_dim * sizeof(char));
			//    MPI_Buffer_attach(buffer_memory, buffer_dim);

			    MPI_Bsend(final_paragraph, strlen(final_paragraph) + 1, MPI_CHAR, MASTER, index, MPI_COMM_WORLD);
			    index++;
			   // printf("!!!!!!!!!!!!!!!!!!!!Paragaraful final procesat comedy: %s\n", final_paragraph);
			    free(paragraph);
		 		paragraph = NULL;
			}
		}
	}

	//printf("S-au procesat si trimis toate paragrafele horror\n");
	pthread_exit(NULL);
}


/*functia de thread executata de thread-ul reader din worker-ul SCI-FI*/
void *worker_sciFi_reader_f(void *arg) {
	
	/*in thread-ul receiver de pe fiecare worker este primit mai intai de la master numarul de paragrafe 
	de tipul asociat lui pe care urmeaza sa le primeasca de la master*/
	//int num_paragraphs;
	//MPI_Recv(&num_paragraphs, 1, MPI_INT, MASTER, SCIFI_WORKER, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	//printf("[SCIFI] Numar de paragrafe asteptate in worker: %d\n", num_paragraphs);

	int index = 0;
	while(1) {
	//for (index = 0; index < num_paragraphs; index++) {
		//analizeaza numarul de linii din fiecare paragraf si porneste threadurile care fac procesarea paragrafului curent i
				//face receive la paragraful care i-a fost trimis de catre thread-ul aferent tipului sau din master
		char *paragraph;
		MPI_Status status;
		int buffer_size;

		MPI_Probe(MASTER, SCIFI_WORKER, MPI_COMM_WORLD, &status);
			
		MPI_Get_count(&status, MPI_CHAR, &buffer_size);
		
		MPI_Status status_recv;
		paragraph = (char *)malloc(buffer_size * sizeof(char));
		MPI_Request request = MPI_REQUEST_NULL;
		MPI_Recv(paragraph, buffer_size, MPI_CHAR, MASTER, SCIFI_WORKER, MPI_COMM_WORLD, &status_recv);

		//printf("Paragraful care ajunge la worker scifi: %s\n", paragraph);
		if (strcmp(paragraph, "DONE") == 0) {
			//printf("A ajuns done in scifi\n");
			break;
		}
		int lines_count = get_num_lines(paragraph);

		//daca paragraful are mai putin de 20 de linii, se porneste un singur thread de procesare
		if (lines_count <= NR_LINES) {
			int r;
			pthread_t processing_thread;
			r = pthread_create(&processing_thread, NULL, worker_data_processing_SciFi, (void *)paragraph);
			if (check_thread_start(r, HORROR_WORKER) == false) {
				exit(-1);
			}
			char *processed_paragraph_horror = NULL;
		   	r = pthread_join(processing_thread, (void**)&processed_paragraph_horror);
		    if (r != 0) {
		    	printf("Eroare la asteptarea thread-ului de procesare in worker-ul %d\n", SCIFI_WORKER);
		    	exit(-1);
		    }
		   
		    
		    //trimite paragraful procesat catre master
		//    int buffer_dim = MAX_SIZE + MPI_BSEND_OVERHEAD;
		//    char *buffer_memory = (char *)malloc(buffer_dim * sizeof(char));
		//    MPI_Buffer_attach(buffer_memory, buffer_dim);

		    MPI_Bsend(processed_paragraph_horror, strlen(processed_paragraph_horror) + 1, MPI_CHAR, MASTER, index, MPI_COMM_WORLD);	
		    index++;
		    //printf("Textul procesat in worker-ul HORROR este:%s\n", (char*)processed_paragraph_horror);
		 	//MPI_Buffer_detach(&buffer_memory, &buffer_dim);
		 	free(paragraph);
		 	paragraph = NULL;
		}
		else {
			int P = sysconf(_SC_NPROCESSORS_CONF); //numarul de threaduri disponibile
			//printf("Se vor folosit mai multe threaduri pentru procesare\n");
			//se pornesc maxim P-1 threaduri pentru procesare(o data sau de mai multe ori in functie de numarul de linii)
			//printf("NUMARUL DE LINII: %d\n", lines_count);
			int threads_num = lines_count / NR_LINES + ceil((lines_count % NR_LINES) / (float)NR_LINES); //numarul de thread-uri de care ar fi nevoie pt a imparti procesarea liniilor
			//printf("numarul de threaduri de care avem nevoie este: %d\n", threads_num);
			//printf("numar disponibile: %d\n", P - 1);

			//trebuie sa folosim un thread de mai multe ori pentru a procesa paragraful
			
			if (threads_num > P - 1) {
				char *final_paragraph = extract_and_process_chuncks(paragraph, SCIFI_WORKER, lines_count);

				//trimite paragraful final procesat catre MASTER
				//printf("!!!Paragaraful final procesat: %s\n", final_paragraph);
			//	int buffer_dim = MAX_SIZE + MPI_BSEND_OVERHEAD;
			 //   char *buffer_memory = (char *)malloc(buffer_dim * sizeof(char));
			//    MPI_Buffer_attach(buffer_memory, buffer_dim);

			    MPI_Bsend(final_paragraph, strlen(final_paragraph) + 1, MPI_CHAR, MASTER, index, MPI_COMM_WORLD);
			    index++;
			}
			//threadurile disponibile sunt suficiente pentru a procesa paragraful pornindu-le o singura data
			else if (threads_num <= P - 1) {
				
				pthread_t processing_threads[threads_num]; //fiecare thread va procesa maxim 20 de linii

				char *line = strtok(paragraph, "\n");
				int current_line = 0;
				char *next_line = malloc(1);
				*next_line = '\0';

				//obtine chunck-ul din paragraf format din liniile care vor fi procesate de fiecare thread i
				int i;
				for (i = 0; i < threads_num; i++) {
					char *paragraph_chunk = malloc(1);
					*paragraph_chunk = '\0';
					int start_line = NR_LINES * i;
					int end_line;
					if (lines_count < NR_LINES * (i + 1) - 1) { //unul din threaduri va primi mai putin de 20 de linii
						end_line = lines_count;
					}
					else {
						end_line = NR_LINES * (i + 1) - 1;
					}
					if (strlen(next_line) != 0) {
						char *complete_line = (char *)malloc(strlen(next_line) + 2);
						sprintf(complete_line, "%s\n", next_line);
						int new_size = strlen(complete_line) + 1 + strlen(paragraph_chunk);
						char *aux = (char *)realloc(paragraph_chunk, new_size);
						if (!aux) {
							exit(-1);
						}
						paragraph_chunk = aux;
						strcat(paragraph_chunk, complete_line);
					}
					while (line != NULL) {
						if (current_line >= start_line && current_line <= end_line) {
							char *complete_line = (char *)malloc(strlen(line) + 2);
							sprintf(complete_line, "%s\n", line);
							int new_size = strlen(complete_line) + 1 + strlen(paragraph_chunk);
							char *aux = (char *)realloc(paragraph_chunk, new_size);
							if (!aux) {
								exit(-1);
							}
							paragraph_chunk = aux;
							strcat(paragraph_chunk, complete_line);
						}
						else {
							//linia curenta va face parte din urmatorul chunk
							char *aux = (char *)realloc(next_line, strlen(line) + 1);
							if (!aux) {
								exit(-1);
							}
							strcpy(aux, line);
							next_line = aux;
							current_line++;
							break;
						}
						line = strtok(NULL, "\n");
						current_line++;
					}

					int r = pthread_create(&processing_threads[i], NULL, worker_data_processing_SciFi, (void *)paragraph_chunk);
					if (check_thread_start(r, HORROR_WORKER) == false) {
						exit(-1);
					}
					if (line != NULL) {
						line = strtok(NULL, "\n");
					}
				}

				//paragraful final procesat care va fi trimis catre master
				char *final_paragraph = malloc(1);
				*final_paragraph = '\0';
				for (i = 0; i < threads_num; i++) {
					char *processed_paragraph_horror = NULL;
			        int r = pthread_join(processing_threads[i], (void**)&processed_paragraph_horror);
			        if (r) {
			            printf("Eroare la asteptarea thread-ului %d\n", i);
			            exit(-1);
			        }
			       // printf("Chunck procesat: %s\n", processed_paragraph_horror);
			        char *aux = (char *)realloc(final_paragraph, strlen(processed_paragraph_horror) + 1 + strlen(final_paragraph));
			        if (!aux) {
			        	exit(-1);
			        }
			        final_paragraph = aux;
			        strcat(final_paragraph, processed_paragraph_horror);
	   		 	}   
		   	//	int buffer_dim = MAX_SIZE + MPI_BSEND_OVERHEAD;
			//    char *buffer_memory = (char *)malloc(buffer_dim * sizeof(char));
			//    MPI_Buffer_attach(buffer_memory, buffer_dim);

			    MPI_Bsend(final_paragraph, strlen(final_paragraph) + 1, MPI_CHAR, MASTER, index, MPI_COMM_WORLD);
			    index++;
			    //printf("!!!!!!!!!!!!!!!!!!!!Paragaraful final procesat: %s\n", final_paragraph);
			    free(paragraph);
		 		paragraph = NULL;
			}
		}
	}

	//printf("S-au procesat si trimis toate paragrafele horror\n");
	pthread_exit(NULL);
}


/*functia de thread executata de thread-ul reader din worker-ul fantasy*/
void *worker_fantasy_reader_f(void *arg) {
	
	/*in thread-ul receiver de pe fiecare worker este primit mai intai de la master numarul de paragrafe 
	de tipul asociat lui pe care urmeaza sa le primeasca de la master*/
	//int num_paragraphs;
	//MPI_Recv(&num_paragraphs, 1, MPI_INT, MASTER, FANTASY_WORKER, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	//printf("[FANTASY] Numar de paragrafe asteptate in worker: %d\n", num_paragraphs);

	int index = 0;
	while(1) {
	//for (index = 0; index < num_paragraphs; index++) {
		//analizeaza numarul de linii din fiecare paragraf si porneste threadurile care fac procesarea paragrafului curent i
		//face receive la paragraful care i-a fost trimis de catre thread-ul aferent tipului sau din master
		char *paragraph;
		MPI_Status status;
		int buffer_size;

		MPI_Probe(MASTER, FANTASY_WORKER, MPI_COMM_WORLD, &status);
			
		MPI_Get_count(&status, MPI_CHAR, &buffer_size);

		MPI_Status status_recv;
		paragraph = (char *)malloc(buffer_size * sizeof(char));

		MPI_Request request = MPI_REQUEST_NULL;
		MPI_Recv(paragraph, buffer_size, MPI_CHAR, MASTER, FANTASY_WORKER, MPI_COMM_WORLD, &status_recv);
		
		
		if (strcmp(paragraph, "DONE") == 0) {
			//printf("A ajuns done in fantasy\n");
			break;
		}
		//printf("Paragraful care ajunge la worker fantasy: %s\n", paragraph);

		int lines_count = get_num_lines(paragraph);
		//daca paragraful are mai putin de 20 de linii, se porneste un singur thread de procesare
		if (lines_count <= NR_LINES) {
			int r;
			pthread_t processing_thread;
			r = pthread_create(&processing_thread, NULL, worker_data_processing_fantasy, (void *)paragraph);
			if (check_thread_start(r, HORROR_WORKER) == false) {
				exit(-1);
			}
			char *processed_paragraph_horror = NULL;
		   	r = pthread_join(processing_thread, (void**)&processed_paragraph_horror);
		    if (r != 0) {
		    	printf("Eroare la asteptarea thread-ului de procesare in worker-ul %d\n", FANTASY_WORKER);
		    	exit(-1);
		    }
		   
		    
		    //trimite paragraful procesat catre master
		//    int buffer_dim = MAX_SIZE + MPI_BSEND_OVERHEAD;
		//    char *buffer_memory = (char *)malloc(buffer_dim * sizeof(char));
		//    MPI_Buffer_attach(buffer_memory, buffer_dim);

		    MPI_Bsend(processed_paragraph_horror, strlen(processed_paragraph_horror) + 1, MPI_CHAR, MASTER, index, MPI_COMM_WORLD);	
		    index++;
		    //printf("Textul procesat in worker-ul HORROR este:%s\n", (char*)processed_paragraph_horror);
		 	//MPI_Buffer_detach(&buffer_memory, &buffer_dim);
		 	free(paragraph);
		 	paragraph = NULL;
		}
		else {
			int P = sysconf(_SC_NPROCESSORS_CONF); //numarul de threaduri disponibile
			//printf("Se vor folosit mai multe threaduri pentru procesare\n");
			//se pornesc maxim P-1 threaduri pentru procesare(o data sau de mai multe ori in functie de numarul de linii)
			//printf("NUMARUL DE LINII: %d\n", lines_count);
			int threads_num = lines_count / NR_LINES + ceil((lines_count % NR_LINES) / (float)NR_LINES); //numarul de thread-uri de care ar fi nevoie pt a imparti procesarea liniilor
			//printf("numarul de threaduri de care avem nevoie este: %d\n", threads_num);
			//printf("numar disponibile: %d\n", P - 1);

			//trebuie sa folosim un thread de mai multe ori pentru a procesa paragraful
			
			if (threads_num > P - 1) {
				char *copy_paragraph = (char *)malloc((strlen(paragraph) + 1) * sizeof(char));
				strcpy(copy_paragraph, paragraph);
				char *final_paragraph = extract_and_process_chuncks(copy_paragraph, FANTASY_WORKER, lines_count);

				//trimite paragraful final procesat catre MASTER
				//printf("!!!Paragaraful final procesat: %s\n", final_paragraph);
			//	int buffer_dim = MAX_SIZE + MPI_BSEND_OVERHEAD;
			//    char *buffer_memory = (char *)malloc(buffer_dim * sizeof(char));
			//    MPI_Buffer_attach(buffer_memory, buffer_dim);

			    MPI_Bsend(final_paragraph, strlen(final_paragraph) + 1, MPI_CHAR, MASTER, index, MPI_COMM_WORLD);
			    index++;
			}
			//threadurile disponibile sunt suficiente pentru a procesa paragraful pornindu-le o singura data
			else if (threads_num <= P - 1) {
				
				pthread_t processing_threads[threads_num]; //fiecare thread va procesa maxim 20 de linii

				char *line = strtok(paragraph, "\n");
				int current_line = 0;
				char *next_line = malloc(1);
				*next_line = '\0';

				//obtine chunck-ul din paragraf format din liniile care vor fi procesate de fiecare thread i
				int i;
				for (i = 0; i < threads_num; i++) {
					char *paragraph_chunk = malloc(1);
					*paragraph_chunk = '\0';
					int start_line = NR_LINES * i;
					int end_line;
					if (lines_count < NR_LINES * (i + 1) - 1) { //unul din threaduri va primi mai putin de 20 de linii
						end_line = lines_count;
					}
					else {
						end_line = NR_LINES * (i + 1) - 1;
					}
					if (strlen(next_line) != 0) {
						char *complete_line = (char *)malloc(strlen(next_line) + 2);
						sprintf(complete_line, "%s\n", next_line);
						int new_size = strlen(complete_line) + 1 + strlen(paragraph_chunk);
						char *aux = (char *)realloc(paragraph_chunk, new_size);
						if (!aux) {
							exit(-1);
						}
						paragraph_chunk = aux;
						strcat(paragraph_chunk, complete_line);
					}
					while (line != NULL) {
						if (current_line >= start_line && current_line <= end_line) {
							char *complete_line = (char *)malloc(strlen(line) + 2);
							sprintf(complete_line, "%s\n", line);
							int new_size = strlen(complete_line) + 1 + strlen(paragraph_chunk);
							char *aux = (char *)realloc(paragraph_chunk, new_size);
							if (!aux) {
								exit(-1);
							}
							paragraph_chunk = aux;
							strcat(paragraph_chunk, complete_line);
						}
						else {
							//linia curenta va face parte din urmatorul chunk
							char *aux = (char *)realloc(next_line, strlen(line) + 1);
							if (!aux) {
								exit(-1);
							}
							strcpy(aux, line);
							next_line = aux;
							current_line++;
							break;
						}
						line = strtok(NULL, "\n");
						current_line++;
					}

					int r = pthread_create(&processing_threads[i], NULL, worker_data_processing_fantasy, (void *)paragraph_chunk);
					if (check_thread_start(r, HORROR_WORKER) == false) {
						exit(-1);
					}
					if (line != NULL) {
						line = strtok(NULL, "\n");
					}
				}

				//paragraful final procesat care va fi trimis catre master
				char *final_paragraph = malloc(1);
				*final_paragraph = '\0';
				for (i = 0; i < threads_num; i++) {
					char *processed_paragraph_horror = NULL;
			        int r = pthread_join(processing_threads[i], (void**)&processed_paragraph_horror);
			        if (r) {
			            printf("Eroare la asteptarea thread-ului %d\n", i);
			            exit(-1);
			        }
			       // printf("Chunck procesat: %s\n", processed_paragraph_horror);
			        char *aux = (char *)realloc(final_paragraph, strlen(processed_paragraph_horror) + 1 + strlen(final_paragraph));
			        if (!aux) {
			        	exit(-1);
			        }
			        final_paragraph = aux;
			        strcat(final_paragraph, processed_paragraph_horror);
	   		 	}   
		   	//	int buffer_dim = MAX_SIZE + MPI_BSEND_OVERHEAD;
			 //   char *buffer_memory = (char *)malloc(buffer_dim * sizeof(char));
			 //   MPI_Buffer_attach(buffer_memory, buffer_dim);

			    MPI_Bsend(final_paragraph, strlen(final_paragraph) + 1, MPI_CHAR, MASTER, index, MPI_COMM_WORLD);
			    index++;
			    //printf("!!!!!!!!!!!!!!!!!!!!Paragaraful final procesat fantasy: %s\n", final_paragraph);
			    free(paragraph);
		 		paragraph = NULL;
			}
		}
	}

	//printf("S-au procesat si trimis toate paragrafele horror\n");
	pthread_exit(NULL);
}


int main (int argc, char *argv[]) {
	//if (argc != 2) {
		//printf("[!] Foloseste \"mpirun -np [nr_nodes] tema [nume_fisier_input]\"");
		//exit(-1);
	//}
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

    int buffer_dim = MAX_SIZE + MPI_BSEND_OVERHEAD;
	char *buffer_memory = (char *)malloc(buffer_dim * sizeof(char));
	MPI_Buffer_attach(buffer_memory, buffer_dim);


	MPI_Request request = MPI_REQUEST_NULL;

    if (rank == MASTER) {
    	//printf("\n");
    	
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

    	for (i = 0; i < MASTER_THREADS; i++) {
    		
    		int r = pthread_join(master_threads[i], NULL);
 
	        if (r) {
	            printf("Eroare la asteptarea thread-ului %ld\n", i);
	            exit(-1);
	        }
    	}

    	//se va retine numarul de paragrafe de fiecare tip care ajung la master pt a mentine ordinea
    	/*este determinat numarul de paragrafe de tipul horror care se gasesc in fisier*/
		/*este retinuta o lista declarata la nivelul nodului master care memoreaza ordinea in care apar paragrafele in functie de tipurile lor*/
		FILE *fptr = fopen(file_name, "r");
		if (fptr == NULL) {
			printf("Eroare la deschiderea fisierului de intrare\n");
			exit(-1);
		}

		char *line_buffer;
		size_t buffer_size; //dimeniunea liniei(incluzand\n)
		ssize_t result;
		char *paragraph = NULL;
		bool check_paragraph_end = false;

		char *final_text = malloc(1);
    	*final_text = '\0';
    	int horror_par = 0;
    	int comedy_par = 0;
    	int sciFi_par = 0;
    	int fantasy_par = 0;

		while ((result = getline(&line_buffer, &buffer_size, fptr)) != -1) {
			if (strcmp(line_buffer, "horror\n") == 0) {
				char *processed_paragraph;
				MPI_Status status;
				int buffer_size;

				MPI_Probe(HORROR_WORKER, horror_par, MPI_COMM_WORLD, &status);
								
				MPI_Get_count(&status, MPI_CHAR, &buffer_size);
							
				processed_paragraph = (char *)malloc(buffer_size * sizeof(char));
				MPI_Recv(processed_paragraph, buffer_size, MPI_CHAR, HORROR_WORKER, horror_par, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				//printf("horror\n%s\n", processed_paragraph);

				char *append_str = (char *)malloc(strlen(processed_paragraph) + strlen("horror\n") + 2);
				sprintf(append_str, "horror\n%s\n", processed_paragraph);

				int new_size = strlen(append_str) + strlen(final_text) + 1;
				char *aux = (char *)realloc(final_text, new_size);
				if (!aux) {
					exit(-1);
				}
				final_text = aux;
				strcat(final_text, append_str);
				
				//printf("%s", final_text);
				horror_par++;
			}
			if (strcmp(line_buffer, "comedy\n") == 0) {
			
				char *processed_paragraph;
				MPI_Status status;
				int buffer_size;

				MPI_Probe(COMEDY_WORKER, comedy_par, MPI_COMM_WORLD, &status);
								
				MPI_Get_count(&status, MPI_CHAR, &buffer_size);
							
				processed_paragraph = (char *)malloc(buffer_size * sizeof(char));
				MPI_Recv(processed_paragraph, buffer_size, MPI_CHAR, COMEDY_WORKER, comedy_par, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				//printf("comedy\n%s\n", processed_paragraph);

				char *append_str = (char *)malloc(strlen(processed_paragraph) + strlen("comedy\n") + 2);
				sprintf(append_str, "comedy\n%s\n", processed_paragraph);

				int new_size = strlen(append_str) + strlen(final_text) + 1;
				char *aux = (char *)realloc(final_text, new_size);
				if (!aux) {
					exit(-1);
				}
				final_text = aux;
				strcat(final_text, append_str);

				//printf("%s", final_text);
				comedy_par++;
			}
			if (strcmp(line_buffer, "fantasy\n") == 0) {
		
				char *processed_paragraph;
				MPI_Status status;
				int buffer_size;

				MPI_Probe(FANTASY_WORKER, fantasy_par, MPI_COMM_WORLD, &status);
								
				MPI_Get_count(&status, MPI_CHAR, &buffer_size);
							
				processed_paragraph = (char *)malloc(buffer_size * sizeof(char));
				MPI_Recv(processed_paragraph, buffer_size, MPI_CHAR, FANTASY_WORKER, fantasy_par, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				
				
				char *append_str = (char *)malloc(strlen(processed_paragraph) + strlen("fantasy\n") + 2);
				sprintf(append_str, "fantasy\n%s\n", processed_paragraph);

				int new_size = strlen(append_str) + strlen(final_text) + 1;
				char *aux = (char *)realloc(final_text, new_size);
				if (!aux) {
					exit(-1);
				}
				final_text = aux;
				strcat(final_text, append_str);
				fantasy_par++;
			}
			if (strcmp(line_buffer, "science-fiction\n") == 0) {
				char *processed_paragraph;
				MPI_Status status;
				int buffer_size;

				MPI_Probe(SCIFI_WORKER, sciFi_par, MPI_COMM_WORLD, &status);
								
				MPI_Get_count(&status, MPI_CHAR, &buffer_size);
							
				processed_paragraph = (char *)malloc(buffer_size * sizeof(char));
				MPI_Recv(processed_paragraph, buffer_size, MPI_CHAR, SCIFI_WORKER, sciFi_par, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				//printf("science-fiction\n%s\n", processed_paragraph);

				char *append_str = (char *)malloc(strlen(processed_paragraph) + strlen("science-fiction\n") + 2);
				sprintf(append_str, "science-fiction\n%s\n", processed_paragraph);

				int new_size = strlen(append_str) + strlen(final_text) + 1;
				char *aux = (char *)realloc(final_text, new_size);
				if (!aux) {
					exit(-1);
				}
				final_text = aux;
				strcat(final_text, append_str);
					
				//strcat(final_text, "\n");
				
				//printf("%s", final_text);
				sciFi_par++;
			}
		}

		fclose(fptr);
	
    	int length = strlen(file_name);

    	char *out_file = (char *)malloc(strlen(file_name) + 1);
    	strcpy(out_file, file_name);
    	
    	for(i = 0; i < length - 1; i++) {
	   		if(out_file[i] == '.' && out_file[i + 1] != '/') {
	       		out_file[i]='\0';
	       		break;
	   		}
		}
		strcat(out_file, ".out");
		//printf("out_file: %s\n", file_name);

    	FILE *out_fptr = fopen(out_file, "w");
    	if (out_fptr == NULL) {
    		printf("Eroare la scrierea in fisier\n");
    		exit(-1);
    	}

    	fprintf(out_fptr, "%s", final_text);

    	//printf("%s", final_text);
	}
    else {
    	

    	//in fiecare worker este pornit initial thread-ul 'reader' care recepteaza datele trimise de master si porneste ulterior threadurile de procesare
        pthread_t reader;
        int r = 0;

        //functia de thread reader va prima ca argument rank-ul workerului de catre care aceasta este pornita
        if (rank == HORROR_WORKER) {
        	r = pthread_create(&reader, NULL, worker_horror_reader_f, NULL);
        }
        else if (rank == COMEDY_WORKER) {
        	r = pthread_create(&reader, NULL, worker_comedy_reader_f, NULL);
        }
        else if (rank == FANTASY_WORKER) {
        	r = pthread_create(&reader, NULL, worker_fantasy_reader_f, NULL);
        }
        else if (rank == SCIFI_WORKER) {
        	r = pthread_create(&reader, NULL, worker_sciFi_reader_f, NULL);
        }

        void *return_value;
        r = pthread_join(reader, &return_value);
    	if (r != 0) {
    		printf("Eroare la asteptarea thread-ului reader din worker-ul %d\n", rank);
    		exit(-1);
    	}
    }

    MPI_Finalize();   

    
}