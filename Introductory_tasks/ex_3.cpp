#include "mpi.h"
#include <iostream>

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int item = 0;

    if(rank > 0) {
        MPI_Status status;
        MPI_Recv(&item, 1, MPI_2INT, rank - 1, rank - 1, MPI_COMM_WORLD, &status);
        std::cout << "process " << rank << " recieved item = " << item << '\n'; 
        item++;
    }

    MPI_Ssend(&item, 1, MPI_INT, (rank + 1) % size, rank, MPI_COMM_WORLD);

    if(rank == 0) {
        MPI_Status status;
        MPI_Recv(&item, 1, MPI_2INT, size - 1, size - 1, MPI_COMM_WORLD, &status);
        std::cout << "process " << rank << " recieved item = " << item << '\n'; 
    }

    MPI_Finalize();

  return 0;
}