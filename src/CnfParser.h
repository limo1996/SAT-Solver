/****************************************************************************
 *	Inspired by an implementation by:	Psallidas Fotis                     *
 *	A.M.:1115200600170                                                      *
 *	email:std06170@di.uoa.gr                                                *
 *	Before starting executing and reading                                   *
 * 	the code read the readme.txt file                                       *
 *  http://www.cs.columbia.edu/~fotis/Artificial_Intelligence/dpll/dpll.cpp *
  **************************************************************************/

#ifndef CNFPARSER_H
#define CNFPARSER_H

#include <unordered_set>
#include <string>
#include "Variable.h"
#include "Clause.h"
#include "Cnf.h"
#include "InternalTypes.h"

/**
 * Used to parse a .cnf file in the dimacs format
 */
class CNFParser {
private:
    VariableSet var;				/* a set of variables */
    ClauseSet Clauses;				/* a set of clauses */
    std::unordered_set<CNF*> cnfs;					/* a set of cnfs */
    std::fstream *input;					/* the .cnf file using dimacs format */
public:
    explicit CNFParser(char* filename);
    int parsing();
    std::unordered_set<CNF*> get_CNFS();
};
#endif // CNFPARSER_H
