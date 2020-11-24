#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
 

#define MIN(x, y) (((x) < (y)) ? (x) : (y))

int SIZE;
int NUM_THREADS;

int *v;

void *f(void *arg)
{
    long id = *(long*) arg;
    int start = id * (double) SIZE/NUM_THREADS;
    int end = MIN((id + 1) * (double) SIZE/NUM_THREADS, SIZE);

    int i;
    for (i = start; i < end; i++) {
      
    }
    

    pthread_exit(NULL);
}
 


int main(int argc, char *argv[])
{
    if (argc != 3) {
        printf("./media N P");
    }
    SIZE= atoi(argv[1]);
    NUM_THREADS = atoi(argv[2]);

    pthread_t threads[NUM_THREADS];
    long arguments[NUM_THREADS];
    int r;
    long id;
    void *status;
    

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