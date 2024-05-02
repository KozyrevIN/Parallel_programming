#include <stdio.h>
#include <pthread.h>

const int NUM_THREADS = 8;

/* create thread argument struct for thr_func() */

struct thread_data {
    int tid;
    double stuff;
};

/* thread function */

void* thr_func(void* arg) {
    thread_data* data = (thread_data*) arg;
    printf("hello world, thread id: %d\n", data->tid);
    pthread_exit(NULL);
}   

int main(int argc, char **argv) {
    pthread_t thr[NUM_THREADS];
    int i, rc;

    /* create a thread_data_t argument array */

    thread_data thr_data[NUM_THREADS];

    /* create threads */

    for(i = 0; i < NUM_THREADS; ++i) {
        thr_data[i].tid = i;
        if((rc = pthread_create(&thr[i], NULL, thr_func, &thr_data[i]))) {
            fprintf(stderr, "error: pthread_create, rc: %d\n", rc);
            return 1;
        }
    }

    /* block until all threads complete */

    for(i = 0; i< NUM_THREADS; ++i) {
        pthread_join(thr[i], NULL);
    }

    return 0;
}