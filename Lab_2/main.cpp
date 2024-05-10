#include <pthread.h>
#include <cmath>
#include <iostream>
#include <fstream>
#include <chrono>

/*define function for integration*/
double f(double x) {
    return(std::cos(1 / x));
}

const double a = 0.01;
const double b = 1;

/*function for klahan summation*/
void csum(double &a,double b,double &c) {
    double y = b - c; 
    b = a + y; 
    c = (b - a) - y;
    a = b;
}

/* create thread argument struct for thr_func() */

struct thread_data {
    int tid;
    int N;
    double a_thread, b_thread;
    double partial_integral;
};

/* thread function */

void* thr_func(void* arg) {
    thread_data* data = (thread_data*) arg;
    double delta = (data -> b_thread - data -> a_thread) / data -> N;
    double f_cur = f(data -> a_thread);
    double f_prev;
    double c = 0; //compensation term

    for (double x = data -> a_thread + delta; x <= data -> b_thread; x += delta) {
        std::swap(f_cur, f_prev);
        f_cur = f(x);
        csum(data -> partial_integral, delta * (f_cur + f_prev) / 2, c);
    }

    pthread_exit(NULL);
}

/*parallel integration algorithm*/

double integrate_parallel(int num_threads, double eps) {
    pthread_t thr[num_threads];
    int i, rc;

    thread_data thr_data[num_threads];

    for (i = 0; i < num_threads; ++i) {
        thr_data[i].tid = i;
        thr_data[i].N = ceil(1 / (2 * a * eps * num_threads));
        thr_data[i].a_thread = a + (b - a) * i / num_threads;
        thr_data[i].b_thread = a + (b - a) * (i + 1) / num_threads;
        thr_data[i].partial_integral = 0;
        if((rc = pthread_create(&thr[i], NULL, thr_func, &thr_data[i]))) {
            fprintf(stderr, "error: pthread_create, rc: %d\n", rc);
            return 1;
        }
    }

    double integral = 0;
    for (i = 0; i < num_threads; ++i) {
        pthread_join(thr[i], NULL);
        integral += thr_data[i].partial_integral;
    }

    return integral;
}

int main(int argc, char **argv) {

    std::ofstream output;
    output.open("out.csv");
    output << "n,t\n";

    for (int num_threads = 1; num_threads <= 8; num_threads++)
    {
        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        integrate_parallel(num_threads, 1e-6);
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

        double t = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
        output << num_threads << ',' << t << '\n';
    }
        
    return 0;
}