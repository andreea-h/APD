#include <stdlib.h>


	//lista de threaduri
typedef struct thread_pool thread_pool;

	//initializeaza pool-ul pentru a putea folosi maxim nr_processors (nr maxim de fire de executie - 1)
thread_pool* thread_pool_init(int nr_processors, int num_tasks);

	//adauga un task in thread_pool
int add_task_to_thread_pool(void (*task_function)(void *), void* task_arg, thread_pool* pool);

	//incheie toate taskurile din pool
void finish_tasks(thread_pool);

	//elibereaza pool-ul
void delete_thread_pool(thread_pool);