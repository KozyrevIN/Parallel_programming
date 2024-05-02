#include <iostream>
#include <vector>
#include <random>
#include <climits>
#include <cmath>
#include <fstream>
#include <mpi.h>

#include "../include/sorting_network.h"
#include "../include/parallel_sort.h"

int* random_array(int N, int M) {
    std::random_device rd;  // a seed source for the random number engine
    std::mt19937 gen(rd()); // mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> distrib(-M, M);

    int* a = new int[N];
    for(int i = 0; i < N; i++) {
        a[i] = distrib(gen);
    }

    return a;
}

int main(int argc, char **argv)
{
    //initializing MPI
    MPI_Init(&argc, &argv);

    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    //initial params
    int M = INT_MAX;
    int pow_max = 26;
    
    //testing algorithm
    std::ofstream output;
    if (rank == 0) {
        output.open("../out/" + std::to_string(size) + "_best.csv");
        output << "N,t\n";
    }

    for (int N = 16; N <= std::pow(2, pow_max); N = N * 2) {
        int* a;
        if (rank == 0) {
            a = new int[N];
            for (int i = 0; i < N; i++) {
                a[i] = N - i;
            }
            //a = random_array(N, M);
        }

        double t1, t2;
        if (rank == 0) {
            t1 = MPI_Wtime();
        }

        if (size > 1) {
            parallel_sort(a, N, size, rank);
        } else {
            std::sort(a, &a[N]);
        }

        if (rank == 0) {
            t2 = MPI_Wtime();
            output << N << ',' << t2 - t1 << '\n';
        }
    }

    //finalizing all
    if (rank == 0) {
        output.close();
    }
    MPI_Finalize();
    return 0;
}