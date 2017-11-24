#ifndef SAT_SLAVESOLVER_WORKER_H
#define SAT_SLAVESOLVER_WORKER_H


#include "CNF.h"
#include "dpll.h"
#include "mpi_types.h"
//#include <stdexcept>
#include <vector>

class Worker {
protected:
    CNF *cnf;
    MPI_Datatype meta_data_type;
    int my_rank;
    
    /*bool stop;
    virtual void send_sat(CNF *cnf);
    virtual void send_unsat();
    
    void run_dpll();
    
    void parse_and_update_variables(unsigned[], int size);
    
    std::vector<unsigned> encode_variables(std::unordered_set<Variable *> *variables);
    
    bool stop_received_before_message_completion(MPI_Request *mpi_requests, int size);
    
    void cerr_model(std::string info, std::unordered_set<Variable *> *variables);*/
    
public:
    virtual void dpll_callback(std::unordered_set<Variable *> *variables) = 0;
    //virtual void wait_for_instructions_from_master() = 0;
};


#endif //SAT_SLAVESOLVER_WORKER_H

