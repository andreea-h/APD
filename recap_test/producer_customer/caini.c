#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>

/*
This program provides a possible solution for producer-consumer problem using mutex and semaphore.
I have used 5 producers and 5 consumers to demonstrate the solution. You can always play with these values.
*/

#define MaxItems 5 // Maximum items a producer can produce or a consumer can consume
int BufferSize;


sem_t empty;
sem_t full;
int in = 0;
int out = 0;
int *buffer;
pthread_mutex_t mutex;

void *producer(void *pno)
{   
    int item;
    for(int i = 0; i < MaxItems; i++) {
        item = rand(); // Produce an random item
        sem_wait(&empty);
        pthread_mutex_lock(&mutex);
        buffer[in] = item;
        printf("Producer %d: Insert Item %d at %d\n", *((int *)pno),buffer[in],in);
        in = (in+1)%BufferSize;
        pthread_mutex_unlock(&mutex);
        sem_post(&full);
    }
}

void *consumer(void *cno)
{   
    for(int i = 0; i < MaxItems; i++) {
        sem_wait(&full);
        pthread_mutex_lock(&mutex);
        int item = buffer[out];
        printf("Consumer %d: Remove Item %d from %d\n",*((int *)cno),item, out);
        out = (out+1)%BufferSize;
        pthread_mutex_unlock(&mutex);
        sem_post(&empty);
    }
}

int main(char **argv)
{   
    int N = atoi(argv[1]);//caini-consumatori
    int M = atoi(argv[1]);//producatori-castroane
    BufferSize = M;

    buffer = (int *)malloc(BufferSize * sizeof(int));

    pthread_t pro[M], con[N];//sunt mai multi consumatori decat producatori
    pthread_mutex_init(&mutex, NULL);
    sem_init(&empty,0,BufferSize);
    sem_init(&full,0,0);

    int arguments[M + N];
    

    for(int i = 0; i < M; i++) {
        arguments[i] = i;
        pthread_create(&pro[i], NULL, (void *)producer, (void *)&arguments[i]);
    }
    for(int i = 0; i < N; i++) {
        arguments[i] = i;
        pthread_create(&con[i], NULL, (void *)consumer, (void *)&arguments[i]);
    }

    for(int i = 0; i < M; i++) {
        pthread_join(pro[i], NULL);
    }
    for(int i = 0; i < N; i++) {
        pthread_join(con[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    sem_destroy(&empty);
    sem_destroy(&full);

    return 0;
    
}