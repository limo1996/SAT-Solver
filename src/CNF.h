/****************************************************************************
 *	Implemented by:	Psallidas Fotis                                         *
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
    std::set<Variable*> var;		/* a set of variables */
    std::set<Clause*> clauses;		/* a set of clauses */
    std::string sentence;           /* the whole cnf as a string */
public:
    CNF(std::set<Variable*> _var, std::set<Clause*> _clauses ,std::string _sentence);
    CNF(CNF &_cnf);
    CNF();

    std::set<Variable*>* get_var();
    std::set<Clause*>* get_clauses();
    std::string get_sentence() const;
    void print_var();
};
#endif // CNF_H
