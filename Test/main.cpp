#include <mpi.h>
#include <gmpxx.h>
#include <iostream>
#include <vector>
#include <math.h>
#include <functional>
#include <string>

long Newton(std::function<float(float)> f, std::function<float(float)> df, float M) {
    float f_M = f(M);
    while(f_M > 2) {
        M -= f_M / df(M);
        f_M = f(M);
    }

    return M;
}

int main(int argc, char **argv)
{
    //initializing MPI
    MPI_Init(&argc, &argv);

    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    //setting constants
    const ulong N = 1000; //precision (digits)
    const ulong n = ulong(std::log2(10) * (N + size)); //precision (bits)
    mpf_set_default_prec(n);
    mpf_t exp;
    mpf_init(exp);

    auto f = [](float M){ return (M * std::log(M) - M) / std::log(10) - N; };
    auto df = [](float M){ return std::log(M) / std::log(10); };
    const ulong M = long(Newton(f, df, float(N))) + 2; //needed of amount of terms - 1
    mpz_t fact[N];

    //calculating factorials
    ulong left = M * rank / size;
    ulong right = M * (rank + 1) / size;
    if(rank == 0) 
        mpz_init(fact[0]);
    else
        mpz_init2(fact[left], left);
    for(ulong i = left + 1; i < right; i++) {
        mpz_init(fact[i]);
        mpz_mul_ui(fact[i], fact[i - 1], i); 
    }

    if(rank != size - 1) {
        ulong length;
        ulong* max_fact = mpz_export(NULL, &length, 1, 64, 1, 0, fact[right - 1]);
        for(int i = 0; i < length; i++) {
            std::cout << max_fact[i] << ' ';
        }
        std::cout << '\n';
        //MPI_Bsend(max_fact, max_fact.size(), MPI_BYTE, rank + 1, 1, MPI_COMM_WORLD);
        delete[] max_fact;
    }
    
    

    //calculating exponent


    MPI_Finalize();

    return 0;
}