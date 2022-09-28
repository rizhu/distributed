#include <cassert>


#include "mpi.h"


#include "../../libs/array/distributed_array.hpp"

using namespace std;

int main(int argc, char *argv[]) {
    int mpi_rank, mpi_size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_size );

    int size = 16;
    DistributedArray<int>arr (size);
    for (int i = 0; i < size; i++)
        arr.set(i, i);
    
    for (int i = 0; i < size; i++)
        assert(i == arr.get(i));

    MPI_Finalize();

    if (mpi_rank == 0) {
        cout << "test_array_one_proc: PASSED" << endl;
    }
    return 0;
}