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
#include <cassert>

#include "MpiTypes.h"
#include "Cnf.h"
#include "Dpll.h"
#include "Model.h"
#include "Meter.h"
#include "Measurement.h"

/**
 Master in parallel sat-solver. Responsible for managing worker threads e.g. collecting tasks that needs to be done,
 sending tasks to free workers, stopping all workers in case solution was found.
 */
class Master : public Meter{
private:
    std::queue<Model> states_to_process;                                        // Tasks that needs to be done
    std::queue<int> available_ranks;                                            // Free processes(threads)
    size_t all_ranks;                                                           // Number of processes
    int my_rank;                                                                // My process id
    MPI_Datatype meta_type;                                                     // MPI datatype used for interprocess comunication
    CNF final_result;                                                           // If result was found that this field contains final values of variables
    bool result;                                                                // Indicates whether the result was found
    Measurement *measurement;

    void add_new_task(int size, int rank);                                      // Listens to workers and if someone send a task than master adds
                                                                                // it to the queue. Message value: 10

    void send_task_to_worker(Model task, int worker_rank);                      // Sends task to worker. Message value: 0
    void stop_workers();                                                        // Prevens all workes from further work. Result found. Message type: 1
    void receive_and_log_measurements();                                                        // Prevens all workes from further work. Result found. Message type: 1
    void receive_and_output_sat_model(int size, int rank);

    MPI_Request send_meta(int to_rank, char message_type, unsigned assigned_count);    // Sends meta data
    MPI_Request send_model(unsigned int *variables, size_t size, int worker_rank);         // Sends model to worker.
public:
    Master(size_t ranks, int my_rank, MPI_Datatype meta_type, std::string cnf_path);    // Creates new instance of master class.
    bool listen_to_workers();                                                   // Listens and reacts to messages of workers.
    void start();                                                               // Starts solving.
};

#endif /* Master_hpp */
