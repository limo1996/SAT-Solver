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
    bool callback_on_branch;
    Worker *worker;

    //void (*callback)(std::set<Variable *> *vars);
    Config(bool _callback_on_branch) {
        callback_on_branch = _callback_on_branch;
        worker = nullptr;
    }

    Config(bool _callback_on_branch, Worker *_worker) {
        callback_on_branch = _callback_on_branch;
        worker = _worker;
    }
};

class DPLL {
private:
    CNF *cnf;
    Config *config;
    void restore_symbol(Variable *v );
    Variable* find_first_unassigned(std::set<Variable*> *vars);
    bool ALL_CLAUSES_ARE_TRUE(std::set<Clause*> *clauses);
    bool ONE_CLAUSE_IS_FALSE(std::set<Clause*> *clauses);
    Variable* FIND_UNIT_CLAUSE(CNF *cnf);
    Variable* FIND_PURE_VAR(CNF *cnf);
    bool DPLLalgorithm(CNF *cnf);
    bool branch_on_variable(Variable *var, CNF *cnf);

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
