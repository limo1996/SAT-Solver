/****************************************************************************
 *	Inspired by an implementation by:	Psallidas Fotis                     *
 *	A.M.:1115200600170                                                      *
 *	email:std06170@di.uoa.gr                                                *
 *	Before starting executing and reading                                   *
 * 	the code read the readme.txt file                                       *
 *  http://www.cs.columbia.edu/~fotis/Artificial_Intelligence/dpll/dpll.cpp *
  **************************************************************************/

#include "Cnf.h"

CNF::CNF() = default;

//creates new instance of CNF
CNF::CNF(ClauseSet _clauses) {
    for(auto c : _clauses) {
        clauses.push_back(c);
    }
}

//creates copy of CNF
CNF::CNF(CNF &_cnf){
    ClauseSet::iterator it_c;

    for (auto c : *(_cnf.get_clauses())) {
        this->clauses.push_back(new Clause(*(c->get_vars())));
    }
}

//gets variables
VariableSet* CNF::get_vars() {
    auto *vars = new VariableSet();
    for (auto c : clauses) {
        for (auto v: *c->get_vars()) {
            vars->push_back(v);
        }
    }
    return vars;
}

VariableSet* CNF::get_model() {
    auto *model = new VariableSet();
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
                model->push_back(v);
            }
        }
    }
    return model;
}

//gets clauses
ClauseSet* CNF::get_clauses() {
    return &clauses;
}

void CNF::add_clause(Clause *clause) {
    for (auto v: *clause->get_vars()) {
        v->set_assigned(false);
        for (auto m: *get_model()) {
            if (v->get_name() == m->get_name()) {
                v->set_value(m->get_value());
                v->set_assigned(true);
            }
        }
    }
    clauses.push_back(new Clause(*clause->get_vars()));
}

void CNF::print() {
    ClauseSet::iterator it;
    for (it = clauses.begin(); it != clauses.end(); it++) {
        if (!(*it)->is_true()) {
            std::cout << "(";
            if (it != clauses.begin()) {
                std::cout << " and ";
            }
            if ((*it)->is_false()) {
                std::cout << " false ";
            }
            std::cout << (*it)->to_string();
            std::cout << ")";
        }
    }
    std::cout << std::endl;
}

VariableSet* CNF::get_partial_model() {
    auto *model = new VariableSet();
    for (auto c : clauses) {
        for (auto v: *c->get_vars()) {
            if (!v->get_assigned()) {
                continue;
            }
            bool already_contained = false;
            for (auto v_model : *model) {
                if (v_model->get_name() == v->get_name()) {
                    already_contained = true;
                    break;
                }
            }
            if (!already_contained) {
                model->push_back(v);
            }
        }
    }
    return model;
}

int CNF::count_un_assigned() {
    return (int) get_model()->size() - get_partial_model()->size();
}

CNF *CNF::build_fresh_cnf_from() {
    ClauseSet new_clauses;
    for (auto clause : clauses) {
        if (!clause->is_true()) {
            VariableSet new_clause;
            for (auto v : *clause->get_vars()) {
                if (!v->get_assigned()) {
                    new_clause.push_back(new Variable(v->get_sign(), v->get_value(), v->get_name()));
                }
            }
            new_clauses.push_back(new Clause(new_clause));
        }
    }
    return new CNF(new_clauses);
}

void set_variable_value(CNF *cnf, Variable *var, bool value) {
    for (auto variable : *cnf->get_vars()) {
        if (variable->get_name() == var->get_name()) {
            variable->set_value(value);
            variable->set_assigned(true);
        }
    }
}

void CNF::overwrite_assignments(VariableSet *partial_model) {
    for (auto pv : *partial_model) {
        set_variable_value(this, pv, pv->get_value());
    }
}
