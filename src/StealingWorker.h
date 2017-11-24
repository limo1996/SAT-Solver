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
    //CNF *cnf;
    //MPI_Datatype meta_data_type;
    std::list<std::unordered_set<Variable *> *> stack;
   // int my_rank;
    int workers_size;
    int next_to_send;
    bool stop;
    
    MPI_Request send_meta(int to_rank, char i, unsigned assigned);
    MPI_Request send_model(int to_rank, std::vector<unsigned int> assigned);
    void print_sat_stop_workers(CNF *cnf);
    void get_model();
    void run_dpll();
    void parse_and_update_variables(unsigned[], int size);
    std::vector<unsigned> encode_variables(std::unordered_set<Variable *> *variables);
    bool stop_received_before_message_completion(MPI_Request *mpi_requests, int size);
    void cerr_model(std::string info, std::unordered_set<Variable *> *variables);
    void output_sat_model(CNF *cnf);
    void stop_workers();
    unsigned count_assigned(std::unordered_set<Variable *> *variables);
    
public:
    explicit StealingWorker(CNF _cnf, MPI_Datatype _meta_data_type, int _my_rank, int _workers_size);
    virtual void dpll_callback(std::unordered_set<Variable *> *variables);
    bool check_and_process_message_from_worker(bool wait);
    void start();
};


#endif //SAT_STEALING_WORKER_H
