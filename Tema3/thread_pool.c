#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "thread_pool.h"

#define NR_MAX_THREADS (sysconf(_SC_NPROCESSORS_CONF) - 1)

typedef struct task {
	//functia executata de task
	void (*task_funct) (void *);
	//argumentul
	void *task_arg;
} task;

typedef struct thread_pool {
	int num_tasks; //numarul de taskuri din coada
	int num_threads; //numarul maxim de threaduri disponibile pt executie

	task* task_queue; //coada de taskuri de executat
	pthread_t *threads; 

	int first; //primul task din coada
	int last; //ultimul task din coada

	pthread_mutex_t queue_mutex;
	pthread_cond_t cond_var;

	int shutdown;
	int started;

	int started_threads;

	void **return_values;
	int num_results;

} thread_pool;

static void *threadpool_thread(void *threadpool);

thread_pool* thread_pool_init(int nr_processors, int num_tasks){
	thread_pool *pool;
	if (nr_processors > NR_MAX_THREADS) {
		return NULL;
	}

	pool = (thread_pool *)malloc(sizeof(thread_pool));
	pool->threads = (pthread_t *)malloc(nr_processors * sizeof(pthread_t));
	pool->num_threads = nr_processors;
	pool->num_tasks = num_tasks;


	pool->first = 0;
	pool->last = 0;

	pool->task_queue = (task *)malloc(num_tasks * sizeof(task));
	//initializare mutex si variabile conditionale
	pthread_mutex_init(&(pool->queue_mutex), NULL);
	pthread_cond_init(&(pool->cond_var),NULL);

	//sunt create threadurile
	int i;
	for (i = 0; i < nr_processors; i++) {
		int r = pthread_create(&(pool->threads[i]), NULL, threadpool_thread, (void *)pool);
		if (r != 0) {
			exit(-1);
		}
		pool->started_threads++;
	}

	return pool;
}

static void *threadpool_thread(void *threadpool)
{
    thread_pool *pool = (thread_pool *)threadpool;
    task task;

    for(;;) {
        pthread_mutex_lock(&(pool->queue_mutex));

        while((pool->num_tasks == 0) && (!pool->shutdown)) {
            pthread_cond_wait(&(pool->cond_var), &(pool->queue_mutex));
        }

        if (pool->num_tasks == 0) {
            break;
        }

        task.task_funct = pool->task_queue[pool->first].task_funct;
        task.task_arg = pool->task_queue[pool->first].task_arg;
        pool->first = (pool->first + 1) % pool->num_tasks;
        pool->num_tasks -= 1;

        pthread_mutex_unlock(&(pool->queue_mutex));

        (*(task.task_funct))(task.task_arg);
    }

    pool->started--;

    pthread_mutex_unlock(&(pool->queue_mutex));
    pthread_exit(NULL);
    return(NULL);
}

int add_task_to_thread_pool(void (*task_function)(void *), void* task_arg, thread_pool* pool) {
	int check = 0;
	int size = pool->num_tasks;
	int next = (pool->last + 1) % size;


	pthread_mutex_lock(&(pool->queue_mutex));
	pool->task_queue[pool->last].task_funct = task_function;
	pool->task_queue[pool->last].task_arg = task_arg;
	pool->last = next;
	pool->num_tasks += 1;

	pthread_mutex_unlock(&(pool->queue_mutex));
	return check;
}
