#include <eigen3/Eigen/Dense>

class SortingNetwork
{
    private:
    int cycles;
    Eigen::MatrixXi comparators;

    public:
    SortingNetwork(int);
    int get_cycles();
    int compare_to(int, int);
};