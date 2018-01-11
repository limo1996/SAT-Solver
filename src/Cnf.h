/****************************************************************************
 *	Inspired by an implementation by:	Psallidas Fotis                     *
 *	A.M.:1115200600170                                                      *
 *	email:std06170@di.uoa.gr                                                *
 *	Before starting executing and reading                                   *
 * 	the code read the readme.txt file                                       *
 *  http://www.cs.columbia.edu/~fotis/Artificial_Intelligence/dpll/dpll.cpp *
  **************************************************************************/

#ifndef CNF_H
#define CNF_H

#include <iostream>
#include <string>
#include "Variable.h"
#include "Clause.h"
#include "InternalTypes.h"

/**
 * Represents a CNF formula
 */
class CNF {
private:
    ClauseSet clauses;		/* a set of clauses */
public:
    CNF(ClauseSet _clauses);
    CNF(CNF &_cnf);
    CNF();

    VariableSet* get_vars();
    VariableSet* get_model();
    ClauseSet* get_clauses();
    int count_un_assigned();
    void print();
    CNF *build_fresh_cnf_from();
    VariableSet* get_partial_model();
    void overwrite_assignments(VariableSet *partial_model);

    void add_clause(Clause *clause);
};
#endif // CNF_H
