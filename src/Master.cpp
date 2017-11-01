//
//  Master.cpp
//  SAT-Solver
//
//  Created by Jakub Lichman on 10/24/17.
//  Copyright © 2017 Jakub Lichman. All rights reserved.
//

#include "Master.h"

extern bool CERR_DEBUG;

/**
 * Creates new instance of master class.
 * @param ranks number of ranks(processes)
 * @my_rank rank of the master
 * @meta_type our custom mpi datatype used for inter process communication
 */
Master::Master(size_t ranks, int my_rank, MPI_Datatype meta_type){

    assert(ranks > 1);

    this->my_rank = my_rank;
    this->all_ranks = ranks;
    this->meta_type = meta_type;

    for(int i = 0; i < ranks; i++){
        if(i != my_rank){
            this->available_ranks.push(i);
        }
    }
    
    if(CERR_DEBUG){
        std::cerr << "Master: created with rank: " << my_rank << ", number of processes: " << ranks << "(" << ranks - 1 << " workers)" << std::endl;
        std::cerr << "Master: Number of free processes: " << available_ranks.size() << std::endl;
    }
}

/**
 * Sends meta data
 * @param to_rank targeted process.
 * @param message_type {0, 1, 10, 11, 12} consult documentation for message types
 * @param assigned_count size of the array of variables that will be send
 * @return the MPI Request on that we can wait for completion of the non-blocking send
 */
MPI_Request Master::send_meta(int to_rank, char message_type, unsigned assigned_count){
    if(CERR_DEBUG){
        std::cerr << "Master: sending metadata... to rank: " << to_rank << " msg type: " << (int)message_type << " count: " << assigned_count << std::endl;
    }
    
    struct meta meta;
    meta.message_type = message_type;
    meta.count = assigned_count;

    MPI_Request request;
    MPI_Isend(&meta, 1, this->meta_type, to_rank, 0, MPI_COMM_WORLD, &request);
    return request;
}

/**
 * Prevens all workes from further work. Result found. Message type: 1
 */
void Master::stop_workers(){
    struct meta meta;
    meta.message_type = 1;
    meta.count = 0;

    int size = static_cast<int>(all_ranks - 1);
    MPI_Request mpi_requests[size];
    int count = 0;
    for (int i=0; i<all_ranks; i++) {
        if (i != my_rank) {
            struct meta meta_copy = meta;
            MPI_Isend(&meta_copy, 1, this->meta_type, i, 0, MPI_COMM_WORLD, mpi_requests + count);
            count++;
        }
    }
    if (CERR_DEBUG) {
        std::cerr << "Master: broadcast initiated" << std::endl;
    }
    MPI_Waitall(size, mpi_requests, MPI_STATUS_IGNORE);
    if (CERR_DEBUG) {
        std::cerr << "Master: broadcast done" << std::endl;
    }
}

/**
 * Sends task to worker. Message value: 0
 * @param task model that will be send to worker to solve.
 * @param worker_rank rank of the targeted worker.
 */
void Master::send_task_to_worker(Model task, int worker_rank){
    MPI_Request mpi_requests[2];
    mpi_requests[0] = send_meta(worker_rank, 0, task.get_size());
    mpi_requests[1] = send_model(task.get_variables(), task.get_size(), worker_rank);
    MPI_Waitall(2, mpi_requests, MPI_STATUS_IGNORE);
}

/**
 * Sends model consisted of n(size) variables.
 * @param variables integer array of variable assignments that needs to be solved.
 * @param size of variables.
 * @param worker_rank rank of the targeted worker.
 */
MPI_Request Master::send_model(unsigned int *variables, size_t size, int worker_rank){
    MPI_Request request;
    MPI_Isend(variables, (int) size, MPI_UNSIGNED, worker_rank, 0, MPI_COMM_WORLD, &request);
    return request;
}

/**
 * Listens to workers and if someone send a task than master adds it to the queue. Message value: 10
 * @param size size of the incoming task
 */
void Master::add_new_task(int size, int rank){
    unsigned* encoded_model = new unsigned[size];
    MPI_Recv(encoded_model, size, MPI_UNSIGNED, rank, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    this->states_to_process.push(Model(encoded_model, size));
}

/**
 * Prints final solution.
 */
void Master::print_solution(bool *flags, std::string filename, int format){
    /*
    std::streambuf *coutbuf = std::cout.rdbuf(); //save old buf
    std::ofstream out;

    if(flags[2]){
        out.open(filename);
        std::cout.rdbuf(out.rdbuf()); //redirect std::cout to output file
    }

    if(this->result) {
        format == 1 ? std::cout << " ->  satisfiable\n" : std::cout << "sat\n";
        if(flags[0] || flags[1])
            DPLL::print(this->final_result.get_clauses(), this->final_result.get_vars(), flags[1], format);
    } else
        format == 1 ? std::cout << " ->  not satisfiable\n" : std::cout << "unsat\n";

    //if output path was specified than redirect output back to console
    if(flags[2])
        std::cout.rdbuf(coutbuf); //reset to standard output again
        */
}

/**
 * Starts solving.
 */
void Master::start(){
    if(CERR_DEBUG){
        std::cerr << "Master: starts solving" << std::endl;
    }
    send_meta(this->available_ranks.front(), 0, 0);
    this->available_ranks.pop();
}

/**
 * Listens and reacts to messages of workers.
 * @return returns true if solving is done, otherwise false.
 */
bool Master::listen_to_workers(){
    struct meta meta;
    MPI_Status status;
    MPI_Recv(&meta, 1, this->meta_type, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

    switch(meta.message_type){
        case 10:
            add_new_task(meta.count, status.MPI_SOURCE);
            if (CERR_DEBUG) {
                std::cerr << "Master: adding new task from worker: " << status.MPI_SOURCE
                          << " of lenght: " << meta.count << std::endl;
            }
            break;
        case 11:
            this->available_ranks.push(status.MPI_SOURCE);
            if (CERR_DEBUG) {
                std::cerr << "Master: rank " << status.MPI_SOURCE << " is now free" << std::endl;
            }
            break;
        case 12:
            if (CERR_DEBUG) {
                std::cerr << "Master: sending bcast to stop" << std::endl;
            }
            stop_workers();
            return true;
    }

    if(this->states_to_process.empty() && this->available_ranks.size() == this->all_ranks - 1){
        if (CERR_DEBUG) {
            std::cerr << "done" << std::endl;
        }
        // no one seems to have found a model, so lets output unsat...
        std::cout << "unsat" << std::endl;
        stop_workers();
        return true;
    }

    if(!this->available_ranks.empty()  && !this->states_to_process.empty()){
        if (CERR_DEBUG) {
            std::cerr << "Master: sending next task to worker: " << available_ranks.front()
                      << " task left: " << states_to_process.size() << std::endl;
        }
        send_task_to_worker(states_to_process.front(), available_ranks.front());
        states_to_process.pop();
        available_ranks.pop();
    }

    return false;
}
