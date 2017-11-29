#ifndef SAT_SLAVESOLVER_WORKER_H
#define SAT_SLAVESOLVER_WORKER_H


#include <stdexcept>
#include <vector>
#include <climits>
#include "CNF.h"
#include "dpll.h"
#include "mpi_types.h"
#include "internal_types.h"

class Worker {
protected:
    CNF *cnf;
    MPI_Datatype meta_data_type;
    int my_rank;

public:
    virtual void dpll_callback(std::unordered_set<Variable *> *variables) = 0;
};


#endif //SAT_SLAVESOLVER_WORKER_H

