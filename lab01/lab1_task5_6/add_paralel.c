#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_THREADS 8/*sysconf(_SC_NPROCESSORS_CONF)*/
#define MIN(a,b) (((a < b) ? a : b))

/*
    schelet pentru exercitiul 5
*/

int* arr;
int array_size;

void *f(void *arg) {
    long id = (long)arg; //identificatorul thread-ului(ID-ul)
    int start = id * (double)array_size/NUM_THREADS;
    int end = MIN ((id + 1) * (double)array_size/NUM_THREADS, array_size);

    for (int i = start; i < end; i++) {
         arr[i] += 100;
    }
    
    pthread_exit(NULL);
}


int main(int argc, char *argv[]) {
    if (argc < 2) {
        perror("Specificati dimensiunea array-ului\n");
        exit(-1);
    }

    array_size = atoi(argv[1]);

    arr = malloc(array_size * sizeof(int));
    for (int i = 0; i < array_size; i++) {
        arr[i] = i;
    }

    /*
    for (int i = 0; i < array_size; i++) {
        printf("%d", arr[i]);
        if (i != array_size - 1) {
            printf(" ");
        } else {
            printf("\n");
        }
    }
    */
    

    long i;
    pthread_t threads[NUM_THREADS];
    void *status;


    for (i = 0; i < NUM_THREADS; i++) {
        int r = pthread_create(&threads[i], NULL, f, (void *)i);

        if (r) {
            printf("Eroare la crearea thread-ului %ld\n", i);
            exit(-1);
        }
    }

    for (i = 0; i < NUM_THREADS; i++) {
        int r = pthread_join(threads[i], &status);

        if (r) {
            printf("Eroare la asteptarea thread-ului %ld\n", i);
            exit(-1);
        }
    }

    /*
    for (int i = 0; i < array_size; i++) {
        printf("%d", arr[i]);
        if (i != array_size - 1) {
            printf(" ");
        } else {
            printf("\n");
        }
    }
    */

  	pthread_exit(NULL);
}
