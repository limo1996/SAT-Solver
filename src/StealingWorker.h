#ifndef SAT_STEALING_WORKER_H
#define SAT_STEALING_WORKER_H

#include <stdexcept>
#include <vector>
#include <list>
#include "Cnf.h"
#include "MpiTypes.h"
#include "Model.h"
#include "Worker.h"
#include "Dpll.h"

//random
#include <ctime>
#include <chrono>
#include <random>
#include <functional>

class StealingWorker : public Worker{
private:
    std::list<std::vector<unsigned> > stack;                                            // local stack of models to process (can be stealed)
    int workers_size;                                                                   // number of workers (processes)
    int next_to_send;                                                                   // rank of next worker, where worker 0 will send next subproblem

    double stealing_ratio;                                                              // in %. Indicates how many % of workers should we try to steal from.
    int check_interval;                                                                 // interval of checking for other messages from other workers
    int check_counter;                                                                  // counts iterations
    int min_stack_size;                                                                 // size of the stack when is worker allowed to send model
    Config *config;

    void run_dpll();                                                                    // runs dpll on this->cnf. Every branch is resolved by dpll_callback
    void stop_workers();                                                                // sends stop message to all workers
    void output_sat_model(CNF *cnf);                                                    // outputs model passed as parameter
    void print_sat_stop_workers(CNF *cnf);                                              // prints sat, model and stops workers
    void debug_output(std::string line, bool newLine, int level = 1);                   // outputs string according to set debug_level
    void cerr_model(std::string info, VariableSet *variables);       // outputs model according to set debug_level

    MPI_Request send_meta(int to_rank, char i, unsigned assigned);                      // sends meta data to given rank with given values
    MPI_Request send_model(int to_rank, std::vector<unsigned int> assigned);            // sends model to given rank

    std::set<int> generate_rand_workers(int max, int n);                                // generates n unique random numbers in range (0, max)

    bool respond_to(struct meta meta, MPI_Status status);                               // performs action according to received meta message
    bool try_to_steal_from_n_workers(int n);                                            // tries to steal model from n workers. Returns true if succeeded.

public:
    explicit StealingWorker(CNF _cnf, MPI_Datatype _meta_data_type, int _my_rank, int _workers_size,
                            double stealing_ratio = 0.5, int check_interval = 1, int min_stack_size = 1);
    void set_config(Config *conf);
    virtual void dpll_callback(VariableSet *variables);              // function that is called whenever dpll makes branching.
    bool check_and_process_message_from_worker(bool wait, int spinForMessage = -1);     // listens and responds for messages from other workers. When spinForMessage != 1 than receives and responds
                                                                                        // while received msg != spinForMesage
    void start();                                                                       // Worker starts to solve cnf and sends one subproblem to each other worker.
    bool stopped() { return stop; }                                                     // Returns true if worker is stopped, false otherwise
    void get_model();                                                                   // gets next model to solve either by getting it from local queue or by stealing it.
};


#endif //SAT_STEALING_WORKER_H
