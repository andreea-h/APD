#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#define NUM_THREADS 8

void *threadFunction(void *arg) {
    long id = *(long*) arg;
    // operatie - exemplu dummy
    id++;
    // intoarcem rezultatul functiei
    return (void*) id;
}

int main() {
    pthread_t threads[NUM_THREADS];
    long r;
    long id;
    void *status;
    long arguments[NUM_THREADS];
    struct timeval tv1, tv2;

    for (id = 0; id < NUM_THREADS; id++) {
        arguments[id] = id;
    }
 
    /*
        un mod indicat de a masura scalabilitatea unui program este
        sa masuram timpul de executie al functiei (sau al functiilor)
        executata / executate de catre thread-uri
        este de recomandat sa punem un timestamp fix inainte de crearea si
        de lansarea thread-urilor (variabila start de mai jos), care sa fie unul de start,
        si inca un timestamp imediat dupa join-ul thread-urilor (un timestamp de stop - variabila end)
        si sa facem diferenta dintre cele doua timestamps (end - start)
    */

    // timestamp de start
    gettimeofday(&tv1, NULL);
    for (id = 0; id < NUM_THREADS; id++) {
        r = pthread_create(&threads[id], NULL, threadFunction, (void *) &arguments[id]);
 
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
        /*
            status reprezinta rezultatul functiei threadFunction si este de tip void*
            astfel facem cast de la void* la long
        */
        long result = (long) status;
        // afisam rezultatul functiei threadFunction
        printf("Rezultatul functiei threadFunction = %ld\n", result);
    }
    // timestamp de stop
    gettimeofday(&tv2, NULL);

    printf ("Total time = %f seconds\n",
        (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +
        (double) (tv2.tv_sec - tv1.tv_sec));
 
    return 0;
}