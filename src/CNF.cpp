/****************************************************************************
 *	Inspired by an implementation by:	Psallidas Fotis                     *
 *	A.M.:1115200600170                                                      *
 *	email:std06170@di.uoa.gr                                                *
 *	Before starting executing and reading                                   *
 * 	the code read the readme.txt file                                       *
 *  http://www.cs.columbia.edu/~fotis/Artificial_Intelligence/dpll/dpll.cpp *
  **************************************************************************/

#include "CNF.h"

#include <iostream>

CNF::CNF(){}

//creates new instance of CNF
CNF::CNF(std::set<Clause*> _clauses) {
    std::set<Variable*>::iterator it_v;
    std::set<Clause*>::iterator it_c;

    for(it_c=_clauses.begin(); it_c != _clauses.end(); it_c++) {
        clauses.insert(*it_c);
    }
}

//creates copy of CNF
CNF::CNF(CNF &_cnf){
    std::set<Clause*>::iterator it_c;

    for(it_c=_cnf.get_clauses()->begin() ; it_c != _cnf.get_clauses()->end() ; it_c++){ /* get the the clauses */
        this->clauses.insert(*it_c);
    }
}

//gets variables
std::set<Variable*>* CNF::get_vars() {
    auto *vars = new std::set<Variable*>();
    for (auto c : clauses) {
        for (auto v: *c->get_vars()) {
            vars->insert(v);
        }
    }
    return vars;
}

std::set<Variable*>* CNF::get_model() {
    auto *model = new std::set<Variable*>();
    for (auto c : clauses) {
        for (auto v: *c->get_vars()) {
            bool already_contained = false;
            for (auto v_model : *model) {
                if (v_model->get_name() == v->get_name()) {
                    already_contained = true;
                    break;
                }
            }
            if (!already_contained) {
                model->insert(v);
            }
        }
    }
    return model;
}

//gets clauses
std::set<Clause*>* CNF::get_clauses() {
    return &clauses;
}

void CNF::print() {
    std::set<Clause*>::iterator it;
    for (it = clauses.begin(); it != clauses.end(); it++) {
        std::cout << (*it)->to_string();
        it++;
        if (it != clauses.end()) {
            std::cout << " and ";
        }
        it--;
    }
    std::cout << std::endl;
}
