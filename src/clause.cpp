/****************************************************************************
 *	Implemented by:	Psallidas Fotis                                         *
 *	A.M.:1115200600170                                                      *
 *	email:std06170@di.uoa.gr                                                *
 *	Before starting executing and reading                                   *
 * 	the code read the readme.txt file                                       *
 *  http://www.cs.columbia.edu/~fotis/Artificial_Intelligence/dpll/dpll.cpp *
  **************************************************************************/

#include "clause.h"

#include <iostream>

//creates new instance of Clause
Clause::Clause(const std::set<Variable*> _var) : value(true), assigned(false) {	/* constructor */
    std::set<Variable*>::iterator it_v;
    for(it_v=_var.begin() ; it_v != _var.end() ; it_v++){
        var.insert(*it_v);
    }
    value = true;
}

// prints i-th Clause
void Clause::print(int i) {
    std::cout << "clause " << i << ": ";
    std::set<Variable*>::iterator it_v, it_v2;
    for(it_v = var.begin() ; it_v != var.end() ; it_v++){
        if((*it_v)->get_sign() == false)
            std::cout << "~";
        std::cout << (*it_v)->get_name();
        if((++it_v) != var.end())
            std::cout << "^";
        it_v--;
    }
    std::cout << std::endl;
}

//prints Clause
void Clause::print() {
    std::set<Variable*>::iterator it_v,it_v2;
    for(it_v = var.begin() ; it_v != var.end() ; it_v++){
        if((*it_v)->get_sign()==false)
            std::cout << "~";
        std::cout << (*it_v)->get_name();
        if((++it_v) != var.end())
            std::cout << "^";
        it_v--;
    }
}

//gets whether is clause assigned
bool Clause::get_assigned() {
    return assigned;
}

//gets value
bool Clause::get_value() {
    return value;
}

//sets value
void Clause::set_value(bool _value) {
    value = _value;
}

//sets whether is clause assigned
void Clause::set_assigned(bool _assigned) {
    assigned = _assigned;
}

//gets variables in clause
std::set<Variable*>* Clause::get_var() {
    return &this->var;
}

//gets clause in string
std::string Clause::get_unit() {
    std::set<Variable*>::iterator it_v,it_v2;
    std::string unit;
    for(it_v = var.begin() ; it_v != var.end() ; it_v++){
        if(!(*it_v)->get_sign())
            unit += "~";

        unit += (*it_v)->get_name();
        if((++it_v) != var.end())
            unit+=" ^ ";
        it_v--;
    }
    return unit;
}
