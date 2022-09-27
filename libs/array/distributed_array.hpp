#include <deque>
#include <variant>


#include <omp.h>
#include "mpi.h"


const int SEND_TAG = 0;
const int RECV_TAG = 1;


template <typename T> class DistributedArray {
public:
    DistributedArray<T>(int capacity) {
        MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);
        DistributedArray<T>(capacity, mpi_size);
    }

    DistributedArray<T>(int capacity, int num_processes) {
        MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);
        MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);

        this->capacity = capacity;
        this->num_processes = num_processes;

        this->local_size = capacity / num_processes;
        if (this->mpi_rank < capacity % num_processes)
            this->local_size++;

        local_arr = new T[this->local_size];
    }

    T get(int index) {
        int owner_rank = index % this->num_processes;
        if (owner_rank == this->mpi_rank) {
            return this->local_arr[index / num_processes];
        }
        MPI_Request request;

    }

    std::variant<T, MPI_Request> iget(int index) {

    }

    void set(int index, T object) {

    }

    MPI_Request iset(int index, T object) {

    }

private:
    int mpi_rank, mpi_size;
    int capacity, num_processes;

    int local_size;
    T *local_arr;
protected:
};

int main(int argc, char* argv[]) {
    return 0;
}
