//
// Created by jan on 23.11.17.
//

#ifndef SAT_SOLVER_SOLVER_CONTROLLER_H
#define SAT_SOLVER_SOLVER_CONTROLLER_H

#include "solver.h"
#include "cdcl.h"

class SolverController {
private:
    Solver *solver;
public:
    SolverController(Config *config, CNF *_cnf);
    bool SATISFIABLE();
    CNF *get_cnf();
};

#endif //SAT_SOLVER_SOLVER_CONTROLLER_H
