//
// Created by jan on 23.11.17.
//

#ifndef SAT_SOLVER_CONFIG_H
#define SAT_SOLVER_CONFIG_H

#include "Worker.h"

class Worker;

/**
 * Config object used in both CDCL and DPLL
 */
class Config {
public:
    int branching_limit; // any node only branches this number of times, then it will solve the remaining task locally
    bool force_cdcl; // use cdcl when solving locally
    Worker *worker;
    SolverType solver_type;

    Config() {
        force_cdcl = false;
        branching_limit = INT_MAX;
        worker = nullptr;
        solver_type = DPLL_;
    }

    explicit Config(SolverType _solver_type) {
        force_cdcl = false;
        branching_limit = INT_MAX;
        worker = nullptr;
        solver_type = _solver_type;
    }

    Config(bool _force_cdcl, int _branching_limit, Worker *_worker, SolverType _solver_type) {
        force_cdcl = _force_cdcl;
        branching_limit = _branching_limit;
        worker = _worker;
        solver_type = _solver_type;
    }

    Config(Worker *_worker, SolverType _solver_type) {
        force_cdcl = false;
        branching_limit = INT_MAX;
        worker = _worker;
        solver_type = _solver_type;
    }
};

#endif //SAT_SOLVER_CONFIG_H
