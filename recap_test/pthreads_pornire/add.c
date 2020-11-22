#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
 
#define NUM_THREADS 8
#define SIZE 20

#define MIN(x, y) (((x) < (y)) ? (x) : (y))

int v[SIZE];

void *f(void *arg)
{
    long id = *(long*) arg;
    int start = id * (double) SIZE/NUM_THREADS;
    int end = MIN((id + 1) * (double) SIZE/NUM_THREADS, SIZE);

    int i;
    for (i = start; i < end; i++) {
        v[i] += 100;
        v[i] *= 2;
    }
    

    pthread_exit(NULL);
}
 



int main(int argc, char *argv[])
{
    pthread_t threads[NUM_THREADS];
    int r;
    long id;
    void *status;
    long arguments[NUM_THREADS];
    //fiecare thread incrementeaza elementele din vector cu 100, apoi le dubleaza


    int i;
    for (i = 0; i < SIZE; i++) {
        v[i] = i; 
    }
 
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