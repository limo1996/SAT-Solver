/****************************************************************************
 *	Inspired by an implementation by:	Psallidas Fotis                     *
 *	A.M.:1115200600170                                                      *
 *	email:std06170@di.uoa.gr                                                *
 *	Before starting executing and reading                                   *
 * 	the code read the readme.txt file                                       *
 *  http://www.cs.columbia.edu/~fotis/Artificial_Intelligence/dpll/dpll.cpp *
  **************************************************************************/
#ifndef CLAUSE_H
#define CLAUSE_H

#include "variable.h"

class Clause {
private:                            /* class clause contains */
    std::unordered_set<Variable*> var;		/* a set of variables */
public:
    Clause(std::unordered_set<Variable *> _var);
    void print();
    bool is_true();
    bool is_false();
    std::unordered_set<Variable*>* get_vars();
    std::string to_string();
};

#endif // CLAUSE_H
