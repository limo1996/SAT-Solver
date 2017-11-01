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

#include <set>
#include <string>

#include "variable.h"
#include "clause.h"

class CNF {                         /* class CNF contains */
private:
    std::set<Clause*> clauses;		/* a set of clauses */
public:
    CNF(std::set<Clause*> _clauses);
    CNF(CNF &_cnf);
    CNF();

    std::set<Variable*>* get_vars();
    std::set<Variable*>* get_model();
    std::set<Clause*>* get_clauses();
    void print();
};
#endif // CNF_H
