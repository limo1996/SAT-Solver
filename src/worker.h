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

    void send_measurements() {
        stop_runtime();
        unsigned data[4];
        data[0] = get_runtime();
        data[1] = get_waiting_time();
        data[2] = get_send_messages();
        data[3] = get_send_meta();
        MPI_Send(data, 4, MPI_UNSIGNED, 0, 2, MPI_COMM_WORLD);
    }
};


#endif //SAT_SLAVESOLVER_WORKER_H

