#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_THREADS 2

void *f1(void *arg) {
  	long id = (long)arg;
  	printf("Hello World din thread-ul %ld!\n", id);
  	pthread_exit(NULL);
}

//calculeaza 20!
void *f2(void *arg) {
  	unsigned long int result = 1;
  	int i;
  	for (i = 1; i <= 20; i++) {
  		result *= i;
  	}
  	printf("***** 20! = %ld\n", result);
  	pthread_exit(NULL);
}

void *f3(void *arg) {
    long id = (long)arg;
    printf("Buna ziua din thread-ul %ld!\n", id);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
	pthread_t threads[NUM_THREADS];
  	int r;
  	long id;
  	void *status;

  	for (id = 0; id < NUM_THREADS; id++) {
  		if (id == 0) {
  			r = pthread_create(&threads[id], NULL, f1, (void *)id);
  		}
		
		if (id == 1) {
			r = pthread_create(&threads[id], NULL, f2, (void *)id);
		}

		if (r) {
	  		printf("Eroare la crearea thread-ului %ld\n", id);
	  		exit(-1);
		}
  	}

  	for (id = 0; id < NUM_THREADS; id++) {
		r = pthread_join(threads[id], &status);

		if (r) {
	  		printf("Eroare la asteptarea thread-ului %ld\n", id);
	  		exit(-1);
		}
  	}

  	pthread_exit(NULL);
}
