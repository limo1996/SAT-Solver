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
#include "variable.h"
#include "clause.h"
#include "CNF.h"
#include "internal_types.h"

class CNFParser {                           /* class CNFparse used to parse a .cnf file using dimacs format ,contains*/
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
