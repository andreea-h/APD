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
#define NR_LINES 20 

#define MIN(x, y) (((x) < (y)) ? (x) : (y))

static char *file_name;
static int *paragraphs_order;
static int num_paragraphs;
static char** horror_processed_paragraphs;
static char** comedy_processed_paragraphs;
static char** fantasy_processed_paragraphs;
static char** scifi_processed_paragraphs;
static int num_horror; //numarul de paragrafe horror
static int num_comedy; //numarul de paragrafe comedy
static int num_scifi; //numarul de paragrafe scifi
static int num_fantasy; //numarul de paragrafe fantasy

/*functia de thread executata in thread-ul master care trimite paragrafele horror
si face receive paragrafelor horror procesate*/
void *master_f_horror(void *arg) {

	FILE *fptr = fopen(file_name, "r");
	if (fptr == NULL) {
		printf("Eroare la deschiderea fisierului de intrare\n");
		exit(-1);
	}

	char *line_buffer;
	size_t buffer_size; //dimeniunea liniei(incluzand\n)
	ssize_t result;
	
	bool check_paragraph_end = false;

	while ((result = getline(&line_buffer, &buffer_size, fptr)) != -1) {
		if (strcmp(line_buffer, "horror\n") == 0) {
			num_paragraphs++;
			num_horror++;
			char *paragraph = malloc(1);
			*paragraph = '\0';
			//citeste continutul paragrafului si trimite-l catre workerul 1
			while (check_paragraph_end == false) {
				result = getline(&line_buffer, &buffer_size, fptr);
				if (result != -1) {
					
					int size = strlen(paragraph) + strlen(line_buffer) + 2;
					char *paragraph_aux = (char *)realloc(paragraph, size);
					if (!paragraph_aux) {
						exit(-1);
					}
					paragraph = paragraph_aux;
					strcat(paragraph, line_buffer);
					
					//este identificat finalul paragrafului
					if (paragraph[strlen(paragraph) - 2] == '\n') {
						paragraph[strlen(paragraph) - 1] = '\0';
						check_paragraph_end = true;
						MPI_Send(paragraph, strlen(paragraph) + 1, MPI_CHAR, HORROR_WORKER, HORROR_WORKER, MPI_COMM_WORLD);
						free(paragraph);
						paragraph = NULL;
					}
				} 
				else {

					MPI_Send(paragraph, strlen(paragraph) + 1, MPI_CHAR, HORROR_WORKER, HORROR_WORKER, MPI_COMM_WORLD);
					free(paragraph);
					paragraph = NULL;
					break;
				}
				
			}
			if (check_paragraph_end == true) {
				check_paragraph_end = false;
			}

			//retine aparitia paragrafului prin indexul asociat tipului acestuia
			//1-horror, 2-comedy, 3-scifi, 4-fantasy
			if (num_paragraphs == 1) {
				paragraphs_order = (int *)malloc(sizeof(int));
				paragraphs_order[0] = 1; 
			}
			else {
				int *aux = (int *)realloc(paragraphs_order, num_paragraphs * sizeof(int));
				if (!aux) {
					exit(-1);
				}
				paragraphs_order = aux;
				paragraphs_order[num_paragraphs - 1] = 1;
			}
		}
		else if (strcmp(line_buffer, "comedy\n") == 0) {
			num_paragraphs++;
			if (num_paragraphs == 1) {
				paragraphs_order = (int *)malloc(sizeof(int));
				paragraphs_order[0] = 2; 
			}
			else {
				int *aux = (int *)realloc(paragraphs_order, num_paragraphs * sizeof(int));
				if (!aux) {
					exit(-1);
				}
				paragraphs_order = aux;
				paragraphs_order[num_paragraphs - 1] = 2;
			}
		}
		else if (strcmp(line_buffer, "science-fiction\n") == 0) {
			num_paragraphs++;
			if (num_paragraphs == 1) {
				paragraphs_order = (int *)malloc(sizeof(int));
				paragraphs_order[0] = 3; 
			}
			else {
				int *aux = (int *)realloc(paragraphs_order, num_paragraphs * sizeof(int));
				if (!aux) {
					exit(-1);
				}
				paragraphs_order = aux;
				paragraphs_order[num_paragraphs - 1] = 3;
			}
		}
		else if (strcmp(line_buffer, "fantasy\n") == 0) {
			num_paragraphs++;
			if (num_paragraphs == 1) {
				paragraphs_order = (int *)malloc(sizeof(int));
				paragraphs_order[0] = 4; 
			}
			else {
				int *aux = (int *)realloc(paragraphs_order, num_paragraphs * sizeof(int));
				if (!aux) {
					exit(-1);
				}
				paragraphs_order = aux;
				paragraphs_order[num_paragraphs - 1] = 4;
			}
		}
	}
	
	//trimite un mesaj care anunta ca s-au trimis toate paragrafele de tipul horror
	char *message = strdup("DONE");
	MPI_Send(message, strlen(message) + 1, MPI_CHAR, HORROR_WORKER, HORROR_WORKER, MPI_COMM_WORLD);
	free(message);
	message = NULL;

	fclose(fptr);
	//se face receive paragrafele procesate de tipul horror
	int i;
	horror_processed_paragraphs = (char **)malloc(num_horror * sizeof(char *));
	for (i = 0; i < num_horror; i++) {
		char *processed_paragraph;
		MPI_Status status;
		int buffer_size;

		MPI_Probe(HORROR_WORKER, i, MPI_COMM_WORLD, &status);
						
		MPI_Get_count(&status, MPI_CHAR, &buffer_size);

		processed_paragraph = (char *)malloc(buffer_size * sizeof(char));
		MPI_Recv(processed_paragraph, buffer_size, MPI_CHAR, HORROR_WORKER, i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		
		horror_processed_paragraphs[i] = (char *)malloc(buffer_size * sizeof(char));
		strcpy(horror_processed_paragraphs[i], processed_paragraph);
		free(processed_paragraph);
	}

	pthread_exit(NULL);
}

/*functia de thread executata in thread-ul master care trimite paragrafele comedy
si face receive paragrafelor comedy procesate*/
void *master_f_comedy(void *arg) {
	FILE *fptr = fopen(file_name, "r");
	if (fptr == NULL) {
		printf("Eroare la deschiderea fisierului de intrare\n");
		exit(-1);
	}

	char *line_buffer;
	size_t buffer_size; //dimeniunea liniei(inluzand\n)
	ssize_t result;
	
	bool check_paragraph_end = false;

	while ((result = getline(&line_buffer, &buffer_size, fptr)) != -1) {
		if (strcmp(line_buffer, "comedy\n") == 0) {
			num_comedy++;
			char *paragraph = malloc(1);
			*paragraph = '\0';
			//memoreaza continutul paragrafului comedy si trimite-l catre workerul 2
			while (check_paragraph_end == false) {
				result = getline(&line_buffer, &buffer_size, fptr);
				if (result != -1) {

					int size = strlen(paragraph) + strlen(line_buffer) + 2;
					char *paragraph_aux = (char *)realloc(paragraph, size);
					if (!paragraph_aux) {
						exit(-1);
					}
					paragraph = paragraph_aux;
					strcat(paragraph, line_buffer);

					if (paragraph[strlen(paragraph) - 2] == '\n') {
						paragraph[strlen(paragraph) - 1] = '\0';
						check_paragraph_end = true;
						MPI_Send(paragraph, strlen(paragraph) + 1, MPI_CHAR, COMEDY_WORKER, COMEDY_WORKER, MPI_COMM_WORLD);
						free(paragraph);
						paragraph = NULL;
					}

				}
				else {
					MPI_Send(paragraph, strlen(paragraph) + 1, MPI_CHAR, COMEDY_WORKER, COMEDY_WORKER, MPI_COMM_WORLD);
					free(paragraph);
					paragraph = NULL;
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
	MPI_Send(message, strlen(message) + 1, MPI_CHAR, COMEDY_WORKER, COMEDY_WORKER, MPI_COMM_WORLD);	
	free(message);
	message = NULL;

	fclose(fptr);
	//se face receive paragrafele procesate de tipul horror
	int i;
	comedy_processed_paragraphs = (char **)malloc(num_comedy * sizeof(char *));
	for (i = 0; i < num_comedy; i++) {
		char *processed_paragraph;
		MPI_Status status;
		int buffer_size;

		MPI_Probe(COMEDY_WORKER, i, MPI_COMM_WORLD, &status);
						
		MPI_Get_count(&status, MPI_CHAR, &buffer_size);

		processed_paragraph = (char *)malloc(buffer_size * sizeof(char));
		MPI_Recv(processed_paragraph, buffer_size, MPI_CHAR, COMEDY_WORKER, i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		
		//memoreaza paragraful procesat de tipul horror primit 
		comedy_processed_paragraphs[i] = (char *)malloc(buffer_size * sizeof(char));
		strcpy(comedy_processed_paragraphs[i], processed_paragraph);
		free(processed_paragraph);
		processed_paragraph = NULL;
	}
	pthread_exit(NULL);
}

/*functia de thread executata in thread-ul master care trimite paragrafele scifi
si face receive paragrafelor scifi procesate*/
void *master_f_scifi(void *arg) {
	FILE *fptr = fopen(file_name, "r");
	if (fptr == NULL) {
		printf("Eroare la deschiderea fisierului de intrare\n");
		exit(-1);
	}

	char *line_buffer;
	size_t buffer_size; //dimeniunea liniei(inluzand\n)
	ssize_t result;
	
	bool check_paragraph_end = false;

	while ((result = getline(&line_buffer, &buffer_size, fptr)) != -1) {
		if (strcmp(line_buffer, "science-fiction\n") == 0) {
			num_scifi++;
			char *paragraph = malloc(1);
			*paragraph = '\0';

			//memoreaza continutul paragrafului si trimite-l catre workerul 3
			while (check_paragraph_end == false) {
				result = getline(&line_buffer, &buffer_size, fptr);
				if (result != -1) {

					int size = strlen(paragraph) + strlen(line_buffer) + 2;
					char *paragraph_aux = (char *)realloc(paragraph, size);
					if (!paragraph_aux) {
						exit(-1);
					}
					paragraph = paragraph_aux;
					strcat(paragraph, line_buffer);

					if (paragraph[strlen(paragraph) - 2] == '\n') {
						paragraph[strlen(paragraph) - 1] = '\0';
						check_paragraph_end = true;
						MPI_Send(paragraph, strlen(paragraph) + 1, MPI_CHAR, SCIFI_WORKER, SCIFI_WORKER, MPI_COMM_WORLD);
						free(paragraph);
						paragraph = NULL;
					}
				}
				else {
					MPI_Send(paragraph, strlen(paragraph) + 1, MPI_CHAR, FANTASY_WORKER, FANTASY_WORKER, MPI_COMM_WORLD);
					free(paragraph);
					paragraph = NULL;
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
	MPI_Send(message, strlen(message) + 1, MPI_CHAR, SCIFI_WORKER, SCIFI_WORKER, MPI_COMM_WORLD);
	free(message);
	fclose(fptr);
	//se face receive paragrafele procesate de tipul horror
	int i;
	scifi_processed_paragraphs = (char **)malloc(num_scifi * sizeof(char *));
	for (i = 0; i < num_scifi; i++) {
		char *processed_paragraph;
		MPI_Status status;
		int buffer_size;

		MPI_Probe(SCIFI_WORKER, i, MPI_COMM_WORLD, &status);
						
		MPI_Get_count(&status, MPI_CHAR, &buffer_size);

		processed_paragraph = (char *)malloc(buffer_size * sizeof(char));
		MPI_Recv(processed_paragraph, buffer_size, MPI_CHAR, SCIFI_WORKER, i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		
		//memoreaza paragraful procesat de tipul horror primit 
		scifi_processed_paragraphs[i] = (char *)malloc(buffer_size * sizeof(char));
		strcpy(scifi_processed_paragraphs[i], processed_paragraph);
		free(processed_paragraph);
		processed_paragraph = NULL;
	}

	pthread_exit(NULL);
}

/*functia de thread executata in thread-ul master care trimite paragrafele fantasy
si face receive paragrafelor fantasy procesate*/
void *master_f_fantasy(void *arg) {
	FILE *fptr = fopen(file_name, "r");
	if (fptr == NULL) {
		printf("Eroare la deschiderea fisierului de intrare\n");
		exit(-1);
	}

	char *line_buffer;
	size_t buffer_size; //dimeniunea liniei(inluzand\n)
	ssize_t result;
	
	bool check_paragraph_end = false;

	while ((result = getline(&line_buffer, &buffer_size, fptr)) != -1) {
		if (strcmp(line_buffer, "fantasy\n") == 0) {
			num_fantasy++;
			char *paragraph = malloc(1);
			*paragraph = '\0';
			//memoreaza continutul paragrafului fantasy si trimite-l catre workerul 4
			while (check_paragraph_end == false) {
				result = getline(&line_buffer, &buffer_size, fptr);
				if (result != -1) {
					
					int size = strlen(paragraph) + strlen(line_buffer) + 2;
					char *paragraph_aux = (char *)realloc(paragraph, size);
					if (!paragraph_aux) {
						exit(-1);
					}
					paragraph = paragraph_aux;
					strcat(paragraph, line_buffer);

					if (paragraph[strlen(paragraph) - 2] == '\n') {
						paragraph[strlen(paragraph) - 1] = '\0';
						check_paragraph_end = true;

						MPI_Send(paragraph, strlen(paragraph) + 1, MPI_CHAR, FANTASY_WORKER, FANTASY_WORKER, MPI_COMM_WORLD);
						free(paragraph);
						paragraph = NULL;
					}
				}
				else {

					MPI_Send(paragraph, strlen(paragraph) + 1, MPI_CHAR, FANTASY_WORKER, FANTASY_WORKER, MPI_COMM_WORLD);
					free(paragraph);
					paragraph = NULL;
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
	MPI_Send(message, strlen(message) + 1, MPI_CHAR, FANTASY_WORKER, FANTASY_WORKER, MPI_COMM_WORLD);
	free(message);
	message = NULL;

	fclose(fptr);
	//se face receive paragrafele procesate de tipul horror
	int i;

	fantasy_processed_paragraphs = (char **)malloc(num_fantasy * sizeof(char *));
	for (i = 0; i < num_fantasy; i++) {
		char *processed_paragraph;
		MPI_Status status;
		int buffer_size;

		MPI_Probe(FANTASY_WORKER, i, MPI_COMM_WORLD, &status);
						
		MPI_Get_count(&status, MPI_CHAR, &buffer_size);

		processed_paragraph = (char *)malloc(buffer_size * sizeof(char));
		MPI_Recv(processed_paragraph, buffer_size, MPI_CHAR, FANTASY_WORKER, i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		
		
		fantasy_processed_paragraphs[i] = (char *)malloc(buffer_size * sizeof(char));
		strcpy(fantasy_processed_paragraphs[i], processed_paragraph);
		free(processed_paragraph);
		processed_paragraph = NULL;
	}

	pthread_exit(NULL);
}

/*returneaza true daca c este litera*/
bool isLetter(char c) {
	return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
}

bool isVowel(char c) {
	char copy = tolower(c);
	return (copy == 'a' || copy == 'e' || copy == 'i' || copy == 'o' || copy == 'u');
}

/*functia de thread executata de threadurile de procesare de pe worker-ul horror*/
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

	processed_paragraph = (char *)malloc((size + k + 1) * sizeof(char));
	int length = 0;
	for (i = 0; i < size; i++) {
		processed_paragraph[length] = paragraph[i];
		length++;
		if (isLetter(paragraph[i]) == true) {
			//verifaca daca litera este consoana
			if (isVowel(paragraph[i]) == false) {
				//aloca un char in plus pentru consoana care va fi introdusa
				processed_paragraph[length] = tolower(paragraph[i]);
				length++;
			}
		}
		
	}	
	return processed_paragraph;
}

/*functia de thread executata de threadurile de procesare de pe worker-ul comedy*/
void *worker_data_processing_comedy(void *arg) {
	char *paragraph = (char*) arg;
	char *processed_paragraph = (char *)malloc((strlen(paragraph) + 1) * sizeof(char));

	char *copy = strdup(paragraph);
	char *res = strdup(paragraph);

	char *token = strtok_r(paragraph, " \n", &res);

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
	return processed_paragraph;
}

/*functia de thread executata de threadurile de procesare de pe worker-ul scifi*/
void *worker_data_processing_SciFi(void *arg) {
	//al saptelea cuvant de pe fiecare rand este inversat
	char *paragraph = (char *) arg;
	char *processed_paragraph = (char *)malloc((strlen(paragraph) + 2) * sizeof(char));

	char *rest = paragraph;
	//reentrant version of strtok
	char *line = strtok_r(paragraph, "\n", &rest);
	int count_words = 0;
	
	while (line != NULL) {
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
					int length = strlen(word) + 1;
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

	return processed_paragraph;
}

/*functia de thread executata de threadurile de procesare de pe worker-ul fantasy*/
void *worker_data_processing_fantasy(void *arg) {
	char *paragraph = (char *) arg;
	
	int size = strlen(paragraph) + 1;
	char *processed_paragraph = (char *)malloc((strlen(paragraph) + 1) * sizeof(char));

	char *paragraph_copy = (char *)malloc(size * sizeof(char));
	strcpy(paragraph_copy, paragraph);

	//prima litera a fiecarui cuvant trebuie facuta majuscula
	char *copy = (char *)malloc(size * sizeof(char));
	char *token_fan = strtok_r(paragraph_copy, " \n", &copy);
	int position_fan = 0;
	while (token_fan != NULL) {
		char first = toupper(token_fan[0]);
		token_fan[0] = first;
		position_fan += strlen(token_fan);
		
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

	return processed_paragraph;
}


/*intoarce o matrice de stringuri in care fiecare string are maxim 20 linii*/
/*num_chunks reprezinta numarul de chunkuri de maxim 20 linii fiecare identificate in paragraf*/
char** divide_paragraph(char *paragraph, int *num_chunks) {

	char *paragraph_chunk = malloc(1); //retine un chunkul din paragraf de maxim 20 de linii
	*paragraph_chunk = '\0';
	
	int current_line = 0;

	int start_line;
	int end_line;	
	int thread_index = 0;

	char **paragraph_chunks = NULL;

	char *line = strtok(paragraph, "\n");
	while (line != NULL) {
		start_line = NR_LINES * thread_index;
		end_line = (thread_index + 1) * NR_LINES - 1 ;

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
		}
		else { //s-au identificat inca 20 de linii, deci se va memora chunkul gasit
			if (thread_index == 0) {
				paragraph_chunks = (char **)malloc(sizeof(char *));
				paragraph_chunks[thread_index] = (char *)malloc((strlen(paragraph_chunk) + 1) * sizeof(char));
				strcpy(paragraph_chunks[thread_index], paragraph_chunk);
			}
			else {
				char **aux = (char **)realloc(paragraph_chunks, (thread_index + 1) * sizeof(char *));
				if (!aux) {
					exit(-1);
				}
				paragraph_chunks = aux;
				paragraph_chunks[thread_index] = (char *)malloc((strlen(paragraph_chunk) + 1) * sizeof(char));
				strcpy(paragraph_chunks[thread_index], paragraph_chunk);
			}
			thread_index++;
			//urmeaza sa se construiasca un nou chunk
			free(paragraph_chunk);
			paragraph_chunk = malloc(1); //retine un chunkul din paragraf de maxim 20 de linii
			*paragraph_chunk = '\0';

			//se adauga linia curenta identificata la chunkul care urmeaza sa se formeze
			char *complete_line = (char *)malloc(strlen(line) + 2);
			sprintf(complete_line, "%s\n", line);
			int new_size = strlen(complete_line) + 2 + strlen(paragraph_chunk);
			char *aux = (char *)realloc(paragraph_chunk, new_size);

			if (!aux) {
				exit(-1);
			}
			paragraph_chunk = aux;
			strcat(paragraph_chunk, complete_line);
		}

		line = strtok(NULL, "\n");
		current_line++;
	}

	if (strlen(paragraph_chunk) != 0) {
		if (thread_index == 0) {
			paragraph_chunks = (char **)malloc(sizeof(char *));
			paragraph_chunks[thread_index] = (char *)malloc((strlen(paragraph_chunk) + 1) * sizeof(char));
			strcpy(paragraph_chunks[thread_index], paragraph_chunk);
			thread_index++;
		}
		else {
			char **aux = (char **)realloc(paragraph_chunks, (thread_index + 1) * sizeof(char *));
			if (!aux) {
				exit(-1);
			}
			paragraph_chunks = aux;
			paragraph_chunks[thread_index] = (char *)malloc((strlen(paragraph_chunk) + 1) * sizeof(char));
			strcpy(paragraph_chunks[thread_index], paragraph_chunk);
			thread_index++;
		}
	}
	*num_chunks = thread_index;
	return paragraph_chunks;
}


bool check_thread_start(int r, long worker_rank) {
	if (r != 0) {
    	printf("Eroare la crearea thread-ului de procesare in worker-ul %ld\n", worker_rank);
    	return false;
    }
    return true;
}

/*functia de thread executata de thread-ul reader din worker-ul horror*/
void *worker_horror_reader_f(void *arg) {

	char **paragraphs = NULL;
	int index = 0;
	int nr = 0;
	while(1) {
		//face receive la paragraful care i-a fost trimis de catre thread-ul aferent tipului sau din master
		char *paragraph;
		MPI_Status status;
		int buffer_size;
		MPI_Probe(MASTER, HORROR_WORKER, MPI_COMM_WORLD, &status);
			
		MPI_Get_count(&status, MPI_CHAR, &buffer_size);
		
		MPI_Status status_recv;
		paragraph = (char *)malloc(buffer_size * sizeof(char));

		MPI_Recv(paragraph, buffer_size, MPI_CHAR, MASTER, HORROR_WORKER, MPI_COMM_WORLD, &status_recv);
			
		if (strcmp(paragraph, "DONE") == 0) {
			break;
		}
		if (nr == 0) {
			paragraphs = (char **)malloc(sizeof(char *));
			paragraphs[0] = (char *)malloc(buffer_size * sizeof(char));
			strcpy(paragraphs[0], paragraph);
		}
		else {

			char **aux = (char **)realloc(paragraphs, sizeof(char *) * (nr + 1));
			if (!aux) {
				exit(-1);
			}
			paragraphs = aux;
			paragraphs[nr] = (char *)malloc(buffer_size * sizeof(char));
			strcpy(paragraphs[nr], paragraph);
		}
		nr++;
		free(paragraph);
		paragraph = NULL;
	}

	int P = sysconf(_SC_NPROCESSORS_CONF) - 1; //numarul de threaduri disponibile 
	char *paragraph;

	for (index = 0; index < nr; index++) {
		char *final_paragraph = malloc(1);
		*final_paragraph = '\0';

		//paragraful care trebuie trimis catre procesare
		paragraph = (char *)malloc(strlen(paragraphs[index]) + 1);
		strcpy(paragraph, paragraphs[index]);

		int num_chunks;
		char **paragraph_chunks = divide_paragraph(paragraph, &num_chunks);
	
		int i;

		int threads_num = MIN(P, num_chunks);

		int chunk = 0;

		int next_chunks = num_chunks;
		for (i = 0; i < num_chunks; i++) {
			chunk = i;
			//threads_num trebuie actualizat cu numarul de chunkuri care mai trebuie procesate
			threads_num = MIN(P, next_chunks);

			pthread_t processing_threads[threads_num];

			int j;
			for (j = 0; j < threads_num; j++) {
				int r = pthread_create(&processing_threads[j], NULL, worker_data_processing_horror, (void *)paragraph_chunks[chunk]);
		        if (r) {
		            printf("Eroare la asteptarea thread-ului %d\n", i);
		            exit(-1);
		   		}
		   		chunk++;
		   		next_chunks -= 1;
			}

			for (j = 0; j < threads_num; j++) {
				char *processed_paragraph = NULL;
		        int r = pthread_join(processing_threads[j], (void**)&processed_paragraph);
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
			i = chunk - 1;
		}

		MPI_Send(final_paragraph, strlen(final_paragraph) + 1, MPI_CHAR, MASTER, index, MPI_COMM_WORLD);
	}

	pthread_exit(NULL);
}

/*functia de thread executata de thread-ul reader din worker-ul comedy*/
void *worker_comedy_reader_f(void *arg) {
	
	char **paragraphs = NULL;
	int index = 0;
	int nr = 0;
	while(1) {
		char *paragraph;
		MPI_Status status;
		int buffer_size;
		MPI_Probe(MASTER, COMEDY_WORKER, MPI_COMM_WORLD, &status);
			
		MPI_Get_count(&status, MPI_CHAR, &buffer_size);
		
		MPI_Status status_recv;
		paragraph = (char *)malloc(buffer_size * sizeof(char));

		MPI_Recv(paragraph, buffer_size, MPI_CHAR, MASTER, COMEDY_WORKER, MPI_COMM_WORLD, &status_recv);
			
		if (strcmp(paragraph, "DONE") == 0) {
			break;
		}
		if (nr == 0) {
			paragraphs = (char **)malloc(sizeof(char *));
			paragraphs[0] = (char *)malloc(buffer_size * sizeof(char));
			strcpy(paragraphs[0], paragraph);
		}
		else {

			char **aux = (char **)realloc(paragraphs, sizeof(char *) * (nr + 1));
			if (!aux) {
				exit(-1);
			}
			paragraphs = aux;
			paragraphs[nr] = (char *)malloc(buffer_size * sizeof(char));
			strcpy(paragraphs[nr], paragraph);
		}
		nr++;
		free(paragraph);
		paragraph = NULL;
	}
		
	int P = sysconf(_SC_NPROCESSORS_CONF) - 1; //numarul de threaduri disponibile 
	char *paragraph;

	for (index = 0; index < nr; index++) {
		char *final_paragraph = malloc(1);
		*final_paragraph = '\0';

		//paragraful care trebuie trimis catre procesare
		paragraph = (char *)malloc(strlen(paragraphs[index]) + 1);
		strcpy(paragraph, paragraphs[index]);

		int num_chunks;
		//este impartit paragraful in chunkuri de maxim 20 de linii fiecare
		//num_chunks - numarul de chunkuri obtinute la divizarea paragrafului
		char **paragraph_chunks = divide_paragraph(paragraph, &num_chunks);
	
		int i;

		int threads_num = MIN(P, num_chunks);

		int chunk = 0;
		
		int next_chunks = num_chunks;
		for (i = 0; i < num_chunks; i++) {
			chunk = i;
			//threads_num trebuie actualizat cu numarul de chunkuri care mai trebuie procesate
			threads_num = MIN(P, next_chunks);

			pthread_t processing_threads[threads_num];
			//printf("Paragraf trimis catre procesare: %s\n", paragraph_chunks[chunk]);
			int j;
			for (j = 0; j < threads_num; j++) {
				int r = pthread_create(&processing_threads[j], NULL, worker_data_processing_comedy, (void *)paragraph_chunks[chunk]);
		        if (r) {
		            printf("Eroare la asteptarea thread-ului %d\n", i);
		            exit(-1);
		   		}
		   		chunk++;
		   		next_chunks -= 1;
			}

			for (j = 0; j < threads_num; j++) {
				char *processed_paragraph = NULL;
		        int r = pthread_join(processing_threads[j], (void**)&processed_paragraph);
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
				i = chunk - 1;
		}

		MPI_Send(final_paragraph, strlen(final_paragraph) + 1, MPI_CHAR, MASTER, index, MPI_COMM_WORLD);
	}

	pthread_exit(NULL);
}


/*functia de thread executata de thread-ul reader din worker-ul SCI-FI*/
void *worker_sciFi_reader_f(void *arg) {
	char **paragraphs = NULL;
	int index = 0;
	int nr = 0;
	while(1) {
		char *paragraph;
		MPI_Status status;
		int buffer_size;
		MPI_Probe(MASTER, SCIFI_WORKER, MPI_COMM_WORLD, &status);
			
		MPI_Get_count(&status, MPI_CHAR, &buffer_size);
		
		MPI_Status status_recv;
		paragraph = (char *)malloc(buffer_size * sizeof(char));

		MPI_Recv(paragraph, buffer_size, MPI_CHAR, MASTER, SCIFI_WORKER, MPI_COMM_WORLD, &status_recv);
			
		if (strcmp(paragraph, "DONE") == 0) {
			break;
		}
		if (nr == 0) {
			paragraphs = (char **)malloc(sizeof(char *));
			paragraphs[0] = (char *)malloc(buffer_size * sizeof(char));
			strcpy(paragraphs[0], paragraph);
		}
		else {

			char **aux = (char **)realloc(paragraphs, sizeof(char *) * (nr + 1));
			if (!aux) {
				exit(-1);
			}
			paragraphs = aux;
			paragraphs[nr] = (char *)malloc(buffer_size * sizeof(char));
			strcpy(paragraphs[nr], paragraph);
		}
		nr++;
		free(paragraph);
		paragraph = NULL;
	}
		
	int P = sysconf(_SC_NPROCESSORS_CONF) - 1; //numarul de threaduri disponibile 
	char *paragraph;

	for (index = 0; index < nr; index++) {
		char *final_paragraph = malloc(1);
		*final_paragraph = '\0';

		//paragraful care trebuie trimis catre procesare
		paragraph = (char *)malloc(strlen(paragraphs[index]) + 1);
		strcpy(paragraph, paragraphs[index]);

		int num_chunks;
		char **paragraph_chunks = divide_paragraph(paragraph, &num_chunks);

		int threads_num = MIN(P, num_chunks);

		int i, chunk = 0;
		

		int next_chunks = num_chunks;
		for (i = 0; i < num_chunks; i++) {
			chunk = i;
			//threads_num trebuie actualizat cu numarul de chunkuri care mai trebuie procesate
			threads_num = MIN(P, next_chunks);

			pthread_t processing_threads[threads_num];
			int j;
			for (j = 0; j < threads_num; j++) {

				int r = pthread_create(&processing_threads[j], NULL, worker_data_processing_SciFi, (void *)paragraph_chunks[chunk]);
		        if (r) {
		            printf("Eroare la asteptarea thread-ului %d\n", i);
		            exit(-1);
		   		}
		   		chunk++;
		   		next_chunks -= 1;
			}

			for (j = 0; j < threads_num; j++) {
				char *processed_paragraph = NULL;
		        int r = pthread_join(processing_threads[j], (void**)&processed_paragraph);
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
				i = chunk - 1;
		}

		MPI_Send(final_paragraph, strlen(final_paragraph) + 1, MPI_CHAR, MASTER, index, MPI_COMM_WORLD);
	}

	pthread_exit(NULL);
}


/*functia de thread executata de thread-ul reader din worker-ul fantasy*/
void *worker_fantasy_reader_f(void *arg) {
	
	/*se face receive tuturor paragrafelor de tipul fantasy*/
	char **paragraphs = NULL;
	int index = 0;
	int nr = 0;
	while(1) {
		char *paragraph;
		MPI_Status status;
		int buffer_size;
		MPI_Probe(MASTER, FANTASY_WORKER, MPI_COMM_WORLD, &status);
			
		MPI_Get_count(&status, MPI_CHAR, &buffer_size);
		
		MPI_Status status_recv;
		paragraph = (char *)malloc(buffer_size * sizeof(char));
		MPI_Recv(paragraph, buffer_size, MPI_CHAR, MASTER, FANTASY_WORKER, MPI_COMM_WORLD, &status_recv);
			
		if (strcmp(paragraph, "DONE") == 0) {
			break;
		}
		if (nr == 0) {
			paragraphs = (char **)malloc(sizeof(char *));
			paragraphs[0] = (char *)malloc(buffer_size * sizeof(char));
			strcpy(paragraphs[0], paragraph);
		}
		else {

			char **aux = (char **)realloc(paragraphs, sizeof(char *) * (nr + 1));
			if (!aux) {
				exit(-1);
			}
			paragraphs = aux;
			paragraphs[nr] = (char *)malloc(buffer_size * sizeof(char));
			strcpy(paragraphs[nr], paragraph);
		}
		nr++;
		free(paragraph);
		paragraph = NULL;
	}

	int P = sysconf(_SC_NPROCESSORS_CONF) - 1; //numarul de threaduri disponibile 
	char *paragraph;

	for (index = 0; index < nr; index++) {
		char *final_paragraph = malloc(1);
		*final_paragraph = '\0';

		//paragraful care trebuie trimis catre procesare
		paragraph = (char *)malloc(strlen(paragraphs[index]) + 1);
		strcpy(paragraph, paragraphs[index]);

		int num_chunks;
		//imparte paragraful in chunk-uri de maxim 20 de linii fiecare
		char **paragraph_chunks = divide_paragraph(paragraph, &num_chunks);
	
		int i, chunk = 0;

		int threads_num = MIN(P, num_chunks);

		int next_chunks = num_chunks;
		for (i = 0; i < num_chunks; i++) {
			chunk = i;
			//threads_num trebuie actualizat cu numarul de chunkuri care mai trebuie procesate
			threads_num = MIN(P, next_chunks);

			pthread_t processing_threads[threads_num];

			int j;
			for (j = 0; j < threads_num; j++) {
				
				int r = pthread_create(&processing_threads[j], NULL, worker_data_processing_fantasy, (void *)paragraph_chunks[chunk]);
		        if (r) {
		            printf("Eroare la asteptarea thread-ului %d\n", i);
		            exit(-1);
		   		}
		   		chunk++;
		   		next_chunks -= 1;
			}

			for (j = 0; j < threads_num; j++) {
				char *processed_paragraph = NULL;
		        int r = pthread_join(processing_threads[j], (void**)&processed_paragraph);
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

			i = chunk - 1;
		}

		MPI_Send(final_paragraph, strlen(final_paragraph) + 1, MPI_CHAR, MASTER, index, MPI_COMM_WORLD);
	}

	pthread_exit(NULL);
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
    	long i;
    	for (i = 0; i < MASTER_THREADS; i++) {
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
		char *final_text = malloc(1);
    	*final_text = '\0';
    	int horror_par = 0;
    	int comedy_par = 0;
    	int sciFi_par = 0;
    	int fantasy_par = 0;

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

		//deschide fisierul de output
    	FILE *out_fptr = fopen(out_file, "w");
    	if (out_fptr == NULL) {
    		printf("Eroare la scrierea in fisier\n");
    		exit(-1);
    	}

    	//scrie textul final procesat in fisierul de output
		for (i = 0; i < num_paragraphs; i++) { //horror
			if (paragraphs_order[i] == 1) {
				char *append_str = (char *)malloc(strlen(horror_processed_paragraphs[horror_par]) + strlen("horror\n") + 2);
				sprintf(append_str, "horror\n%s\n", horror_processed_paragraphs[horror_par]);
				horror_par++;
				fprintf(out_fptr, "%s", append_str);
			}
			else if (paragraphs_order[i] == 2) { //comedy
				char *append_str = (char *)malloc(strlen(comedy_processed_paragraphs[comedy_par]) + strlen("comedy\n") + 2);
				sprintf(append_str, "comedy\n%s\n", comedy_processed_paragraphs[comedy_par]);
				comedy_par++;
				fprintf(out_fptr, "%s", append_str);
			}
			else if (paragraphs_order[i] == 4) { //fantasy
		
				char *append_str = (char *)malloc(strlen(fantasy_processed_paragraphs[fantasy_par]) + strlen("fantasy\n") + 2);
				sprintf(append_str, "fantasy\n%s\n", fantasy_processed_paragraphs[fantasy_par]);
				fantasy_par++;
				fprintf(out_fptr, "%s", append_str);
			}
			else if (paragraphs_order[i] == 3) { //scifi
				
				char *append_str = (char *)malloc(strlen(scifi_processed_paragraphs[sciFi_par]) + strlen("science-fiction\n") + 2);
				sprintf(append_str, "science-fiction\n%s\n", scifi_processed_paragraphs[sciFi_par]);
				sciFi_par++;
				fprintf(out_fptr, "%s", append_str);
			}
		}
    	
    	free(out_file);
    	out_file = NULL;
    	free(file_name);
    	file_name = NULL;
    	for (i = 0; i < num_horror; i++) {
    		free(horror_processed_paragraphs[i]);
    		horror_processed_paragraphs[i] = NULL;
    	}
    	free(horror_processed_paragraphs);
    	horror_processed_paragraphs = NULL;

    	for (i = 0; i < num_comedy; i++) {
    		free(comedy_processed_paragraphs[i]);
    		comedy_processed_paragraphs[i] = NULL;
    	}
    	free(comedy_processed_paragraphs);
    	comedy_processed_paragraphs = NULL;

    	for (i = 0; i < num_scifi; i++) {
    		free(scifi_processed_paragraphs[i]);
    		scifi_processed_paragraphs[i] = NULL;
    	}
    	free(scifi_processed_paragraphs);
    	scifi_processed_paragraphs = NULL;

    	for (i = 0; i < num_fantasy; i++) {
    		free(fantasy_processed_paragraphs[i]);
    		fantasy_processed_paragraphs[i] = NULL;
    	}
    	free(fantasy_processed_paragraphs);
    	fantasy_processed_paragraphs = NULL;

    	free(paragraphs_order);
    	paragraphs_order = NULL;
	}
    else {
    	
    	//in fiecare worker este pornit initial thread-ul 'receiver' care recepteaza datele trimise de master si porneste ulterior threadurile de procesare
        pthread_t reader;
        int r = 0;

        //fiecare thread executa prelucrarile aferente tipului de paragraf care ii este asociat
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