#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
 
//#define NUM_THREADS 8

#define MIN(x, y) (((x) < (y)) ? (x) : (y))

int SIZE;
int *v;
int NUM_THREADS;
int result;
int sum = 0;

pthread_mutex_t mutex;

void *f(void *arg)
{
    long id = *(long*) arg;

    int start = id * (double) SIZE/NUM_THREADS;
    int end = MIN((id + 1) * (double) SIZE/NUM_THREADS, SIZE);
    


    int i;
    for (i = start; i < end; i++) {
        sum += v[i];
    }

    //pthread_mutex_lock(&mutex);
   // result += sum;
   // pthread_mutex_unlock(&mutex);

    pthread_exit(NULL);
}
 


int main(int argc, char *argv[])
{
    int r;
    long id;
    void *status;
    
    if (argc != 3) {
        printf("./parallel_sum N P\n");
    }

    SIZE = atoi(argv[1]);
    NUM_THREADS = atoi(argv[2]);

    long arguments[NUM_THREADS];
    pthread_t threads[NUM_THREADS];

    v = (int *)malloc(SIZE * sizeof(int));

    int i;
    for (i = 0; i < SIZE; i++) {
        v[i] = 10; 
    }

    pthread_mutex_init(&mutex, NULL);

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
    
    r = pthread_mutex_destroy(&mutex);
    if (r != 0) {
        printf("Eroare la distrugerea mutexului\n");
    }

    for (i = 0; i < SIZE; i++) {
        printf("%d ", v[i]); 
    }
    printf("\n");

    printf("result: %d\n", sum);


    pthread_exit(NULL);
}