//
//  Master.cpp
//  SAT-Solver
//
//  Created by Jakub Lichman on 10/24/17.
//  Copyright © 2017 Jakub Lichman. All rights reserved.
//

#include "Master.h"

/**
 * Creates new instance of master class.
 * @param ranks number of ranks(processes)
 * @my_rank rank of the master
 * @meta_type our custom mpi datatype used for inter process communication
 */
Master::Master(size_t ranks, int my_rank, MPI_Datatype meta_type){
    this->my_rank = my_rank;
    this->all_ranks = ranks;
    this->meta_type = meta_type;
}

/**
 * Sends meta data
 * @param to_rank targeted process.
 * @param message_type {0, 1, 10, 11, 12} consult documentation for message types
 * @param assigned_count size of the array of variables that will be send
 * @return the MPI Request on that we can wait for completion of the non-blocking send
 */
MPI_Request Master::send_meta(int to_rank, char message_type, unsigned assigned_count){
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
    
    MPI_Bcast(&meta, 1, this->meta_type, this->my_rank, 0, MPI_COMM_WORLD);
}

/**
 * Sends task to worker. Message value: 0
 * @param task model that will be send to worker to solve.
 * @param worker_rank rank of the targeted worker.
 */
void Master::send_task_to_worker(Model task, int worker_rank){
    send_meta(worker_rank, 0, task.get_size());
    send_model(task.get_variables(), task.get_size(), worker_rank);
}

/**
 * Sends model consisted of n(size) variables.
 * @param variables integer array of variable assignments that needs to be solved.
 * @param size of variables.
 * @param worker_rank rank of the targeted worker.
 */
void Master::send_model(unsigned int *variables, size_t size, int worker_rank){
    //TODO: Mpi send array...
}

/**
 * If previously was send success message of finding model than this method receives and stores it. Message value: 12
 */
void Master::get_model(){
    // MPI_Recv
}

/**
 * Listens to workers and if someone send a task than master adds it to the queue. Message value: 10
 */
void Master::add_new_task(){
    
}

/**
 * Prints final solution.
 */
void Master::print_solution(bool *flags, std::string filename, int format){
    std::streambuf *coutbuf = std::cout.rdbuf(); //save old buf
    std::ofstream out;
    
    if(flags[2]){
        out.open(filename);
        std::cout.rdbuf(out.rdbuf()); //redirect std::cout to output file
    }
    
    if(this->result) {
        format == 1 ? std::cout << " ->  satisfiable\n" : cout << "sat\n";
        if(flags[0] || flags[1])
            DPLL::print(this->final_result.get_clauses(), this->final_result.get_var(), flags[1], format);
    } else
        format == 1 ? std::cout << " ->  not satisfiable\n" : cout << "unsat\n";
    
    //if output path was specified than redirect output back to console
    if(flags[2])
        std::cout.rdbuf(coutbuf); //reset to standard output again
}

/**
 * Starts solving.
 */
void Master::start(){
    
}

/**
 * Listens and reacts to messages of workers.
 */
void Master::listen_to_workers(){
    
}
