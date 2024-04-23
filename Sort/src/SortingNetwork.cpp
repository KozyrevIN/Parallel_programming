#include "../include/SortingNetwork.h"

SortingNetwork::SortingNetwork(int size) {
    //Optimal solutions
    if (size == 1) {
        cycles = 0;
    } else if (size == 4) {
        cycles = 3;
        comparators = Eigen::MatrixXi {{ 1,  0,  3,  2},
                                       { 2,  3,  0,  1},
                                       {-1,  2,  1, -1}};
    } else if (size == 6) {
        cycles = 5;
        comparators = Eigen::MatrixXi {{ 1,  0,  3,  2,  5,  4},
                                       { 2,  4,  0,  5,  1,  3},
                                       { 1,  0,  3,  2,  5,  4},
                                       {-1,  2,  1,  4,  3, -1},
                                       {-1, -1,  3,  2, -1, -1}};
    }
}

int SortingNetwork::GetCycles() {
    return cycles;
}

int SortingNetwork::CompareTo(int cycle, int rank) {
    return comparators(cycle, rank);
}