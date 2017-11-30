#ifndef SAT_STEALING_WORKER_H
#define SAT_STEALING_WORKER_H

#include "CNF.h"
#include "dpll.h"
#include "mpi_types.h"
#include "Model.h"
#include "worker.h"
#include <stdexcept>
#include <vector>
#include <list>

class StealingWorker : Worker{
private:
    std::list<std::vector<unsigned> > stack;                                            // local stack of models to process (can be stealed)
    int workers_size;                                                                   // number of workers (processes)
    int next_to_send;                                                                   // rank of next worker, where worker 0 will send next subproblem
    bool stop;                                                                          // indicates whether this worker was stopped
    
    void run_dpll();                                                                    // runs dpll on this->cnf. Every branch is resolved by dpll_callback
    void get_model();                                                                   // gets next model to solve either by getting it from local queue or by stealing it.
    void stop_workers();                                                                // sends stop message to all workers
    void output_sat_model(CNF *cnf);                                                    // outputs model passed as parameter
    void print_sat_stop_workers(CNF *cnf);                                              // prints sat, model and stops workers
    void parse_and_update_variables(unsigned[], int size);                              // updates this->cnf with new values passed as paramters
    void debug_output(std::string line, bool newLine, int level = 1);                   // outputs string according to set debug_level
    void cerr_model(std::string info, std::unordered_set<Variable *> *variables);       // outputs model according to set debug_level
    
    MPI_Request send_meta(int to_rank, char i, unsigned assigned);                      // sends meta data to given rank with given values
    MPI_Request send_model(int to_rank, std::vector<unsigned int> assigned);            // sends model to given rank
    
    unsigned count_assigned(std::unordered_set<Variable *> *variables);                 // returns number of assigned variables
    bool stop_received_before_message_completion(MPI_Request *mpi_requests, int size);  // tests whether were all requests sends and if there is new message than processed it
    std::vector<unsigned> encode_variables(std::unordered_set<Variable *> *variables);  // encode variables to sendable format
    
    bool respond_to(struct meta meta, MPI_Status status);                               // performs action according to received meta message
    
public:
    explicit StealingWorker(CNF _cnf, MPI_Datatype _meta_data_type, int _my_rank, int _workers_size);
    virtual void dpll_callback(std::unordered_set<Variable *> *variables);              // function that is called whenever dpll makes branching.
    bool check_and_process_message_from_worker(bool wait, int spinForMessage = -1);     // listens and responds for messages from other workers. When spinForMessage != 1 than receives and responds
                                                                                        // while received msg != spinForMesage
    void start();                                                                       // Worker starts to solve cnf and sends one subproblem to each other worker.
    bool stopped() { return stop; }                                                     // Returns true if worker is stopped, false otherwise
};


#endif //SAT_STEALING_WORKER_H