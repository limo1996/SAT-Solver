//
// Created by jan on 23.11.17.
//

#ifndef SAT_SOLVER_CONFIG_H
#define SAT_SOLVER_CONFIG_H

#include "worker.h"

class Worker;

class Config {
public:
    int cdcl_limit; // if there are only this many variables left unassigned, we run CDCL on the formula
    int branching_limit; // if there are only this many variables left unassigned, we do not branch further
    Worker *worker;
    SolverType solver_type;

    Config() {
        cdcl_limit = -1;
        branching_limit = -1;
        worker = nullptr;
        solver_type = DPLL_;
    }

    explicit Config(SolverType _solver_type) {
        cdcl_limit = -1;
        branching_limit = -1;
        worker = nullptr;
        solver_type = _solver_type;
    }

    Config(int _cdcl_limit, SolverType _solver_type) {
        cdcl_limit = _cdcl_limit;
        branching_limit = _cdcl_limit;
        worker = nullptr;
        solver_type = _solver_type;
    }

    Config(int _cdcl_limit, int _branching_limit, Worker *_worker, SolverType _solver_type) {
        cdcl_limit = _cdcl_limit;
        branching_limit = _branching_limit;
        worker = _worker;
        solver_type = _solver_type;
    }
};

#endif //SAT_SOLVER_CONFIG_H
