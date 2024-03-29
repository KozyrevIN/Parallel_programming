#include <mpi.h>
#include <iostream>
#include <functional>

struct ConvectionDiffusionProblem //equation of form u_t + au_x = 0 with initial values f_0 and boundary conditions y_0
{
    double a; //parameter in equation
    double L, T; //space and time sizes of study area
    
    std::function<double(double)> f_0; //initial values
    std::function<double(double)> y_0; //boundary conditions

    std::string name;

    //constructor
    ConvectionDiffusionProblem(double, double, double, std::function<double(double)>, std::function<double(double)>, std::string name);

    //method that gives exact solution y(x, t)
    double y_exact(double, double);
};

ConvectionDiffusionProblem::ConvectionDiffusionProblem(double a, double L, double T,
                                                         std::function<double(double)> f_0, std::function<double(double)> y_0, std::string name):
                                                         a{a}, L{L}, T{T}, f_0{f_0}, y_0{y_0}, name{name} { }

double ConvectionDiffusionProblem::y_exact(double x, double t) {
    if (x > a * t) {
        return(f_0(x - a * t));
    }
    else {
        return(y_0(t - x / a));
    }
}

int main(int argc, char **argv) 
{
    //initializing MPI
    MPI_Init(&argc, &argv);

    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    double t_1, t_2;
    double pass = 1.0;

    t_1 = MPI_Wtime();
    for(int i = 0; i < 1000000; i++) {
        if (i % 2 == 0) {
            if(rank == 0) {
            MPI_Send(&pass, 1, MPI_DOUBLE, 1, i, MPI_COMM_WORLD);
            }
            else if (rank == 1) {
                MPI_Recv(&pass, 1, MPI_DOUBLE, 0, i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
        } else {
            if(rank == 1) {
                MPI_Send(&pass, 1, MPI_DOUBLE, 0, i, MPI_COMM_WORLD);
            }
            else if (rank == 0) {
                MPI_Recv(&pass, 1, MPI_DOUBLE, 1, i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
        }

        if(rank == 0) {
            MPI_Send(&pass, 1, MPI_DOUBLE, 1, i, MPI_COMM_WORLD);
        }
        else if (rank == 1) {
            MPI_Recv(&pass, 1, MPI_DOUBLE, 0, i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    }
    t_2 = MPI_Wtime();

    std::cout << "send, recieve " << t_2 - t_1 << '\n';

    double recv;
    t_1 = MPI_Wtime();
    for(int i = 0; i < 1000000; i++) {
        MPI_Sendrecv(&pass, 1, MPI_DOUBLE, (rank + 1) % 2, i,
        &recv, 1, MPI_DOUBLE, (rank + 1) % 2, i, 
        MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    t_2 = MPI_Wtime();

    std::cout << "sendrecv " << t_2 - t_1 << '\n';

    double a = 0.5; //parameter in equation
    double L = 4; double T = 4; //space and time sizes of study area

    std::string name_rect = "rectangle";
    auto f_0_rect = []( double x ) { if (std::abs(x - 1) < 0.5) return 1.0;
                                     else return 0.0; }; //initial values
    auto y_0_rect = []( double t ) { return 0; }; //boundary conditions

    auto problem = ConvectionDiffusionProblem(a, L, T, f_0_rect, y_0_rect, name_rect);

    int N = 10000; int K = 10000;
    double time_coef = T / K;
    double tau = problem.T / K;
    double h = problem.L / N;
    double c = problem.a * tau / h;
    auto y_cur = std::vector(N, 0.0);
    auto y_prev = std::vector(N, 0.0);
    auto y_prev_2 = std::vector(N, 0.0);

    for (int i = 0; i <= N; i++) {
        y_cur[i] = problem.f_0(i * L / N);
    }

    t_1 = MPI_Wtime();
    for(int j = 1; j <= K; j++) {
        std::swap(y_prev, y_prev_2);
        std::swap(y_cur, y_prev);

        y_cur[0] = problem.y_0(time_coef * j);

        for(int i = 1; i < N - 1; i++) {
            y_cur[i] = y_prev_2[i] - c * (y_prev[i + 1] - y_prev[i - 1]);
        }

        y_cur[N - 1] = y_prev[N - 1] - c * (y_prev[N - 1] - y_prev[N - 2]);
    }
    t_2 = MPI_Wtime();
    std::cout << "C " << t_2 - t_1 << '\n';

    MPI_Finalize();

    return 0;
}

