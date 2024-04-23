#include <eigen3/Eigen/Dense>

class SortingNetwork
{
    private:
    int cycles;
    Eigen::MatrixXi comparators;

    public:
    SortingNetwork(int);
    int GetCycles();
    int CompareTo(int, int);
};