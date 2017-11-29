//
// Created by jan on 23.11.17.
//

#ifndef SAT_SOLVER_CONFIG_H
#define SAT_SOLVER_CONFIG_H

#include "worker.h"

class Worker;

class Config {
public:
    int num_callbacks;
    Worker *worker;
    SolverType solver_type;

    Config() {
        num_callbacks = -1;
        worker = nullptr;
        solver_type = DPLL_;
    }

    explicit Config(SolverType _solver_type) {
        num_callbacks = -1;
        worker = nullptr;
        solver_type = _solver_type;
    }

    Config(int _num_callbacks, Worker *_worker, SolverType _solver_type) {
        num_callbacks = _num_callbacks;
        worker = _worker;
        solver_type = _solver_type;
    }
};

#endif //SAT_SOLVER_CONFIG_H
