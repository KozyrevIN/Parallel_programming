#include <iostream>
#include <fstream>
#include <math.h>
#include <mpi.h>

#ifndef convection_diffusion_problem
#define convection_diffusion_problem
    #include "../include/convection_diffusion_problem.h"
#endif
#ifndef solver
#define solver
    #include "../include/solver.h"
#endif
#include "../include/schemes.h"

int main(int argc, char **argv)
{
    //initializing MPI
    MPI_Init(&argc, &argv);

    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    double a = 0.5; //parameter in equation
    double L = 4; double T = 4; //space and time sizes of study area
    
    //Setting up the rectangle problem
    std::string name_rect = "rectangle";
    auto f_0_rect = []( double x ) { if (std::abs(x - 1) < 0.5) return 1.0;
                                     else return 0.0; }; //initial values
    auto y_0_rect = []( double t ) { return 0; }; //boundary conditions

    auto rectangle = ConvectionDiffusionProblem(a, L, T, f_0_rect, y_0_rect, name_rect);

    //initial paraneters for solver
    int N = 1000; int K = 1000;

    //setting up solver
    auto rect_solver = CrossSolver(size, rank, N, K, rectangle);

    //making animation
    rect_solver.solve(180);

    //plotting time depending on number of cells in a mesh
    int deg_min = 4; int deg_max = 17;
    std::ofstream output;

    if (rank == size - 1) {
        output.open("../out/times/" + std::to_string(size) + ".csv");
        output << "n,t\n";
    }
   
    for(int i = deg_min; i <= deg_max; i++) {
        rect_solver.change_N(std::pow(2, i));
        double t_1, t_2;
        t_1 = MPI_Wtime();
        rect_solver.solve(0);
        t_2 = MPI_Wtime();
        if(rank == size - 1) {
            output << std::pow(2, i) << "," << t_2 - t_1 << '\n';
        }
    }

    if (rank == size - 1) {
        output.close();
    }

    MPI_Finalize();

    return 0;
}
