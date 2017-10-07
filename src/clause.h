#ifndef CLAUSE_H
#define CLAUSE_H

#include <set>

#include "variable.h"

class Clause {
private:                            /* class clause contains */
    std::set<Variable*> var;		/* a set of variables */
    bool value;                     /* a value */
    bool assigned;                  /* a flag to know if it is assigned or not */
public:
    Clause(const std::set<Variable*> _var);
    void print(int i);
    void print();
    bool get_assigned();
    bool get_value();
    void set_value(bool _value);
    void set_assigned(bool _assigned);
    std::set<Variable*>* get_var();
    std::string get_unit();
};

#endif // CLAUSE_H
