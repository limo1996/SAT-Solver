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
#include "internal_types.h"

class Clause {
private:                            /* class clause contains */
    VariableSet var;		/* a set of variables */
public:
    explicit Clause(VariableSet _var);
    bool is_true();
    bool is_false();
    VariableSet* get_vars();
    std::string to_string();
};

#endif // CLAUSE_H
