/****************************************************************************
 *	Inspired by an implementation by:	Psallidas Fotis                     *
 *	A.M.:1115200600170                                                      *
 *	email:std06170@di.uoa.gr                                                *
 *	Before starting executing and reading                                   *
 * 	the code read the readme.txt file                                       *
 *  http://www.cs.columbia.edu/~fotis/Artificial_Intelligence/dpll/dpll.cpp *
  **************************************************************************/

#include "clause.h"

#include <iostream>

//creates new instance of Clause
Clause::Clause(const std::set<Variable*> _var) {	/* constructor */
    std::set<Variable*>::iterator it_v;
    for(it_v=_var.begin() ; it_v != _var.end() ; it_v++){
        var.insert(*it_v);
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

//prints Clause
void Clause::print() {
    std::cout << this->to_string() << std::endl;
}

//gets variables in clause
std::set<Variable*>* Clause::get_vars() {
    return &this->var;
}

//gets clause in string
std::string Clause::to_string() {
    std::set<Variable*>::iterator it_v;
    std::string s;
    for(it_v = var.begin() ; it_v != var.end() ; it_v++){
        if (!(*it_v)->get_assigned()) {
            if (!(*it_v)->get_sign()) {
                s += "!";
            }

            s += (*it_v)->get_name();
            if ((++it_v) != var.end()) {
                s += " or ";
            }
            it_v--;
        }
    }
    return s;
}
