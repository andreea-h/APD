#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
 
#define NUM_THREADS 8

int a = 0;
pthread_mutex_t mutex;


void *f(void *arg)
{
    long id = *(long*) arg;
    int i;

    printf("Eu sunt thread-ul %ld\n", id);

    for (i = 0; i < 10; i ++) {
        pthread_mutex_lock(&mutex);
        a += 1;
        printf("increment din thread-ul %ld, valoarea lui a este: %d\n", id, a);
        pthread_mutex_unlock(&mutex);
    }

    


    //printf("Hello World din thread-ul %ld! Valoarea lui a este %d\n", id, a);
    

    pthread_exit(NULL);
}
 



int main(int argc, char *argv[])
{
    pthread_t threads[NUM_THREADS];
    int r;
    long id;
    void *status;
    long arguments[NUM_THREADS];

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
    
    printf("Valoarea lui a este: %d\n", a);
    pthread_exit(NULL);
}