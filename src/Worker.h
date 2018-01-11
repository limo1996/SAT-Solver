#ifndef SAT_SLAVESOLVER_WORKER_H
#define SAT_SLAVESOLVER_WORKER_H

#include <stdexcept>
#include <vector>
#include <climits>
#include "Cnf.h"
#include "Dpll.h"
#include "MpiTypes.h"
#include "InternalTypes.h"
#include "Meter.h"

/**
 * Worker base class
 *
 * Inherits from Meter to make the fields of Meter available in the worker.
 */
class Worker : public Meter {
protected:
    CNF *cnf;                                                                               // parsed cnf
    MPI_Datatype meta_data_type;                                                            // MPi datatype used for communication
    int my_rank;                                                                            // rank of process working is running on
    bool stop;                                                                              // indicates whether worker was stopped

public:
    Worker();
    void send_measurements();
    unsigned count_assigned(VariableSet *variables);
    std::vector<unsigned> encode_variables(VariableSet *variables);
    void parse_and_update_variables(unsigned[], int size);
    bool get_stop();

    virtual void dpll_callback(VariableSet *variables) = 0;              // callback that is called when dpll branches

};

#endif //SAT_SLAVESOLVER_WORKER_H
