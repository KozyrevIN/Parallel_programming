#include <mpi.h>
#include <iostream>

void wait_2_seconds() {
    double t1, t2;
    t1 = MPI_Wtime();
    t2 = 0;
    while (t2 - t1 < 2) {
        t2 = MPI_Wtime();
    }
}

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    double t1, t2;
    const int N = 10000;
    int* data = new int[N];
    MPI_Status status;

    if (rank == 0) {
        std::cout << "testing MPI_Send\n\n";
    }
    
    for (int i = 1; i <= N; i = i * 2) {
        MPI_Barrier(MPI_COMM_WORLD);

        if (rank == 0) {
            t1 = MPI_Wtime();
            MPI_Send(data, i, MPI_INT, 1, 0, MPI_COMM_WORLD);
            t2 = MPI_Wtime();
            std::cout << i * sizeof(int) << " bytes sent in " << t2 - t1 << " seconds\n\n"; 
        }
        else if (rank == 1) {
            wait_2_seconds();
            MPI_Recv(data, i, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        }
    }

    if (rank == 0) {
        std::cout << "testing MPI_Ssend\n\n";
    }
    
    for (int i = 1; i <= N; i = i * 2) {
        MPI_Barrier(MPI_COMM_WORLD);

        if (rank == 0) {
            t1 = MPI_Wtime();
            MPI_Ssend(data, i, MPI_INT, 1, 0, MPI_COMM_WORLD);
            t2 = MPI_Wtime();
            std::cout << i * sizeof(int) << " bytes sent in " << t2 - t1 << " seconds\n\n"; 
        }
        else if (rank == 1) {
            wait_2_seconds();
            MPI_Recv(data, i, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        }
    }

    if (rank == 0) {
        std::cout << "testing MPI_Rsend\n\n";
    }
    
    for (int i = 1; i <= N; i = i * 2) {
        MPI_Barrier(MPI_COMM_WORLD);

        if (rank == 0) {
            t1 = MPI_Wtime();
            MPI_Rsend(data, i, MPI_INT, 1, 0, MPI_COMM_WORLD);
            t2 = MPI_Wtime();
            std::cout << i * sizeof(int) << " bytes sent in " << t2 - t1 << " seconds\n\n"; 
        }
        else if (rank == 1) {
            wait_2_seconds();
            MPI_Recv(data, i, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        }
    }

    if (rank == 0) {
        std::cout << "testing MPI_Bsend\n\n";
    }
    
    for (int i = 1; i <= N; i = i * 2) {
        MPI_Barrier(MPI_COMM_WORLD);

        if (rank == 0) {
            t1 = MPI_Wtime();
            MPI_Bsend(data, i, MPI_INT, 1, 0, MPI_COMM_WORLD);
            t2 = MPI_Wtime();
            std::cout << i * sizeof(int) << " bytes sent in " << t2 - t1 << " seconds\n\n"; 
        }
        else if (rank == 1) {
            wait_2_seconds();
            MPI_Recv(data, i, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        }
    }

    delete[] data;

    MPI_Finalize();

    return 0;
}