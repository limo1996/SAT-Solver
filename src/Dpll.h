#ifndef DPLL_H
#define DPLL_H

#include <unordered_set>
#include <iostream>
#include <stdexcept>
#include "Solver.h"
#include "Cdcl.h"
#include "Config.h"
#include "Variable.h"
#include "Clause.h"
#include "Cnf.h"
#include "Worker.h"
#include "InternalTypes.h"

class Worker;
class Solver;
class CDCL;

/**
 * Result of the solver, contains a boolean flag sat/unsat and if sat a model CNF object.
 * From this CNF object, the valid model can then be gathered with model_cnf.get_model()
 */
class DpllResult {
public:
    bool sat;
    CNF *model_cnf;
    DpllResult(bool _sat, CNF *_model_cnf) {
        sat = _sat;
        model_cnf =_model_cnf;
    }
};

/**
 * DPLL solver implementation
 */
class DPLL: public Solver {
private:
    CNF *cnf;
    Config *config;
    DpllResult * DPLLalgorithm(CNF *cnf);
    DpllResult * branch_on_variable(Variable *var, CNF *cnf);

public:
    static Variable* find_first_unassigned(VariableSet *vars);
    static bool ALL_CLAUSES_ARE_TRUE(ClauseSet *clauses);
    static bool ONE_CLAUSE_IS_FALSE(ClauseSet *clauses);
    static Variable* FIND_UNIT_CLAUSE(CNF *cnf);
    static Variable* FIND_PURE_VAR(CNF *cnf);
    DPLL(CNF _cnf, Config *_config);
    bool SATISFIABLE() override;
    static void output_model(VariableSet *vars);
    CNF *get_cnf() override;


    static void set_variable_value(CNF *cnf, Variable *var, bool value);
    static void unset_variable_value(CNF *cnf, Variable *var);

    static void cout_clauses(ClauseSet *clauses);
};

#endif // DPLL_H
