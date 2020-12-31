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
	for (i = 0; i < strlen(paragraph); i++) {
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
	return result;
}

/*functia de thread executata de thread-ul de procesare de pe worker-ul horror*/
void *worker_data_processing_comedy(void *arg) {
	char *paragraph = (char*) arg;
	char *processed_paragraph = (char *)malloc(strlen(paragraph) * sizeof(char));

	char *token = strtok(paragraph, " ");
	bool ok = false;
	while (token != NULL) {
		ok = false;
		char *new_token = (char *)malloc(strlen(token) * sizeof(char));
		strcpy(new_token, token);
		//fiecare litera de pe pozitie para este facuta majuscula
		int i;
		for (i = 1; i < strlen(new_token); i++) {
			if (token[i] != '\n' && i % 2 == 1) {
				new_token[i] = toupper(new_token[i]);
			}
			else if (token[i] == '\n') {
				ok = true;
				break;
			}
		}
		//s-a idenificat finalul de linie
		if (ok == true) {
			char *dest1 = (char *)malloc(sizeof(char) * i);
			strncpy(dest1, new_token, i);
			strcat(processed_paragraph, dest1);
			strcat(processed_paragraph, "\n");

			int j;
			//este extras primul cuvant de pe linie
			for (j = i + 2; j < strlen(new_token); j = j + 2) {
				new_token[j] = toupper(new_token[j]);
			}
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
	return result;
}

/*functia de thread executata de thread-ul de procesare de pe worker-ul horror*/
void *worker_data_processing_SciFi(void *arg) {
	//al saptelea cuvant de pe fiecare rand este inversat
	char *paragraph = (char *) arg;
	char *processed_paragraph = (char *)malloc(strlen(paragraph) * sizeof(char));

	char *rest = paragraph;
	//reentrant version of strtok
	char *line = strtok_r(paragraph, "\n", &rest);
	int count_words = 0;
	
	while (line != NULL) {
		count_words = 0;
		//line va reprezenta o linie din paragraf
		//desparte linia in cuvinte si gaseste al saptelea cuvant de pe linie
		char *line_copy  = strdup(line);
		char *copy = line_copy;
		char *word = strtok_r(line_copy, " ", &copy);
		strcat(processed_paragraph, word);
		strcat(processed_paragraph, " ");
		while (word != NULL) {
			
			word = strtok_r(NULL, " ", &copy);
			count_words++;
			if (word != NULL) {
				if (count_words == 6) {
					printf("al saptelea cuvant: %s\n", word);
					int i, length = strlen(word);
					char *reverse_word = (char *)malloc(strlen(word) * sizeof(char));
					for (i = 0; i < length; i++) {
						reverse_word[length - i - 1] = word[i];
					}
					strcat(processed_paragraph, reverse_word);
				}
				else {
					printf("cuvant: %s\n", word);
					strcat(processed_paragraph, word);
				}
				strcat(processed_paragraph, " ");
			}
		}
		line = strtok_r(NULL, "\n", &rest);
		if (line != NULL) {
			strcat(processed_paragraph, "\n");
		}
	}

	void *result = processed_paragraph;
	return result;
}

/*functia de thread executata de thread-ul de procesare de pe worker-ul horror*/
void *worker_data_processing_fantasy(void *arg) {
	char *paragraph = (char *) arg;
	char *processed_paragraph = (char *)malloc(strlen(paragraph) * sizeof(char));

	//prima litera a fiecarui cuvant trebuie facuta majuscula
	printf("textul care ajunge la thread: %s\n", paragraph);
	char *token = strtok(paragraph, " ");
	bool ok = false;
	int i;
	while (token != NULL) {
		ok = false;
		char *new_token = (char *)malloc(strlen(token) * sizeof(char));
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
	return result;
}


bool check_thread_start(int r, long worker_rank) {
	if (r != 0) {
    	printf("Eroare la crearea thread-ului de procesare in worker-ul %ld\n", worker_rank);
    	return false;
    }
    return true;
}


/*functia de thread executata de cele 4 thread-uri reader(cate unul pe fiecare worker)*/
void *worker_reader_f(void *arg) {
	long worker_rank = *(long*) arg;
	//face receive la paragraful care i-a fost trimis de catre thread-ul aferent tipului sau din master
	char *paragraph;
	MPI_Status status;
	int buffer_size;

	MPI_Probe(0, 0, MPI_COMM_WORLD, &status);
	MPI_Get_count(&status, MPI_CHAR, &buffer_size);

	paragraph = (char *)malloc(buffer_size * sizeof(char));
	MPI_Recv(paragraph, buffer_size, MPI_CHAR, MASTER, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

	int lines_count = get_num_lines(paragraph);
	//printf("Mesajul primit in worker-ul %ld este: %s; Are %d linii\n", worker_rank, paragraph, lines_count);
	//numara cate linii contine paragraful primit
	


	//porneste atatea thread-uri de cat este nevoie pt a procesa paragraful in functie de numarul de linii din paragraf
	int r;
	pthread_t processing_thread;
	if (worker_rank == 1) {
		r = pthread_create(&processing_thread, NULL, worker_data_processing_horror, (void *)paragraph);
		if (check_thread_start(r, worker_rank) == false) {
			exit(-1);
		}
		void *processed_paragraph_horror;
   		r = pthread_join(processing_thread, &processed_paragraph_horror);
    	if (r != 0) {
    		printf("Eroare la asteptarea thread-ului de procesare in worker-ul %ld\n", worker_rank);
    		exit(-1);
    	}
    	printf("Textul procesat in worker-ul %ld este:%s\n", worker_rank, (char*)processed_paragraph_horror);

	}
	else if (worker_rank == 2) {
		r = pthread_create(&processing_thread, NULL, worker_data_processing_comedy, (void *)paragraph);
		if (check_thread_start(r, worker_rank) == false) {
			exit(-1);
		}

		void *processed_paragraph_comedy;
    	r = pthread_join(processing_thread, &processed_paragraph_comedy);
    	if (r != 0) {
    		printf("Eroare la asteptarea thread-ului de procesare in worker-ul %ld\n", worker_rank);
    		exit(-1);
   		}
    	printf("Textul procesat in worker-ul %ld este:%s\n", worker_rank, (char*)processed_paragraph_comedy);
    
	}
	else if (worker_rank == 3) {
		r = pthread_create(&processing_thread, NULL, worker_data_processing_SciFi, (void *)paragraph);
		if (check_thread_start(r, worker_rank) == false) {
			exit(-1);
		}

		void *processed_paragraph_sci_fi;
    	r = pthread_join(processing_thread, &processed_paragraph_sci_fi);
    	if (r != 0) {
    		printf("Eroare la asteptarea thread-ului de procesare in worker-ul %ld\n", worker_rank);
    		exit(-1);
   		}
    	printf("Textul procesat in worker-ul %ld este:%s\n", worker_rank, (char*)processed_paragraph_sci_fi);
	}
	else if (worker_rank == 4) {
		r = pthread_create(&processing_thread, NULL, worker_data_processing_fantasy, (void *)paragraph);
		if (check_thread_start(r, worker_rank) == false) {
			exit(-1);
		}

		void *processed_paragraph_fantasy;
    	r = pthread_join(processing_thread, &processed_paragraph_fantasy);
    	if (r != 0) {
    		printf("Eroare la asteptarea thread-ului de procesare in worker-ul %ld\n", worker_rank);
    		exit(-1);
   		}
    	printf("Textul procesat in worker-ul %ld este:%s\n", worker_rank, (char*)processed_paragraph_fantasy);
	}
    
 
	//asteapta ca thread-urile care fac procesarea sa isi incheie treaba, apoi trimite inapoi la master paragraful intreg procesat

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
