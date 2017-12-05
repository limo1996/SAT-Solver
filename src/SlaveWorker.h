#ifndef SAT_SLAVE_SOLVER_WORKER_H
#define SAT_SLAVE_SOLVER_WORKER_H


#include "CNF.h"
#include "dpll.h"
#include "mpi_types.h"
#include "worker.h"
#include <stdexcept>
#include <vector>
#include <chrono>
#include "internal_types.h"

using namespace std::chrono;

class SlaveWorker : Worker {
private:
    long long waitingTime;
    high_resolution_clock::time_point startTime;
    bool measurement_started;
    
    MPI_Request send_meta(char i, unsigned assigned);
    
    MPI_Request send_model(std::vector<unsigned int> assigned);
    
    void send_sat(CNF *cnf);
    
    void send_unsat();
    
    void run_dpll();
    
    void parse_and_update_variables(unsigned[], int size);
    
    std::vector<unsigned> encode_variables(VariableSet *variables);
    
    bool stop_received_before_message_completion(MPI_Request *mpi_requests, int size);
    
    void cerr_model(std::string info, VariableSet *variables);
    
    void start_measure();
    
    void stop_measure();
    
public:
    explicit SlaveWorker(CNF _cnf, MPI_Datatype _meta_data_type, int _worker_rank);
    
    virtual void dpll_callback(VariableSet *variables);
    
    void wait_for_instructions_from_master();
    
    int get_waiting_time() { return waitingTime; }
};


#endif //SAT_SLAVE_SOLVER_WORKER_H
