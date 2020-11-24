#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
 
#define NUM_THREADS 5
#define SIZE 12

#define MIN(x, y) (((x) < (y)) ? (x) : (y))

int v[] = {9, 4, 2, 7, 3, 5, 6, 11, 10, 13, 12, 8};
int v_copy[] = {9, 4, 2, 7, 3, 5, 6, 11, 10, 13, 12, 8};
int result[SIZE];
int ranks[SIZE];

pthread_barrier_t barrier;

void *f(void *arg)
{
    int thread_id = *(int*) arg;
    int i, j;

    int start = thread_id * (double) SIZE / NUM_THREADS;
    int end = MIN((thread_id + 1) * (double) SIZE / NUM_THREADS, SIZE);

    for (i = start; i < end; i++) {
        int rank = 0;
        for (j = 0; j < SIZE; j++) {
            if (v[j] <= v[i] && i != j) {
                rank++;
            }
        }
        ranks[i] = rank;
    }

    pthread_barrier_wait(&barrier);

    if (thread_id == 0) {
        for (i = 0; i < SIZE; i++) {
            int aux = v_copy[i];
            v[ranks[i]] = aux;
        }
    }

    pthread_exit(NULL);
}
 

int main(int argc, char *argv[])
{
    pthread_t threads[NUM_THREADS];
    int r, i;
    long id;
    void *status;
    int arguments[NUM_THREADS];

    pthread_barrier_init(&barrier, NULL, NUM_THREADS);


    for (id = 0; id < NUM_THREADS; id++) {
        arguments[id] = id;
        r = pthread_create(&threads[id], NULL, f, (void *) &arguments[id]);
 
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

    

    for (i = 0; i < SIZE; i++) {
        printf("%d ", v[i]);
    }
    printf("\n");

    
   

    pthread_exit(NULL);
}