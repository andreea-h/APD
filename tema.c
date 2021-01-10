#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
 
#define MASTER 0
#define MASTER_THREADS 4
#define MAX_LENGTH 50
#define HORROR_WORKER 1
#define COMEDY_WORKER 2
#define SCIFI_WORKER 3
#define FANTASY_WORKER 4
#define PARAGRAPH_NR 5

pthread_mutex_t mutex;

char *file_name;
/*retine tipurile tuturor paragrafelor care apar in fisier in ordine*/
int *paragraphs_order;
int nr_total_paragraphs;
int capacity;

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

	/*este determinat numarul de paragrafe de tipul horror care se gasesc in fisier*/
	/*este retinuta o lista declarata la nivelul nodului master care memoreaza ordinea in care apar paragrafele in functie de tipurile lor*/
	int nr_paragraphs = 0;
	while ((result = getline(&line_buffer, &buffer_size, fptr)) != -1) {
		if (strcmp(line_buffer, "horror\n") == 0) {
			nr_paragraphs++;
			if (nr_total_paragraphs == 0) {
				paragraphs_order = (int *)malloc(PARAGRAPH_NR * sizeof(int));
				paragraphs_order[0] = 0;
				nr_total_paragraphs++;
				capacity = PARAGRAPH_NR;
			}
			else {
				if (nr_total_paragraphs == capacity) {
					paragraphs_order = (int *)realloc(paragraphs_order, 2 * capacity * sizeof(int));	
					capacity *= 2;
				}
				paragraphs_order[nr_total_paragraphs] = 0;
				nr_total_paragraphs++;
			}
		}
		if (strcmp(line_buffer, "comedy\n") == 0) {
			if (nr_total_paragraphs == 0) {
				paragraphs_order = (int *)malloc(PARAGRAPH_NR * sizeof(int));
				paragraphs_order[0] = 1;
				nr_total_paragraphs = 1;
				capacity = PARAGRAPH_NR;
			}
			else {
				if (nr_total_paragraphs == capacity) {
					paragraphs_order = (int *)realloc(paragraphs_order, 2 * capacity * sizeof(int));	
					capacity *= 2;
				}
				paragraphs_order[nr_total_paragraphs] = 1;
				nr_total_paragraphs++;
			}
		}
		if (strcmp(line_buffer, "fantasy\n") == 0) {
			if (nr_total_paragraphs == 0) {
				paragraphs_order = (int *)malloc(PARAGRAPH_NR * sizeof(int));
				paragraphs_order[0] = 3;
				nr_total_paragraphs = 1;
				capacity = PARAGRAPH_NR;
			}
			else {
				if (nr_total_paragraphs == capacity) {
					paragraphs_order = (int *)realloc(paragraphs_order, 2 * capacity * sizeof(int));	
					capacity *= 2;
				}
				paragraphs_order[nr_total_paragraphs] = 3;
				nr_total_paragraphs++;
			}
		}
		if (strcmp(line_buffer, "science-fiction\n") == 0) {
			if (nr_total_paragraphs == 0) {
				paragraphs_order = (int *)malloc(PARAGRAPH_NR * sizeof(int));
				paragraphs_order[0] = 2;
				nr_total_paragraphs = 1;
				capacity = PARAGRAPH_NR;
			}
			else {
				if (nr_total_paragraphs == capacity) {
					paragraphs_order = (int *)realloc(paragraphs_order, 2 * capacity * sizeof(int));	
					capacity *= 2;
				}
				paragraphs_order[nr_total_paragraphs] = 2;
				nr_total_paragraphs++;
			}
		}
	}

	//trimite numarul de paragrafe catre workerul aferent tipului de paragraf
	MPI_Send(&nr_paragraphs, 1, MPI_INT, HORROR_WORKER, HORROR_WORKER, MPI_COMM_WORLD);

	//reseteaza fptr la inceputul fisierului
	fseek(fptr, 0, SEEK_SET);

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
						paragraph = (char *)realloc(paragraph, strlen(paragraph) + strlen(line_buffer) + 2);
						strcat(paragraph, line_buffer);
					}


					//este identificat finalul paragrafului
					if (paragraph[strlen(paragraph) - 2] == '\n') {
						paragraph[strlen(paragraph) - 1] = '\0';
						check_paragraph_end = true;
						//s-a citit un paragraf de tipul horror
						//printf("[MASTER]Paragraf horror: %s", paragraph);
			
						MPI_Send(paragraph, strlen(paragraph) + 1, MPI_CHAR, HORROR_WORKER, HORROR_WORKER, MPI_COMM_WORLD);
						free(paragraph);
						paragraph = NULL;

						//primeste paragraful procesat de la worker
						/*char *processed_paragraph;
						MPI_Status status;
						int buffer_size;

						MPI_Probe(HORROR_WORKER, HORROR_WORKER, MPI_COMM_WORLD, &status);
							
						MPI_Get_count(&status, MPI_CHAR, &buffer_size);
						
						processed_paragraph = (char *)malloc(buffer_size * sizeof(char));
						MPI_Recv(processed_paragraph, buffer_size, MPI_CHAR, HORROR_WORKER, nr_total_paragraphs, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
						printf("[HORROR-MASTER]\n%s\n", processed_paragraph);*/
					}
				} 
				else {
					MPI_Send(paragraph, strlen(paragraph) + 1, MPI_CHAR, HORROR_WORKER, HORROR_WORKER, MPI_COMM_WORLD);
					free(paragraph);
					paragraph = NULL;

					//primeste paragraful procesat de la worker
					//primeste paragraful procesat de la worker
					/*char *processed_paragraph;
					MPI_Status status;
					int buffer_size;

					MPI_Probe(HORROR_WORKER, HORROR_WORKER, MPI_COMM_WORLD, &status);
					
					MPI_Get_count(&status, MPI_CHAR, &buffer_size);
						
					processed_paragraph = (char *)malloc(buffer_size * sizeof(char));
					MPI_Recv(processed_paragraph, buffer_size, MPI_CHAR, HORROR_WORKER, nr_total_paragraphs, MPI_COMM_WORLD, MPI_STATUS_IGNORE);	
					printf("[HORROR-MASTER]\n%s\n", processed_paragraph);*/

					break;
				}
				
			}
			if (check_paragraph_end == true) {
				check_paragraph_end = false;
			}
		}
	}
	

	//fclose(fptr);
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

	int nr_paragraphs = 0;
	while ((result = getline(&line_buffer, &buffer_size, fptr)) != -1) {
		if (strcmp(line_buffer, "comedy\n") == 0) {
			nr_paragraphs++;
		}
	}
	MPI_Send(&nr_paragraphs, 1, MPI_INT, COMEDY_WORKER, COMEDY_WORKER, MPI_COMM_WORLD);

	//reseteaza fptr la inceputul fisierului
	fseek(fptr, 0, SEEK_SET);

	while ((result = getline(&line_buffer, &buffer_size, fptr)) != -1) {
		if (strcmp(line_buffer, "comedy\n") == 0) {
			//citeste continutul paragrafului si trimite-l catre workerul 1
			while (check_paragraph_end == false) {
				result = getline(&line_buffer, &buffer_size, fptr);
				if (result != -1) {
					if (paragraph == NULL) {
						paragraph = (char *)malloc((strlen(line_buffer) + 1) * sizeof(char));
						strcpy(paragraph, line_buffer);
					}
					else {
						paragraph = (char *)realloc(paragraph, strlen(paragraph) + strlen(line_buffer) + 2);
						strcat(paragraph, line_buffer);
					}

					if (paragraph[strlen(paragraph) - 2] == '\n') {
						paragraph[strlen(paragraph) - 1] = '\0';
						check_paragraph_end = true;
						//s-a citit un paragraf de tipul comedy
				
						MPI_Send(paragraph, strlen(paragraph) + 1, MPI_CHAR, COMEDY_WORKER, COMEDY_WORKER, MPI_COMM_WORLD);
						free(paragraph);
						paragraph = NULL;
						//primeste paragraful procesat de la worker
						/*char *processed_paragraph;
						MPI_Status status;
						int buffer_size;

						MPI_Probe(COMEDY_WORKER, COMEDY_WORKER, MPI_COMM_WORLD, &status);
								
						MPI_Get_count(&status, MPI_CHAR, &buffer_size);
							
						processed_paragraph = (char *)malloc(buffer_size * sizeof(char));
						MPI_Recv(processed_paragraph, buffer_size, MPI_CHAR, COMEDY_WORKER, nr_total_paragraphs, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
						printf("[COMEDY-MASTER]\n%s\n", processed_paragraph);*/
					}

				}
				else {

					MPI_Send(paragraph, strlen(paragraph) + 1, MPI_CHAR, COMEDY_WORKER, COMEDY_WORKER, MPI_COMM_WORLD);
					free(paragraph);
					paragraph = NULL;

					//primeste paragraful procesat de la worker
					/*char *processed_paragraph;
					MPI_Status status;
					int buffer_size;

					MPI_Probe(COMEDY_WORKER, COMEDY_WORKER, MPI_COMM_WORLD, &status);
							
					MPI_Get_count(&status, MPI_CHAR, &buffer_size);
						
					processed_paragraph = (char *)malloc(buffer_size * sizeof(char));
					MPI_Recv(processed_paragraph, buffer_size, MPI_CHAR, COMEDY_WORKER, nr_total_paragraphs, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
					printf("[COMEDY-MASTER]\n%s\n", processed_paragraph);*/		

					break;
				}
			}
			if (check_paragraph_end == true) {
				check_paragraph_end = false;
			}
		}
	}



	//fclose(fptr);
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

	int nr_paragraphs = 0;
	while ((result = getline(&line_buffer, &buffer_size, fptr)) != -1) {
		if (strcmp(line_buffer, "science-fiction\n") == 0) {
			nr_paragraphs++;
		}
	}
	MPI_Send(&nr_paragraphs, 1, MPI_INT, SCIFI_WORKER, SCIFI_WORKER, MPI_COMM_WORLD);

	//reseteaza fptr la inceputul fisierului
	fseek(fptr, 0, SEEK_SET);

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
						paragraph = (char *)realloc(paragraph, strlen(paragraph) + strlen(line_buffer) + 2);
						strcat(paragraph, line_buffer);
					}

					if (paragraph[strlen(paragraph) - 2] == '\n') {
						paragraph[strlen(paragraph) - 1] = '\0';
						check_paragraph_end = true;
						//s-a citit un paragraf de tipul scifi
						//printf("[MASTER]Paragraf scifi: %s", paragraph);
					
						MPI_Send(paragraph, strlen(paragraph) + 1, MPI_CHAR, SCIFI_WORKER, SCIFI_WORKER, MPI_COMM_WORLD);
						free(paragraph);
						paragraph = NULL;

						//primeste paragraful procesat de la worker
						/*char *processed_paragraph;
						MPI_Status status;
						int buffer_size;

						MPI_Probe(SCIFI_WORKER, SCIFI_WORKER, MPI_COMM_WORLD, &status);
								
						MPI_Get_count(&status, MPI_CHAR, &buffer_size);
							
						processed_paragraph = (char *)malloc(buffer_size * sizeof(char));
						MPI_Recv(processed_paragraph, buffer_size, MPI_CHAR, SCIFI_WORKER, nr_total_paragraphs, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
						printf("[SCI-FI MASTER]\n%s\n", processed_paragraph);*/		
					}
				}
				else {
					//printf("PARAGRAF scifi:%s", paragraph);

					MPI_Send(paragraph, strlen(paragraph) + 1, MPI_CHAR, SCIFI_WORKER, SCIFI_WORKER, MPI_COMM_WORLD);
					free(paragraph);
					paragraph = NULL;

					//primeste paragraful procesat de la worker
					/*char *processed_paragraph;
					MPI_Status status;
					int buffer_size;

					MPI_Probe(SCIFI_WORKER, SCIFI_WORKER, MPI_COMM_WORLD, &status);
							
					MPI_Get_count(&status, MPI_CHAR, &buffer_size);
						
					processed_paragraph = (char *)malloc(buffer_size * sizeof(char));
					MPI_Recv(processed_paragraph, buffer_size, MPI_CHAR, SCIFI_WORKER, nr_total_paragraphs, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
					printf("[SCI-FI MASTER]\n%s\n", processed_paragraph);*/		

					break;
				}
			}
			if (check_paragraph_end == true) {
				check_paragraph_end = false;
			}
		}
	}
	
	//fclose(fptr);
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

	int nr_paragraphs = 0;
	while ((result = getline(&line_buffer, &buffer_size, fptr)) != -1) {
		if (strcmp(line_buffer, "fantasy\n") == 0) {
			nr_paragraphs++;
		}
	}
	MPI_Send(&nr_paragraphs, 1, MPI_INT, FANTASY_WORKER, FANTASY_WORKER, MPI_COMM_WORLD);

	//reseteaza fptr la inceputul fisierului
	fseek(fptr, 0, SEEK_SET);


	while ((result = getline(&line_buffer, &buffer_size, fptr)) != -1) {
		if (strcmp(line_buffer, "fantasy\n") == 0) {
			//citeste continutul paragrafului si trimite-l catre workerul 1
			while (check_paragraph_end == false) {
				result = getline(&line_buffer, &buffer_size, fptr);
				if (result != -1) {
					if (paragraph == NULL) {
						paragraph = (char *)malloc((strlen(line_buffer) + 1) * sizeof(char));
						strcpy(paragraph, line_buffer);
					}
					else {
						paragraph = (char *)realloc(paragraph, strlen(paragraph) + strlen(line_buffer) + 2);
						strcat(paragraph, line_buffer);
					}

					if (paragraph[strlen(paragraph) - 2] == '\n') {
						paragraph[strlen(paragraph) - 1] = '\0';
						check_paragraph_end = true;

						//printf("PARAGRAF FANTASY: %s\n", paragraph);
						//s-a citit un paragraf de tipul fantasy
						MPI_Send(paragraph, strlen(paragraph) + 1, MPI_CHAR, FANTASY_WORKER, FANTASY_WORKER, MPI_COMM_WORLD);
						free(paragraph);
						paragraph = NULL;

						//primeste paragraful procesat de la worker
						/*char *processed_paragraph;
						MPI_Status status;
						int buffer_size;

						MPI_Probe(FANTASY_WORKER, FANTASY_WORKER, MPI_COMM_WORLD, &status);
								
						MPI_Get_count(&status, MPI_CHAR, &buffer_size);
							
						processed_paragraph = (char *)malloc(buffer_size * sizeof(char));
						MPI_Recv(processed_paragraph, buffer_size, MPI_CHAR, FANTASY_WORKER, nr_total_paragraphs, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
						printf("[FANTASY-MASTER]\n%s\n", processed_paragraph);	*/	
					}
				}
				else {

					//printf("PARAGRAF:%s", paragraph);
					MPI_Send(paragraph, strlen(paragraph) + 1, MPI_CHAR, FANTASY_WORKER, FANTASY_WORKER, MPI_COMM_WORLD);
					free(paragraph);
					paragraph = NULL;

					//primeste paragraful procesat de la worker
					/*char *processed_paragraph;
					MPI_Status status;
					int buffer_size;

					MPI_Probe(FANTASY_WORKER, FANTASY_WORKER, MPI_COMM_WORLD, &status);
								
					MPI_Get_count(&status, MPI_CHAR, &buffer_size);
							
					processed_paragraph = (char *)malloc(buffer_size * sizeof(char));
					MPI_Recv(processed_paragraph, buffer_size, MPI_CHAR, FANTASY_WORKER, nr_total_paragraphs, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
					printf("[FANTASY-MASTER]\n%s\n", processed_paragraph);*/

					break;
				}
			}
			if (check_paragraph_end == true) {
				check_paragraph_end = false;
			}
		}
	}


	//fclose(fptr);
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
	for (i = 0; i < strlen(paragraph); i++) {
		if (isLetter(paragraph[i]) == true) {
			//verifaca daca litera este consoana
			if (isVowel(paragraph[i]) == false) {
				k++;
			}
		}
	}
	processed_paragraph = (char *)malloc((strlen(paragraph) + k) * sizeof(char));
	for (i = 0; i <= strlen(paragraph); i++) {
		processed_paragraph[strlen(processed_paragraph)] = paragraph[i];
		if (isLetter(paragraph[i]) == true) {
			//verifaca daca litera este consoana
			if (isVowel(paragraph[i]) == false) {
				//aloca un char in plus pentru consoana care va fi introdusa
				processed_paragraph[strlen(processed_paragraph)] = tolower(paragraph[i]);
			}
		}
	}
	void *result = processed_paragraph;
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

	void *result = processed_paragraph;
	return processed_paragraph;
}

/*functia de thread executata de thread-ul de procesare de pe worker-ul horror*/
void *worker_data_processing_SciFi(void *arg) {
	//al saptelea cuvant de pe fiecare rand este inversat
	char *paragraph = (char *) arg;
	char *processed_paragraph = (char *)malloc((strlen(paragraph) + 1) * sizeof(char));

	char *rest = paragraph;
	//reentrant version of strtok
	char *line = strtok_r(paragraph, "\n", &rest);
	int count_words = 0;
	
	while (line != NULL) {
		//printf("LINIE PROCESATA:%s\n", line);
		count_words = 1;
		//line va reprezenta o linie din paragraf
		//desparte linia in cuvinte si gaseste al saptelea cuvant de pe linie
		char *line_copy = (char *)malloc((strlen(line) + 1) * sizeof(char));
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
					char *reverse_word = (char *)malloc(length * sizeof(char));

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

/*functia de thread executata de thread-ul de procesare de pe worker-ul horror*/
void *worker_data_processing_fantasy(void *arg) {
	char *paragraph = (char *) arg;
	char *processed_paragraph = (char *)malloc((strlen(paragraph) + 1) * sizeof(char));

	//prima litera a fiecarui cuvant trebuie facuta majuscula
	char *token = strtok(paragraph, " ");
	bool ok = false;
	int i;
	while (token != NULL) {
		ok = false;
		char *new_token = (char *)malloc((strlen(token) + 1) * sizeof(char));
		strcpy(new_token, token);
		new_token[0] = toupper(new_token[0]);
		for (i = 1; i < strlen(token); i++) {
			if (token[i] == '\n') {
				ok = true;
				break;
			}
		}
		//separa ultimul cuvant de pe rand si primul cuvant de pe urmatorul rand
		if (ok == true) {
			char *dest1 = (char *)malloc(sizeof(char) * i);
			strncpy(dest1, new_token, i);
			strcat(processed_paragraph, dest1);
			strcat(processed_paragraph, "\n");

			int j;
			//este extras primul cuvant de pe linie
			new_token[i + 1] = toupper(new_token[i + 1]);
			char *dest2 = (char *)malloc(sizeof(char) * (strlen(token) - i));
			strcpy(dest2, new_token + i + 1);
			strcat(processed_paragraph, dest2);
			if (token[strlen(token)] != '\n') {
				strcat(processed_paragraph, " ");
			}	

			token = strtok(NULL, " ");
			continue;
		}
		if (processed_paragraph == NULL) { 
			strcpy(processed_paragraph, new_token);
		}
		else {
			strcat(processed_paragraph, new_token);
		}
		token = strtok(NULL, " ");
		if (token != NULL && token[strlen(token)] != '\n') {
			strcat(processed_paragraph, " ");
		}
	}


	void *result = processed_paragraph;

	return processed_paragraph;
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
	/*in thread-ul receiver de pe fiecare worker este primit mai intai de la master numarul de paragrafe 
	de tipul asociat lui pe care urmeaza sa le primeasca de la master*/

	int num_paragraphs;
	MPI_Recv(&num_paragraphs, 1, MPI_INT, MASTER, HORROR_WORKER, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	//printf("[HORROR] Numar de paragrafe asteptate in worker: %d\n", num_paragraphs);

	int i;
	for (i = 0; i < num_paragraphs; i++) {
		//analizeaza numarul de linii din fiecare paragraf si porneste threadurile care fac procesarea paragrafului curent i
		//face receive la paragraful care i-a fost trimis de catre thread-ul aferent tipului sau din master
		char *paragraph;
		MPI_Status status;
		int buffer_size;

		MPI_Probe(MASTER, HORROR_WORKER, MPI_COMM_WORLD, &status);
			
		MPI_Get_count(&status, MPI_CHAR, &buffer_size);
		
		MPI_Status status_recv;
		paragraph = (char *)malloc(buffer_size * sizeof(char));
		MPI_Recv(paragraph, buffer_size, MPI_CHAR, MASTER, HORROR_WORKER, MPI_COMM_WORLD, &status_recv);


		//printf("horror\n");
		//printf("%s\n", paragraph);
	
		int lines_count = get_num_lines(paragraph);
		//printf("Mesajul primit in worker-ul %ld este: %s; Are %d linii\n", worker_rank, paragraph, lines_count);
		//numara cate linii contine paragraful primit


		//porneste atatea thread-uri de cat este nevoie pt a procesa paragraful in functie de numarul de linii din paragraf

		//valoarea intoarsa la join reprezinta paragraful procesat conform cu tipul acestuia
		//ulterior paragraful procesat conform cu tipul acestuia este trimis catre Master
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
	    //printf("Textul procesat in worker-ul HORROR este:%s\n", (char*)processed_paragraph_horror);
	    //trimite paragraful procesat catre master
	    MPI_Send(processed_paragraph_horror, strlen(processed_paragraph_horror) + 1, MPI_CHAR, MASTER, i, MPI_COMM_WORLD);	
	 	//free(processed_paragraph_horror);
	 	//free(paragraph);
	 	//paragraph = NULL;
		//asteapta ca thread-urile care fac procesarea sa isi incheie treaba, apoi trimite inapoi la master paragraful intreg procesat
	}


	//printf("S-au procesat si trimis toate paragrafele horror\n");
	pthread_exit(NULL);
}

/*functia de thread executata de thread-ul reader din worker-ul comedy*/
void *worker_comedy_reader_f(void *arg) {
	
	/*in thread-ul receiver de pe fiecare worker este primit mai intai de la master numarul de paragrafe 
	de tipul asociat lui pe care urmeaza sa le primeasca de la master*/
	int num_paragraphs;
	MPI_Recv(&num_paragraphs, 1, MPI_INT, MASTER, COMEDY_WORKER, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	//printf("[COMEDY] Numar de paragrafe asteptate in worker: %d\n", num_paragraphs);

	int i;
	for (i = 0; i < num_paragraphs; i++) {
		//analizeaza numarul de linii din fiecare paragraf si porneste threadurile care fac procesarea paragrafului curent i
				//face receive la paragraful care i-a fost trimis de catre thread-ul aferent tipului sau din master
		char *paragraph;
		MPI_Status status;
		int buffer_size;

		MPI_Probe(MASTER, COMEDY_WORKER, MPI_COMM_WORLD, &status);
			
		MPI_Get_count(&status, MPI_CHAR, &buffer_size);
		
		MPI_Status status_recv;
		paragraph = (char *)malloc(buffer_size * sizeof(char));
		MPI_Recv(paragraph, buffer_size, MPI_CHAR, MASTER, COMEDY_WORKER, MPI_COMM_WORLD, &status_recv);

	//	printf("comedy\n");
	//	printf("%s\n", paragraph);
	
		int lines_count = get_num_lines(paragraph);
		//printf("Mesajul primit in worker-ul %ld este: %s; Are %d linii\n", worker_rank, paragraph, lines_count);
		//numara cate linii contine paragraful primit

		//porneste atatea thread-uri de cat este nevoie pt a procesa paragraful in functie de numarul de linii din paragraf

		//valoarea intoarsa la join reprezinta paragraful procesat conform cu tipul acestuia
		//ulterior paragraful procesat conform cu tipul acestuia este trimis catre Master
		int r;
		pthread_t processing_thread;
		r = pthread_create(&processing_thread, NULL, worker_data_processing_comedy, (void *)paragraph);
		if (check_thread_start(r, COMEDY_WORKER) == false) {
				exit(-1);
		}

		char *processed_paragraph_comedy = NULL;
	    r = pthread_join(processing_thread, (void**)&processed_paragraph_comedy);
	    if (r != 0) {
	    	printf("Eroare la asteptarea thread-ului de procesare in worker-ul %d\n", COMEDY_WORKER);
	    	exit(-1);
	   	}
	    //printf("Textul procesat in worker-ul COMEDY este:%s\n", (char*)processed_paragraph_comedy);

	    //trimite paragraful procesat catre master
	    MPI_Send(processed_paragraph_comedy, strlen(processed_paragraph_comedy) + 1, MPI_CHAR, MASTER, i, MPI_COMM_WORLD);
		//free(processed_paragraph_comedy);			
	}

	//printf("S-au procesat si trimis toate paragrafele comedy\n");
	//asteapta ca thread-urile care fac procesarea sa isi incheie treaba, apoi trimite inapoi la master paragraful intreg procesat
	pthread_exit(NULL);
}


/*functia de thread executata de thread-ul reader din worker-ul SCI-FI*/
void *worker_sciFi_reader_f(void *arg) {
	
	/*in thread-ul receiver de pe fiecare worker este primit mai intai de la master numarul de paragrafe 
	de tipul asociat lui pe care urmeaza sa le primeasca de la master*/
	int num_paragraphs;
	MPI_Recv(&num_paragraphs, 1, MPI_INT, MASTER, SCIFI_WORKER, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	//printf("[SCIFI] Numar de paragrafe asteptate in worker: %d\n", num_paragraphs);

	int i;
	for (i = 0; i < num_paragraphs; i++) {
		//analizeaza numarul de linii din fiecare paragraf si porneste threadurile care fac procesarea paragrafului curent i
				//face receive la paragraful care i-a fost trimis de catre thread-ul aferent tipului sau din master
		char *paragraph;
		MPI_Status status;
		int buffer_size;

		MPI_Probe(MASTER, SCIFI_WORKER, MPI_COMM_WORLD, &status);
			
		MPI_Get_count(&status, MPI_CHAR, &buffer_size);
		
		MPI_Status status_recv;
		paragraph = (char *)malloc(buffer_size * sizeof(char));
		MPI_Recv(paragraph, buffer_size, MPI_CHAR, MASTER, SCIFI_WORKER, MPI_COMM_WORLD, &status_recv);

		//printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!science-finction\n");
		//printf("sci_fi\n%s\n", paragraph);
	
		//int lines_count = get_num_lines(paragraph);
		//printf("Mesajul primit in worker-ul %d este: %s; Are %d linii\n", SCIFI_WORKER, paragraph, lines_count);
		//numara cate linii contine paragraful primit

		//porneste atatea thread-uri de cat este nevoie pt a procesa paragraful in functie de numarul de linii din paragraf

		//valoarea intoarsa la join reprezinta paragraful procesat conform cu tipul acestuia
		//ulterior paragraful procesat conform cu tipul acestuia este trimis catre Master
		int r;
		pthread_t processing_thread;
		r = pthread_create(&processing_thread, NULL, worker_data_processing_SciFi, (void *)paragraph);
		if (check_thread_start(r, SCIFI_WORKER) == false) {
				exit(-1);
		}
		//printf("functia de creare s-a incheiat cu succes\n");

		char *processed_paragraph_sci_fi = NULL;
	    r = pthread_join(processing_thread, (void**)&processed_paragraph_sci_fi);
	    if (r != 0) {
	    	printf("Eroare la asteptarea thread-ului de procesare in worker-ul %d\n", SCIFI_WORKER);
	    	exit(-1);
	   	}

	  // 	printf("TEXT:::::%s\n", processed_paragraph_sci_fi);
	   //	printf("DIMENSIUNE TEXT PROCESAT: %ld\n", strlen(processed_paragraph_sci_fi));

	    //trimite paragraful procesat catre master
	    MPI_Send(processed_paragraph_sci_fi, strlen(processed_paragraph_sci_fi) + 1, MPI_CHAR, MASTER, i, MPI_COMM_WORLD);
		//free(processed_paragraph_sci_fi);

		//printf("Textul procesat in worker-ul SCI-FI este:%s\n", (char*)processed_paragraph_sci_fi);
	}
	    
	//asteapta ca thread-urile care fac procesarea sa isi incheie treaba, apoi trimite inapoi la master paragraful intreg procesat
	//printf("S-au procesat si trimis toate paragrafele sci-fi\n");
	pthread_exit(NULL);
}


/*functia de thread executata de thread-ul reader din worker-ul fantasy*/
void *worker_fantasy_reader_f(void *arg) {
	
	/*in thread-ul receiver de pe fiecare worker este primit mai intai de la master numarul de paragrafe 
	de tipul asociat lui pe care urmeaza sa le primeasca de la master*/
	int num_paragraphs;
	MPI_Recv(&num_paragraphs, 1, MPI_INT, MASTER, FANTASY_WORKER, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	//printf("[FANTASY] Numar de paragrafe asteptate in worker: %d\n", num_paragraphs);

	int i;
	for (i = 0; i < num_paragraphs; i++) {
		//analizeaza numarul de linii din fiecare paragraf si porneste threadurile care fac procesarea paragrafului curent i
				//face receive la paragraful care i-a fost trimis de catre thread-ul aferent tipului sau din master
		char *paragraph;
		MPI_Status status;
		int buffer_size;

		MPI_Probe(MASTER, FANTASY_WORKER, MPI_COMM_WORLD, &status);
			
		MPI_Get_count(&status, MPI_CHAR, &buffer_size);

		MPI_Status status_recv;
		paragraph = (char *)malloc(buffer_size * sizeof(char));
		MPI_Recv(paragraph, buffer_size, MPI_CHAR, MASTER, FANTASY_WORKER, MPI_COMM_WORLD, &status_recv);

		//printf("fantasy\n");
		//printf("%s\n", paragraph);
	
		int lines_count = get_num_lines(paragraph);
		//printf("Mesajul primit in worker-ul %ld este: %s; Are %d linii\n", worker_rank, paragraph, lines_count);
		//numara cate linii contine paragraful primit

		//porneste atatea thread-uri de cat este nevoie pt a procesa paragraful in functie de numarul de linii din paragraf

		//valoarea intoarsa la join reprezinta paragraful procesat conform cu tipul acestuia
		//ulterior paragraful procesat conform cu tipul acestuia este trimis catre Master
		int r;
		pthread_t processing_thread;
		r = pthread_create(&processing_thread, NULL, worker_data_processing_fantasy, (void *)paragraph);
		if (check_thread_start(r, FANTASY_WORKER) == false) {
				exit(-1);
		}

		char *processed_paragraph_fantasy;
	    r = pthread_join(processing_thread, (void**)&processed_paragraph_fantasy);
	    if (r != 0) {
	    	printf("Eroare la asteptarea thread-ului de procesare in worker-ul %d\n", FANTASY_WORKER);
	    	exit(-1);
	   	}
	   //printf("Textul procesat in worker-ul FANTASY este:%s\n", (char*)processed_paragraph_fantasy);

	    //trimite paragraful procesat catre master
	    MPI_Send(processed_paragraph_fantasy, strlen(processed_paragraph_fantasy) + 1, MPI_CHAR, MASTER, i, MPI_COMM_WORLD);
	  // free(processed_paragraph_fantasy);
	}
	    
	//asteapta ca thread-urile care fac procesarea sa isi incheie treaba, apoi trimite inapoi la master paragraful intreg procesat
	//printf("S-au procesat si trimis toate paragrafele fantasy\n");
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

    if (rank == MASTER) {
    	printf("\n");
    	
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

    	int horror_par = 0;
    	int comedy_par = 0;
    	int sciFi_par = 0;
    	int fantasy_par = 0;

    	for (i = 0; i < nr_total_paragraphs; i++) {
    		printf("%d\n", paragraphs_order[i]);
    	}

    	//primeste paragrafele procesate de la workeri	
    	for (i = 0; i < nr_total_paragraphs; i++) {
    		if (paragraphs_order[i] == 0) { //asteapta un paragraf horror procesat
    			char *processed_paragraph;
				MPI_Status status;
				int buffer_size;

				MPI_Probe(HORROR_WORKER, horror_par, MPI_COMM_WORLD, &status);
								
				MPI_Get_count(&status, MPI_CHAR, &buffer_size);
							
				processed_paragraph = (char *)malloc(buffer_size * sizeof(char));
				MPI_Recv(processed_paragraph, buffer_size, MPI_CHAR, HORROR_WORKER, horror_par, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				printf("horror\n%s\n", processed_paragraph);
				horror_par++;
    		}
    		else if (paragraphs_order[i] == 1) { //asteapta un paragraf comedy procesat
    			char *processed_paragraph;
				MPI_Status status;
				int buffer_size;

				MPI_Probe(COMEDY_WORKER, comedy_par, MPI_COMM_WORLD, &status);
								
				MPI_Get_count(&status, MPI_CHAR, &buffer_size);
							
				processed_paragraph = (char *)malloc(buffer_size * sizeof(char));
				MPI_Recv(processed_paragraph, buffer_size, MPI_CHAR, COMEDY_WORKER, comedy_par, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				printf("comedy\n%s\n", processed_paragraph);
				comedy_par++;
    		}
    		else if (paragraphs_order[i] == 2) { //asteapta un paragraf sci-fi procesat
    			char *processed_paragraph;
				MPI_Status status;
				int buffer_size;

				MPI_Probe(SCIFI_WORKER, sciFi_par, MPI_COMM_WORLD, &status);
								
				MPI_Get_count(&status, MPI_CHAR, &buffer_size);
							
				processed_paragraph = (char *)malloc(buffer_size * sizeof(char));
				MPI_Recv(processed_paragraph, buffer_size, MPI_CHAR, SCIFI_WORKER, sciFi_par, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				printf("science-fiction\n%s\n", processed_paragraph);
				sciFi_par++;
    		}
    		else if (paragraphs_order[i] == 3) { //asteapta un paragraf fantasy procesat
    			char *processed_paragraph;
				MPI_Status status;
				int buffer_size;

				MPI_Probe(FANTASY_WORKER, fantasy_par, MPI_COMM_WORLD, &status);
								
				MPI_Get_count(&status, MPI_CHAR, &buffer_size);
							
				processed_paragraph = (char *)malloc(buffer_size * sizeof(char));
				MPI_Recv(processed_paragraph, buffer_size, MPI_CHAR, FANTASY_WORKER, fantasy_par, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				printf("fantasy\n%s\n", processed_paragraph);
				fantasy_par++;
    		}
    	}
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