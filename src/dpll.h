#ifndef DPLL_H
#define DPLL_H

#include "variable.h"
#include "clause.h"
#include "cnf.h"

#include <set>

class DPLL {
private:
    CNF *cnf;
    void restore_symbol(Variable *v );
    bool ALL_VARIABLES_ARE_FALSE(Clause* cl);
    bool ONE_VARIABLE_IS_TRUE(Clause* cl);
    void fix_clauses(Variable *var, std::set<Clause*> *clauses , bool lvalue , bool flag = true , bool unit = false);
    void restore_clauses(Variable* var, std::set<Clause*> *clauses);
    void fix_variables(Variable *v, std::set<Variable*>* var, bool u = false);
    void restore_variables(Variable *v, std::set<Variable*>* var);
    void fix_pures(std::set<Variable*> *vars, std::set<Clause*> *clauses);
    Variable* find_first_unassigned(std::set<Variable*> *vars);
    bool ALL_CLAUSES_ARE_TRUE(std::set<Clause*> *clauses);
    bool ONE_CLAUSE_IS_FALSE(std::set<Clause*> *clauses);
    Variable* FIND_UNIT_CLAUSE(std::set<Clause*> *clauses , std::set<Variable*> *vars);
    Variable* FIND_PURE_SYMBOL(std::set<Variable*> *var);
    bool DPLLalgorithm(std::set<Variable*> *vars, std::set<Clause*> *clauses);

public:
    DPLL(CNF _cnf);
    bool DPLL_SATISFIABLE();
    void print(std::set<Clause*> *clauses , std::set<Variable*> *vars);
};

#endif // DPLL_H
