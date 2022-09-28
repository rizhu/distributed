#include <atomic>
#include <chrono>
#include <deque>
#include <thread> 
#include <variant>

#include "mpi.h"


const int TAG_OP_LEN = 1;
const int GET_TAG = 0;

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

        this->run_threads = true;
        this->get_listener = thread(&DistributedArray<int>::handle_get, this);

        this->get_tag = this->generate_get_tag(this->mpi_rank);
    }


    ~DistributedArray<T>() {
        delete this->local_arr;

        this->run_threads.store(false, memory_order_release);
        this->get_listener.join();
    }


    int local_index(int global_index) {
        return global_index / this->num_processes;
    }


    T get(int index) {
        int owner_rank = index % this->num_processes;

        if (owner_rank == this->mpi_rank) {
            return this->local_arr[local_index(index)];
        }

        char recv_buf[sizeof(T)];
        MPI_Request request;
        MPI_Status status;
        MPI_Isend(&index, 1, MPI_INT, owner_rank, this->get_tag, MPI_COMM_WORLD, &request);
        MPI_Wait(&request, &status);
        MPI_Irecv(&recv_buf, sizeof(T), MPI_CHAR, owner_rank, this->get_tag, MPI_COMM_WORLD, &request);
        MPI_Wait(&request, &status);

        return (T) *recv_buf;
    }


    unsigned int generate_get_tag(unsigned mpi_rank) {
        return (mpi_rank << TAG_OP_LEN) | GET_TAG;
    }


    unsigned int get_tag_source(int get_tag) {
        return get_tag >> TAG_OP_LEN;
    }


    void set(int index, T object) {
        int owner_rank = index % this->num_processes;
        if (owner_rank == this->mpi_rank) {
            this->local_arr[index / this->num_processes] = object;
        }
    }


    void handle_get() {
        MPI_Request get_requests[this->mpi_size];
        int index[this->mpi_size];

        for (int rank = 0; rank < this->mpi_size; rank++) {
            MPI_Irecv(&index[rank], 1, MPI_INT, rank, this->generate_get_tag(rank), MPI_COMM_WORLD, &get_requests[rank]);
        }

        int rank, flag;
        char send_buf[sizeof(T)];

        MPI_Status status;
        MPI_Request dummy;

        while (this->run_threads.load(memory_order_acquire)) {
            MPI_Testany(this->mpi_size, get_requests, &rank, &flag, &status);
            if (flag) {
                memcpy((void *) send_buf, &this->local_arr[this->local_index(index[rank])], sizeof(T));
                MPI_Isend(send_buf, sizeof(T), MPI_CHAR, rank, generate_get_tag(rank), MPI_COMM_WORLD, &dummy);
                MPI_Irecv(&index[rank], 1, MPI_INT, rank, this->generate_get_tag(rank), MPI_COMM_WORLD, &get_requests[rank]);
            } else {
                this_thread::sleep_for(chrono::seconds(1));
            }
        }
    }


private:
    int mpi_rank, mpi_size;
    int size, num_processes;

    int local_size;
    T *local_arr;

    atomic_bool run_threads;

    thread get_listener;

    unsigned int get_tag;
};
