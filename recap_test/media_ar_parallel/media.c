#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
 
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

int SIZE;
int NUM_THREADS;
int *v;
int sum;
double media;

pthread_mutex_t mutex;
pthread_barrier_t barrier;


void *f(void *arg)
{
    long id = *(long*) arg;

    int start = id * (double) SIZE/NUM_THREADS;
    int end = MIN((id + 1) * (double) SIZE/NUM_THREADS, SIZE);
  
    int sum1 = 0; //este suma pe bucata de vector pe care lucreaza un thread

    int i;
    for (i = start; i < end; i++) {
       // printf("Hello din thread-ul %ld; index = %d \n", id, i);
        sum1 += v[i];
    }
    
    pthread_mutex_lock(&mutex);
    sum += sum1;
    pthread_mutex_unlock(&mutex);
    

    pthread_barrier_wait(&barrier);
    if (id == 0) {
        media = (double)sum / SIZE;
    }

    pthread_exit(NULL);
}
 


int main(int argc, char *argv[])
{
    if (argc != 3) {
        printf("./media N P\n");
        return -1;
    }
    SIZE = atoi(argv[1]);
    NUM_THREADS = atoi(argv[2]);

   // printf("size: %d\n", SIZE);
   // printf("thr: %d\n", NUM_THREADS);


    pthread_t threads[NUM_THREADS];
    long arguments[NUM_THREADS];
    int r;
    long id;
    void *status;
    
    v = (int *)malloc(SIZE * sizeof(int));

    int i;
    for (i = 0; i < SIZE; i++) {
        v[i] = i; 
    }
 
    pthread_mutex_init(&mutex, NULL);
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
    /*
    for (i = 0; i < SIZE; i++) {
        printf("%d ", v[i]);
    }
    printf("\n");

    printf("suma: %d\n", sum);
    printf("media: %lf\n", media);
*/
    r = pthread_barrier_destroy(&barrier);
    if (r != 0) {
        printf("Eroare la distrugerea barierei\n");
    }
    r = pthread_mutex_destroy(&mutex);
    if (r != 0) {
        printf("Eroare la distrugere mutex\n");
    }

    pthread_exit(NULL);
}