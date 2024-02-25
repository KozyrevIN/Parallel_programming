#include <mpi.h>
#include <iostream>

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int N = atoi(argv[1]);

    int left = (N * rank) / size + 1;
    int right = (N * (rank + 1)) / size + 1;
    double sum = 0;

    for(int i = left; i < right; i++) {
        sum += 1.0 / i;
    }

    if(rank != 0) {
        MPI_Ssend(&sum, 1, MPI_2DOUBLE_PRECISION, 0, 0, MPI_COMM_WORLD);
    }
    else {
        for(int i = 1; i < size; i++) {
            double recv_sum;
            MPI_Status status;
            MPI_Recv(&recv_sum, 1, MPI_2DOUBLE_PRECISION, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);

            sum += recv_sum;
        }

        std::cout << sum << '\n';
    }

    MPI_Finalize();

    return 0;
}