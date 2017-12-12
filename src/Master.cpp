//
//  Master.cpp
//  SAT-Solver
//
//  Created by Jakub Lichman on 10/24/17.
//  Copyright © 2017 Jakub Lichman. All rights reserved.
//

#include "Master.h"

extern int CERR_LEVEL;

void debug_output(std::string msg, bool newLine, int level = 1) {
    if(CERR_LEVEL >= level){
        std::cerr << "Master: " << msg;
        if(newLine)
            std::cerr << std::endl;
    }
}

/**
 * Creates new instance of master class.
 * @param ranks number of ranks(processes)
 * @my_rank rank of the master
 * @meta_type our custom mpi datatype used for inter process communication
 */
Master::Master(size_t ranks, int my_rank, MPI_Datatype meta_type, std::string cnf_path){

    assert(ranks > 1);

    this->my_rank = my_rank;
    this->all_ranks = ranks;
    this->meta_type = meta_type;
    this->measurement = new Measurement(cnf_path, "parallel");

    for(int i = 0; i < ranks; i++){
        if(i != my_rank){
            this->available_ranks.push(i);
        }
    }

    debug_output("created with rank: " + std::to_string(my_rank) + ", number of processes: " + std::to_string(ranks) + "(" + std::to_string(ranks - 1) + " workers)", true);
    debug_output("number of free processes: " + std::to_string(available_ranks.size()), true);
}

/**
 * Sends meta data
 * @param to_rank targeted process.
 * @param message_type {0, 1, 10, 11, 12} consult documentation for message types
 * @param assigned_count size of the array of variables that will be send
 * @return the MPI Request on that we can wait for completion of the non-blocking send
 */
MPI_Request Master::send_meta(int to_rank, char message_type, unsigned assigned_count){
    debug_output("Master: sending metadata... to rank: " + std::to_string(to_rank) + " msg type: " + std::to_string((int)message_type) + " count: " + std::to_string(assigned_count), true);

    struct meta meta;
    meta.message_type = message_type;
    meta.count = assigned_count;

    MPI_Request request;
    MPI_Isend(&meta, 1, this->meta_type, to_rank, 0, MPI_COMM_WORLD, &request);
    inc_send_messages(sizeof(struct meta));
    inc_send_meta_cout();
    return request;
}

/**
 * Prevents all workers from doing further work. Result found. Message type: 1
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
            MPI_Isend(&meta_copy, 1, this->meta_type, i, 1, MPI_COMM_WORLD, mpi_requests + count);
            inc_send_messages(sizeof(struct meta));
            inc_send_meta_cout();
            count++;
        }
    }
    debug_output("broadcast initiated", true);
    MPI_Waitall(size, mpi_requests, MPI_STATUS_IGNORE);
    debug_output("broadcast done", true);
}

/**
 * Receive and log measurements from all workers
 */
void Master::receive_and_log_measurements() {
    // add the measurement of the master:
    stop_runtime();
    std::vector<unsigned> master = {get_runtime(), get_waiting_time(), get_send_messages(), get_send_meta()};
    measurement->add_measurement(master);
    for (int i=0; i<all_ranks; i++) {
        if (i != my_rank) {
            unsigned data[NUM_MEASUREMENTS];
            MPI_Recv(&data, NUM_MEASUREMENTS, MPI_UNSIGNED, i, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            measurement->add_measurement(std::vector<unsigned>(data, data + NUM_MEASUREMENTS));
        }
    }
    measurement->write_to_files();
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
MPI_Request Master::send_model(unsigned *variables, size_t size, int worker_rank){
    MPI_Request request;
    MPI_Isend(variables, (int) size, MPI_UNSIGNED, worker_rank, 0, MPI_COMM_WORLD, &request);
    inc_send_messages(size * sizeof(unsigned));
    return request;
}

/**
 * Listens to workers and if someone send a task than master adds it to the queue. Message value: 10
 * @param size of the model to receive
 * @param rank from which to receive
 */
void Master::add_new_task(int size, int rank){
    unsigned* encoded_model = new unsigned[size];
    MPI_Recv(encoded_model, size, MPI_UNSIGNED, rank, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    inc_recv_messages(size * sizeof(unsigned));
    this->states_to_process.push(Model(encoded_model, size));
}

/**
 * Receives a sat model and outputs it
 * @param size of the model to receive
 * @param rank from which to receive
 */
void Master::receive_and_output_sat_model(int size, int rank) {
    unsigned* encoded_model = new unsigned[size];
    MPI_Recv(encoded_model, size, MPI_UNSIGNED, rank, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    inc_recv_messages(size * sizeof(unsigned));
    std::cout  << "sat" << std::endl;
    for (int i=0; i< size; i++) {
        std::string name = std::to_string(encoded_model[i] >> 1);
        bool encoded_val = encoded_model[i] % 2 == 1;
        std::cout <<name << " " << (encoded_val ? "t" : "f") << std::endl;
    }
}

/**
 * Starts solving.
 */
void Master::start(){
    debug_output("starts solving", true);
    send_meta(this->available_ranks.front(), 0, 0);
    this->available_ranks.pop();
}

/**
 * Listens and reacts to messages of workers.
 * @return returns true if solving is done, otherwise false.
 */
bool Master::listen_to_workers(){
    start_waiting();
    struct meta meta;
    MPI_Status status;
    MPI_Recv(&meta, 1, this->meta_type, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    stop_waiting();
    inc_recv_messages(sizeof(struct meta));

    switch(meta.message_type){
        case 10:
            add_new_task(meta.count, status.MPI_SOURCE);
            debug_output("adding new task from worker: " + std::to_string(status.MPI_SOURCE) + " of lenght: " + std::to_string(meta.count), true);
            break;
        case 11:
            this->available_ranks.push(status.MPI_SOURCE);
            debug_output("rank " + std::to_string(status.MPI_SOURCE) + " is now free", true);
            break;
        case 12:
            receive_and_output_sat_model(meta.count, status.MPI_SOURCE);
            debug_output("sending bcast to stop", true);
            stop_workers();
            receive_and_log_measurements();
            return true;
    }

    if(this->states_to_process.empty() && this->available_ranks.size() == this->all_ranks - 1){
        debug_output("done", true);

        // no one seems to have found a model, so lets output unsat...
        std::cout << "unsat" << std::endl;
        stop_workers();
        receive_and_log_measurements();
        return true;
    }

    if(!this->available_ranks.empty()  && !this->states_to_process.empty()){
        debug_output("sending next task to worker: " + std::to_string(available_ranks.front()) + " task left: " + std::to_string(states_to_process.size()), true);

        send_task_to_worker(states_to_process.front(), available_ranks.front());
        states_to_process.pop();
        available_ranks.pop();
    }

    return false;
}
