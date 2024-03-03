#include <mpi.h>
#include <gmp.h>
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
    const ulong N = 500000; //precision (digits)
    auto f = [](float M){ return (M * std::log(M) - M) / std::log(10) - N; };
    auto df = [](float M){ return std::log(M) / std::log(10); };
    const ulong M = ulong(Newton(f, df, float(N))) + size; //needed of amount of terms - 1
    const ulong n = ulong(std::log2(10) * (N + size + M)); //precision (bits)

    mpz_t fact[N];
    mpf_t exp;
    mpf_init2(exp, n);

    //calculating factorials
    ulong left = M * rank / size;
    ulong right = M * (rank + 1) / size;
    if(rank == 0) 
        mpz_init_set_ui(fact[0], 1);
    else
        mpz_init_set_ui(fact[left], left);
    for(ulong i = left + 1; i < right; i++) {
        mpz_init(fact[i]);
        mpz_mul_ui(fact[i], fact[i - 1], i); 
    }
    
    mpz_t low_fact;
    mpz_init(low_fact);
    
    if(rank != 0) {
        MPI_Status status;
        ulong length;
        MPI_Recv(&length, 1, MPI_UNSIGNED_LONG, rank - 1, 0, MPI_COMM_WORLD, &status);
        ulong* fact_pass = new ulong[length];
        MPI_Recv(fact_pass, length, MPI_UNSIGNED_LONG, rank - 1, 1, MPI_COMM_WORLD, &status);
        mpz_import(low_fact, length, 1, sizeof(ulong), 0, 0, fact_pass);
        //std::cout << "process " << rank << " recieved\n";
        //gmp_printf("%Zd \n", low_fact);
        delete[] fact_pass;
        mpz_mul(fact[right - 1], fact[right - 1], low_fact);
    }

    if(rank != size - 1) {
        ulong* length = new ulong;
        void* fact_pass = mpz_export(NULL, length, 1, sizeof(ulong), 0, 0, fact[right - 1]);
        //std::cout << "sending to process " << rank + 1 << '\n';
        //gmp_printf("%Zd \n", fact[right - 1]);
        MPI_Send(length, 1, MPI_UNSIGNED_LONG, rank + 1, 0, MPI_COMM_WORLD);
        MPI_Send(fact_pass, *length, MPI_UNSIGNED_LONG, rank + 1, 1, MPI_COMM_WORLD);
        delete length;
    }

    if(rank != 0) {
        for(ulong i = left; i < right - 1; i++) {
            mpz_mul(fact[i], fact[i], low_fact);
        }
    }

    //calculating exponents

    mpf_t tmp;
    mpf_init2(tmp, n);

    for(ulong i = left; i < right; i++) {
        mpf_set_z(tmp, fact[i]);
        mpf_ui_div(tmp, 1, tmp);
        mpf_add(exp, exp, tmp);
    }

    ulong pass_size = n;

    if(rank != 0) {
        long* exponent = new long;
        char* exp_pass_raw = mpf_get_str(NULL, exponent, 2, pass_size, exp);
        char* exp_pass = new char[pass_size + 2];
        exp_pass[0] = '.';
        for(ulong i = 1; i <= - *exponent; i++) {
            exp_pass[i] = '0';
        }
        for(ulong i = - *exponent + 1; i <= pass_size; i++) {
            exp_pass[i] = exp_pass_raw[i + *exponent - 1];
        }
        exp_pass[pass_size + 1] = '@';
        MPI_Send(exp_pass, pass_size + 2, MPI_CHAR, 0, 3, MPI_COMM_WORLD);
        delete exponent;
        delete[] exp_pass;
    }
    else {
        MPI_Status status;
        long exponent;
        char* exp_pass = new char[pass_size + 2];
        for(int i = 1; i < size; i++) {
            MPI_Recv(exp_pass, pass_size + 2, MPI_CHAR, MPI_ANY_SOURCE, 3, MPI_COMM_WORLD, &status);
            mpf_set_str(tmp, exp_pass, 2);
            mpf_add(exp, exp, tmp);
        }
        delete[] exp_pass;
        //gmp_printf("e = %.10000Ff\n", exp);
    }

    std::cout << "finished\n";

    mpf_clear(tmp);

    MPI_Finalize();

    return 0;
}