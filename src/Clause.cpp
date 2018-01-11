/****************************************************************************
 *	Inspired by an implementation by:	Psallidas Fotis                     *
 *	A.M.:1115200600170                                                      *
 *	email:std06170@di.uoa.gr                                                *
 *	Before starting executing and reading                                   *
 * 	the code read the readme.txt file                                       *
 *  http://www.cs.columbia.edu/~fotis/Artificial_Intelligence/dpll/dpll.cpp *
  **************************************************************************/

#include "Clause.h"

/**
 * creates a new instance of Clause
 */
Clause::Clause(VariableSet _var) {	/* constructor */
    for(auto v: _var){
        var.push_back(new Variable(*v));
    }
}

bool Clause::is_true() {
    // a clause is true if one of it's real values is true
    for (auto variable : var) {
        if (variable->get_assigned() && variable->get_real_value()) {
            return true;
        }
    }
    return false;
}

bool Clause::is_false() {
    // all variables have to be assigned
    for (auto variable : var) {
        if (!variable->get_assigned()) {
            return false;
        }
    }
    // if all are assigned, all real values have to be false
    for (auto variable : var) {
        if (variable->get_real_value()) {
            return false;
        }
    }
    return true;
}

//gets variables in clause
VariableSet* Clause::get_vars() {
    return &this->var;
}

//gets clause as string
std::string Clause::to_string() {
    std::string s;
    VariableSet unassignedVars;
    for(auto v : var){
        if (!v->get_assigned()) {
            unassignedVars.push_back(v);
        }
    }

    VariableSet::iterator it_v;
    for (it_v = unassignedVars.begin(); it_v != unassignedVars.end(); it_v++) {
        if (it_v != unassignedVars.begin()) {
            s += " or ";
        }

        if (!(*it_v)->get_sign()) {
            s += "!";
        }

        s += std::to_string((*it_v)->get_name());
    }
    return s;
}
