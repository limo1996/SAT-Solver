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
CNF::CNF(std::unordered_set<Clause*> _clauses) {
    std::unordered_set<Variable*>::iterator it_v;
    std::unordered_set<Clause*>::iterator it_c;

    for(it_c=_clauses.begin(); it_c != _clauses.end(); it_c++) {
        clauses.insert(*it_c);
    }
}

//creates copy of CNF
CNF::CNF(CNF &_cnf){
    std::unordered_set<Clause*>::iterator it_c;

    for(it_c=_cnf.get_clauses()->begin() ; it_c != _cnf.get_clauses()->end() ; it_c++){ /* get the the clauses */
        this->clauses.insert(new Clause(*(*it_c)->get_vars()));
    }
}

//gets variables
std::unordered_set<Variable*>* CNF::get_vars() {
    auto *vars = new std::unordered_set<Variable*>();
    for (auto c : clauses) {
        for (auto v: *c->get_vars()) {
            vars->insert(v);
        }
    }
    return vars;
}

std::unordered_set<Variable*>* CNF::get_model() {
    auto *model = new std::unordered_set<Variable*>();
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
std::unordered_set<Clause*>* CNF::get_clauses() {
    return &clauses;
}

void CNF::print() {
    std::unordered_set<Clause*>::iterator it;
    for (it = clauses.begin(); it != clauses.end(); it++) {
        if (it != clauses.begin()) {
            std::cout << " and ";
        }
        std::cout << (*it)->to_string();
    }
    std::cout << std::endl;
}
