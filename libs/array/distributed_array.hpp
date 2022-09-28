#include <chrono>
#include <deque>
#include <thread> 
#include <variant>


#include "mpi.h"


const int SEND_TAG = 0;
const int RECV_TAG = 1;


using namespace std;

template <typename T> class DistributedArray {

public:
    DistributedArray<T>(int size, int num_processes=0) {
        MPI_Comm_size(MPI_COMM_WORLD, &this->mpi_size);
        MPI_Comm_rank(MPI_COMM_WORLD, &this->mpi_rank);

        this->size = size;
        if (num_processes > 0)
            this->num_processes = num_processes;
        else
            this->num_processes = this->mpi_size;

        this->local_size = this->size / this->num_processes;
        if (this->mpi_rank < this->size % this->num_processes)
            this->local_size++;

        this->local_arr = new T[this->local_size];
    }


    T get(int index) {
        int owner_rank = index % this->num_processes;
        if (owner_rank == this->mpi_rank) {
            return this->local_arr[index / this->num_processes];
        }
    }


    std::variant<T, MPI_Request> iget(int index) {

    }


    void set(int index, T object) {
        int owner_rank = index % this->num_processes;
        if (owner_rank == this->mpi_rank) {
            this->local_arr[index / this->num_processes] = object;
        }
    }


    MPI_Request iset(int index, T object) {

    }


private:
    int mpi_rank, mpi_size;
    int size, num_processes;

    int local_size;
    T *local_arr;
};
