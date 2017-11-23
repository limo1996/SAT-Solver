#ifndef DPLL_H
#define DPLL_H

#include <unordered_set>
#include <iostream>
#include <stdexcept>
#include "variable.h"
#include "clause.h"
#include "CNF.h"
#include "worker.h"
#include "internal_types.h"

class Worker;

class Config {
public:
    int num_callbacks;
    Worker *worker;

    Config() {
        num_callbacks = -1;
        worker = nullptr;
    }

    Config(int _num_callbacks, Worker *_worker) {
        num_callbacks = _num_callbacks;
        worker = _worker;
    }
};

class DpllResult{
public:
    bool sat;
    CNF *model_cnf;
    DpllResult(bool _sat, CNF *_model_cnf) {
        sat = _sat;
        model_cnf =_model_cnf;
    }
};

class DPLL {
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
    bool DPLL_SATISFIABLE();
    static void output_model(VariableSet *vars);
    CNF *get_cnf();


    static void set_variable_value(CNF *cnf, Variable *var, bool value);
    static void unset_variable_value(CNF *cnf, Variable *var);

    static void cout_clauses(ClauseSet *clauses);
};

#endif // DPLL_H
