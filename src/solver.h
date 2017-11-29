//
// Created by jan on 23.11.17.
//

#ifndef SAT_SOLVER_SOLVER_H
#define SAT_SOLVER_SOLVER_H

#include "CNF.h"

enum SolverType {
    DPLL_, CDCL_
};

class Solver {
public:
    Solver() = default;
    virtual bool SATISFIABLE() = 0;
    virtual CNF *get_cnf() = 0;
};


#endif //SAT_SOLVER_SOLVER_H
