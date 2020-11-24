#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
 
int NUM_THREADS; //nr de threaduri
int L;
int apparitions[52];

#define MIN(x, y) (((x) < (y)) ? (x) : (y))



void *f(void *arg)
{
    long id = *(long*) arg;
    //int start = id * (double) SIZE/NUM_THREADS;
    //int end = MIN((id + 1) * (double) SIZE/NUM_THREADS, SIZE);


    

    pthread_exit(NULL);
}
 



int main(int argc, char *argv[])
{
    L = atoi(argv[1]); //dimensiunea chunk-ului citit de un thread 

    pthread_t threads[NUM_THREADS];
    int r;
    long id;
    void *status;
    long arguments[NUM_THREADS];

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