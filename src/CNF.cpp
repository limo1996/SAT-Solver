#include "cnf.h"

#include <iostream>

//creates new instance of CNF
CNF::CNF(std::set<Variable*> _var, std::set<Clause*> _clauses , std::string _sentence) {
    std::set<Variable*>::iterator it_v;
    std::set<Clause*>::iterator it_c;

    for (it_v=_var.begin(); it_v != _var.end(); it_v++) {
        var.insert(*it_v);
    }

    for(it_c=_clauses.begin(); it_c != _clauses.end(); it_c++) {
        clauses.insert(*it_c);
    }
    sentence = _sentence;
}

//creates copy of CNF
CNF::CNF(CNF &_cnf){
    std::set<Variable*>::iterator it_v;
    std::set<Clause*>::iterator it_c;

    for(it_v = _cnf.get_var()->begin(); it_v != _cnf.get_var()->end(); it_v++) {        /* get the variables */
        this->var.insert(*it_v);
    }

    for(it_c=_cnf.get_clauses()->begin() ; it_c != _cnf.get_clauses()->end() ; it_c++){ /* get the the clauses */
        this->clauses.insert(*it_c);
    }

    this->sentence = _cnf.get_sentence();                                               /* get the whole cnf sentence */
}

//gets variables
std::set<Variable*>* CNF::get_var() {
    return &var;
}

//gets clauses
std::set<Clause*>* CNF::get_clauses() {
    return &clauses;
}

//gets sentense
std::string CNF::get_sentence() const{
    return sentence;
}

//prints itself
void CNF::print_var() {
    std::set<Variable*>::iterator it_v;
    std::cout << sentence << std::endl;
    for(it_v = var.begin(); it_v != var.end();  it_v++)
        (*it_v)->print();
}
