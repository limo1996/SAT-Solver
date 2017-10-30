#ifndef SAT_SOLVER_WORKER_H
#define SAT_SOLVER_WORKER_H


#include "CNF.h"
#include "dpll.h"
#include "mpi_types.h"
#include <stdexcept>
#include <vector>

class Worker {
private:
    CNF *cnf;

    MPI_Datatype meta_data_type;

    int worker_rank;

    MPI_Request send_meta(char i, unsigned assigned);

    MPI_Request send_model(std::vector<unsigned int> assigned);

    void send_sat(CNF *cnf);

    void send_unsat();

    void run_dpll();

    void parse_and_update_variables(unsigned[], int size);

    std::vector<unsigned> encode_variables(std::set<Variable *> *variables);

    bool received_message_from_master();

    void wait_for_instructions_from_master();

public:
    explicit Worker(CNF _cnf, MPI_Datatype _meta_data_type, int _worker_rank);

    void dpll_callback(std::set<Variable *> *variables);
};


#endif //SAT_SOLVER_WORKER_H
