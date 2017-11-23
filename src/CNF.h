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
#include "variable.h"
#include "clause.h"
#include "internal_types.h"

class CNF {                         /* class CNF contains */
private:
    ClauseSet clauses;		/* a set of clauses */
public:
    CNF(ClauseSet _clauses);
    CNF(CNF &_cnf);
    CNF();

    VariableSet* get_vars();
    VariableSet* get_model();
    ClauseSet* get_clauses();
    void print();

    void add_clause(Clause *clause);
};
#endif // CNF_H
