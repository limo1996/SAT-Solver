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

#include "Variable.h"
#include "InternalTypes.h"

/**
 * Represents a clause in a CNF formula
 */
class Clause {
private:
    VariableSet var;		/* a set of variables */
public:
    explicit Clause(VariableSet _var);
    bool is_true();
    bool is_false();
    VariableSet* get_vars();
    std::string to_string();
};

#endif // CLAUSE_H
