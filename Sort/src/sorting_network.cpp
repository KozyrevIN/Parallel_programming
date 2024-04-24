#include "../include/sorting_network.h"

SortingNetwork::SortingNetwork(int size) {
    //Optimal solutions
    if (size == 1) {
        cycles = 0;
    } else if (size == 2) {
        cycles = 1;
        comparators = Eigen::MatrixXi {{ 1,  0}};
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
    } else if (size == 8) {
        cycles = 6;
        comparators = Eigen::MatrixXi {{ 2,  3,  0,  1,  6,  7,  4,  5},
                                       { 4,  5,  6,  7,  0,  1,  2,  3},
                                       { 1,  0,  3,  2,  5,  4,  7,  6},
                                       {-1, -1,  4,  5,  2,  3, -1, -1},
                                       {-1,  4, -1,  6,  1, -1,  3, -1},
                                       {-1,  2,  1,  4,  3,  6,  5, -1}};
    }
}

int SortingNetwork::get_cycles() {
    return cycles;
}

int SortingNetwork::compare_to(int cycle, int rank) {
    return comparators(cycle, rank);
}