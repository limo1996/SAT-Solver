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

#include "variable.h"
#include "clause.h"
#include "CNF.h"

#include <set>
#include <string>

class CNFParser {                           /* class CNFparse used to parse a .cnf file using dimacs format ,contains*/
private:
    std::set<Variable*> var;				/* a set of variables */
    std::set<Clause*> Clauses;				/* a set of clauses */
    std::set<CNF*> cnfs;					/* a set of cnfs */
    std::fstream *input;					/* the .cnf file using dimacs format */
public:
    CNFParser(char* filename);
    bool fix_pureness(const Variable *v);
    int parsing();
    std::set<CNF*> get_CNFS();
};
#endif // CNFPARSER_H
