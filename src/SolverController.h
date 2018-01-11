//
// Created by jan on 23.11.17.
//

#ifndef SAT_SOLVER_SOLVER_CONTROLLER_H
#define SAT_SOLVER_SOLVER_CONTROLLER_H

#include "Solver.h"
#include "Cdcl.h"

/**
 * Solver implementation that picks CDCL or DPLL depending on the provided config
 */
class SolverController: public Solver {
private:
    Solver *solver;
public:
    SolverController(Config *config, CNF *_cnf);
    bool SATISFIABLE();
    CNF *get_cnf();
};

#endif //SAT_SOLVER_SOLVER_CONTROLLER_H
