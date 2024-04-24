#include <mpi.h>

#include "../include/sorting_network.h"
#include "../include/parallel_sort.h"


void compare(int rank, int compare_to, int* a, int n) {
    if (compare_to >= 0) {
        int* b = new int[n];
        int* c = new int[n];

        MPI_Sendrecv(a, n, MPI_INT, compare_to, 0,
                     b, n, MPI_INT, compare_to, 0, 
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        if (rank < compare_to) {
            for (int k = 0, i = 0, j = 0; k < n; k++) {
                if (a[i] < b[j]) {
                    c[k] = a[i++];
                } else {
                    c[k] = b[j++];
                }
            }
        } else {
            for (int k = n - 1, i = n - 1, j = n - 1; k >= 0; k--) {
                if (a[i] > b[j]) {
                    c[k] = a[i--];
                } else {
                    c[k] = b[j--];
                }
            }
        }

        std::swap(a, c);
        delete[] b, c;
    } 
}

void parallel_sort(int* a, int N, int size, int rank) {
    int n = N / size;
    int* ai = new int[n];
    
    MPI_Scatter(a, n, MPI_INTEGER,
                ai, n, MPI_INTEGER, 0, MPI_COMM_WORLD);

    std::sort(ai, &ai[n]);

    auto net = SortingNetwork(size);

    for (int i = 0; i < net.get_cycles(); i++) {
        compare(rank, net.compare_to(i, rank), ai, n);
        MPI_Barrier;
    }

    MPI_Gather(ai, n, MPI_INT,
               a, n, MPI_INTEGER, 0, MPI_COMM_WORLD);

    delete[] ai;
}