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
    int pow_max = 20;
    
    //testing algorithm
    std::ofstream output;
    output.open("../out/" + std::to_string(rank) + ".csv");
    output << "N,t\n";

    for (int N = 8; N < std::pow(2, pow_max); N = N * 2) {
        int* a;
        if (rank == 0) {
            a = random_array(N, M);
        }

        double t1, t2;
        if (rank == 0) {
            t1 = MPI_Wtime();
        }

        parallel_sort(a, N, size, rank);

        if (rank == 0) {
            t2 = MPI_Wtime();
            output << N << ',' << t2 - t1 << '\n';
        }
    }

    //finalizing all
    MPI_Finalize();
    return 0;
}