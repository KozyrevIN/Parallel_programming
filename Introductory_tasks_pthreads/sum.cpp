#include <stdio.h>
#include <pthread.h>

const int NUM_THREADS = 8;
const int N = 1000;

/* create thread argument struct for thr_func() */

struct thread_data {
    int tid;
    double partial_sum;
};

/* thread function */

void* thr_func(void* arg) {
    thread_data* data = (thread_data*) arg;
    for (int i = (data -> tid) * N / NUM_THREADS + 1; i <= (data -> tid + 1) * N / NUM_THREADS; i++) {
        data -> partial_sum += 1.0 / i;
    }
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
        thr_data[i].partial_sum = 0;
        if((rc = pthread_create(&thr[i], NULL, thr_func, &thr_data[i]))) {
            fprintf(stderr, "error: pthread_create, rc: %d\n", rc);
            return 1;
        }
    }

    /* block until all threads complete */

    double sum = 0;
    for(i = 0; i< NUM_THREADS; ++i) {
        pthread_join(thr[i], NULL);
        sum += thr_data[i].partial_sum;
    }

    printf("%.10f\n", sum);

    return 0;
}