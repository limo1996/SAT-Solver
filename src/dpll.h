#ifndef DPLL_H
#define DPLL_H

#include "variable.h"
#include "clause.h"
#include "CNF.h"
#include "worker.h"

#include <set>
#include <iostream>
#include <stdexcept>

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
    Variable* find_first_unassigned(std::set<Variable*> *vars);
    bool ALL_CLAUSES_ARE_TRUE(std::set<Clause*> *clauses);
    bool ONE_CLAUSE_IS_FALSE(std::set<Clause*> *clauses);
    Variable* FIND_UNIT_CLAUSE(CNF *cnf);
    Variable* FIND_PURE_VAR(CNF *cnf);
    DpllResult * DPLLalgorithm(CNF *cnf);
    DpllResult * branch_on_variable(Variable *var, CNF *cnf);

public:
    DPLL(CNF _cnf, Config *_config);
    bool DPLL_SATISFIABLE();
    static void output_model(std::set<Variable*> *vars);
    //static void print(std::set<Clause*> *clauses , std::set<Variable*> *vars, bool extended, int format);
    CNF *get_cnf();


    void set_variable_value(CNF *cnf, Variable *var, bool value);
    void unset_variable_value(CNF *cnf, Variable *var);
};

#endif // DPLL_H
