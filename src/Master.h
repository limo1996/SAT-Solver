//
//  Master.hpp
//  SAT-Solver
//
//  Created by Jakub Lichman on 10/24/17.
//  Copyright © 2017 Jakub Lichman. All rights reserved.
//

#ifndef Master_hpp
#define Master_hpp

#include <queue>
#include <string>
#include <iostream>
#include <fstream>

#include "mpi_types.h"
#include "CNF.h"
#include "dpll.h"
#include "State.h"

/*
 Master in parallel sat-solver. Responsible for managing worker threads e.g. collecting tasks that needs to be done,
 sending tasks to free workers, stoping all workers in case solution was found.
 */
class Master{
private:
    std::queue<State> states_to_process;                                        // Tasks that needs to be done
    std::queue<int> available_ranks;                                            // Free processes(threads)
    size_t all_ranks;                                                           // Number of processes
    int my_rank;                                                                // My process id
    MPI_Datatype meta_type;                                                     // MPI datatype used for interprocess comunication
    CNF final_result;                                                           // If result was found that this field contains final values of variables
    bool result;                                                                // Indicates whether the result was found
    
    void add_new_task();                                                        // Listens to workers and if someone send a task than master adds
                                                                                // it to the queue. Message value: 10
    void get_model();                                                           // If previously was send success message of finding model
                                                                                // than this method receives and stores it. Message value: 12
    
    void send_task_to_worker(State task, int worker_rank);                      // Sends task to worker. Message value: 0
    void stop_workers();                                                        // Prevens all workes from further work. Result found. Message type: 1
    
    void send_meta(int to_rank, char message_type, unsigned assigned_count);    // Sends meta data
    void send_model(unsigned *variables, size_t size, int worker_rank);         // Sends model to worker.
public:
    Master(size_t ranks, int my_rank, MPI_Datatype meta_type);                  // Creates new instance of master class.
    void print_solution(bool* flags, std::string filename, int format);         // Prints final solution.
    void listen_to_workers();                                                   // Listens and reacts to messages of workers.
    void start();                                                               // Starts solving.
};

#endif /* Master_hpp */
