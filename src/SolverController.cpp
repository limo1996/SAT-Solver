//
// Created by jan on 23.11.17.
//

#include "SolverController.h"

SolverController::SolverController(Config *config, CNF *cnf) {
    if (config->solver_type == DPLL_) {
        solver = (Solver *) new DPLL(*cnf, config);
    } else {
        solver = (Solver *) new CDCL(cnf);
    }
}

bool SolverController::SATISFIABLE() {
    return solver->SATISFIABLE();
}

CNF *SolverController::get_cnf() {
    return solver->get_cnf();
}
