#ifndef SAT_SLAVESOLVER_WORKER_H
#define SAT_SLAVESOLVER_WORKER_H


#include <stdexcept>
#include <vector>
#include <climits>
#include "CNF.h"
#include "dpll.h"
#include "mpi_types.h"
#include "internal_types.h"
#include "Meter.h"
class Worker : public Meter{
protected:
    CNF *cnf;                                                                               // parsed cnf
    MPI_Datatype meta_data_type;                                                            // MPi datatype used for communication
    int my_rank;                                                                            // rank of process working is running on
    bool stop;                                                                              // indicates whether worker was stopped

public:
    virtual void dpll_callback(VariableSet *variables) = 0;              // callback that is called when dpll branches
    
    Worker() {
        stop = false;
    }
};


#endif //SAT_SLAVESOLVER_WORKER_H

