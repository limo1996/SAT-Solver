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

    void send_meta(char i, unsigned assigned);

    void send_model(std::vector<unsigned int> assigned);

    void send_sat(CNF *cnf);

    void send_unsat();

    void run_dpll();

    void parse_and_update_variables(unsigned[], int size);

    std::vector<unsigned> encode_variables(std::set<Variable *> *variables);

public:
    explicit Worker(CNF _cnf, MPI_Datatype _meta_data_type);

    void dpll_callback(std::set<Variable *> *variables);
};


#endif //SAT_SOLVER_WORKER_H
