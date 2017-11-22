//
// Created by jan on 21.11.17.
//

#include "cdcl.h"

extern int CERR_LEVEL;

CDCL::CDCL(CNF _cnf) {
    cnf = new CNF(_cnf);
    dependency_graph = new Graph();
    parent_decision = nullptr;
}

bool CDCL::SATISFIABLE() {
    DpllResult *res = CDCLAlgorithm(cnf);
    cnf = res->model_cnf;
    return res->sat;
}

StandardLiteral *CDCL::create_standard_literal(unsigned name, bool sign) {
    auto *l = new StandardLiteral(name, sign, parent_decision);
    if (parent_decision != nullptr) {
        parent_decision->add_implies(l);
    }
    return l;
}

DecisionLiteral *CDCL::create_decision_literal(unsigned name, bool sign, CNF *cnf) {
    auto *l = new DecisionLiteral(name, sign, cnf);
    if (parent_decision != nullptr) {
        parent_decision->add_child(l);
        l->parents.insert((Literal*) parent_decision);
    }
    return l;
}

void CDCL::add_dependency_edges_to_graph(unsigned name, bool sign, Clause *clause) {
    Literal *l = create_standard_literal(name, sign);
    dependency_graph->add_node(l);
    for (auto v: *clause->get_vars()) {
        if (!(v->get_sign() == sign && v->get_name() == name)) {
            Literal *parent = dependency_graph->find_negation(v);
            if (parent != nullptr) {
                if (parent->name != name) {
                    parent->children.insert(l);
                    l->parents.insert(parent);
                }
            } else {
                throw std::runtime_error("there should exist some node here!");
            }
        }
    }
}

/**
 * Goes through the cnf formula and looks for a unit clause
 * @param cnf the cnf object that represents the original formula and a partial model (var->value assignment)
 * @return the unit clause if it found one, nullptr otherwise
 */
std::pair<Clause*, Variable*> CDCL::FIND_UNIT_CLAUSE(CNF *cnf) {
    for (auto c : *cnf->get_clauses()) {
        if (!c->is_true()) {
            long count = c->get_vars()->size();
            Variable *unasigned = nullptr;
            for (auto cv: *(c->get_vars())) {
                if (cv->get_assigned()) {
                    count--;
                } else {
                    unasigned = cv;
                }
            }
            if (count == 1) {
                return std::make_pair(c, unasigned);
            }
        }
    }
    return std::make_pair((Clause*) nullptr, (Variable*) nullptr);
}

DpllResult *CDCL::CDCLAlgorithm(CNF *cnf) {
    Variable *var;
    Clause *clause;
    std::set<Clause *> *clauses = cnf->get_clauses();
    std::set<Variable *> *vars = cnf->get_vars();
    if (CERR_LEVEL >= 3) {
        DPLL::cout_clauses(clauses);
        std::cerr << (dependency_graph->has_conflict ? "has conflict!" : "no conflict") << std::endl;
    }
    if (dependency_graph->has_conflict && parent_decision == nullptr) {
        return new DpllResult(false, nullptr);
    }
    if (dependency_graph->has_conflict) {
        CNF *new_cnf = conflict_resolution();
        return CDCL::CDCLAlgorithm(new_cnf);
    }
    if (DPLL::ALL_CLAUSES_ARE_TRUE(clauses))
        return new DpllResult(true, cnf);
    if (DPLL::ONE_CLAUSE_IS_FALSE(clauses))
        if (parent_decision == nullptr) {
            return new DpllResult(false, nullptr);
        } else {
            throw std::runtime_error("there should be a conflict if one clause is false!");
        }
    std::pair<Clause*, Variable*> uc = CDCL::FIND_UNIT_CLAUSE(cnf);
    clause = uc.first;
    var = uc.second;
    if (clause != nullptr) {
        DPLL::set_variable_value(cnf, var, var->get_sign());
        if (CERR_LEVEL >= 3) {
            std::cerr << "unit clause rule on " << var->to_string() << std::endl;
        }
        if (CERR_LEVEL >= 3) {
            DPLL::cout_clauses(cnf->get_clauses());
        }
        add_dependency_edges_to_graph(var->get_name(), var->get_sign(), clause);
        if (DPLL::ONE_CLAUSE_IS_FALSE(cnf->get_clauses())) {
            if (CERR_LEVEL >= 3) {
                std::cerr << "forcing conflict because of false clause" << std::endl;
            }
            Clause *false_clause;
            for (auto c : *clauses) {
                if (c->is_false()) {
                    false_clause = c;
                }
            }
            add_dependency_edges_to_graph(var->get_name(), !var->get_sign(), false_clause);
        }
        return CDCLAlgorithm(cnf);
    }
    var = DPLL::find_first_unassigned(vars);
    if (var == nullptr) {
        return new DpllResult(false, nullptr);
    } else {
        DecisionLiteral *l = create_decision_literal(var->get_name(), var->get_sign(), new CNF(*cnf));
        if (CERR_LEVEL >= 2) {
            std::cerr << "cdcl decision on " << l->to_string() << std::endl;
        }
        dependency_graph->add_node(l);
        parent_decision = l;
        DPLL::set_variable_value(cnf, var, var->get_sign());
        return CDCLAlgorithm(cnf);
    }
}

CNF *CDCL::get_cnf() {
    return cnf;
}

CNF *CDCL::conflict_resolution() {
    if (CERR_LEVEL >= 2) {
        std::cerr << "conflict resolution" << std::endl;
    }
    auto *p = (StandardLiteral*) dependency_graph->find(new Variable(true, false, dependency_graph->conflict));
    auto *n = (StandardLiteral*) dependency_graph->find(new Variable(false, false, dependency_graph->conflict));

    // cut the graph and retrieve a new clause that represents the conflict
    LiteralSet parents;
    parents.insert(p->parents.begin(), p->parents.end());
    parents.insert(n->parents.begin(), n->parents.end());
    std::set<Variable*> new_clause_variables;
    for (auto parent : parents) {
        new_clause_variables.insert(new Variable(!parent->sign, false, parent->name));
    }

    // add the new clause to all decision nodes (means also applying the model to them)
    for (auto l : dependency_graph->get_all_decision_literals()) {
        l->cnf->add_clause(new Clause(new_clause_variables));
    }
    cnf->add_clause(new Clause(new_clause_variables));
    if (CERR_LEVEL >= 3) {
        std::cerr << "learning clause: (" << (new Clause(new_clause_variables))->to_string() << ")" << std::endl;
    }

    // identify the relevant decision literals
    LiteralSet relevant;
    relevant.insert((Literal *) p->implied_by);
    relevant.insert((Literal *) n->implied_by);

    // pop one relevant literal and as many others as possible
    bool one_popped = false;
    while (parent_decision != nullptr) {
        if (relevant.find(parent_decision) != relevant.end()) {
            if (one_popped) {
                break;
            } else {
                one_popped = true;
            }
        }
        if (CERR_LEVEL >= 3) {
            std::cerr << "Undoing " << parent_decision->to_string() << std::endl;
        }
        if (parent_decision->parents.size() > 1) {
            throw std::runtime_error("decision literal has more than one parent!");
        }
        remove_all_consequences(parent_decision);
        if (parent_decision->parents.size() == 1) {
            parent_decision = (DecisionLiteral *) *(parent_decision->parents.begin());
        } else {
            parent_decision = nullptr;
        }
    }
    dependency_graph->has_conflict = false;
    if (parent_decision == nullptr) {
        return cnf;
    } else {
        return parent_decision->cnf;
    }
}

void CDCL::remove_all_consequences(DecisionLiteral *literal) {
    std::set<CNF*> cnfs;
    for (auto dl : dependency_graph->get_all_decision_literals()) {
        cnfs.insert(dl->cnf);
    }
    cnfs.insert(cnf);
    for (auto cnf_ : cnfs) {
        for (auto consequence : literal->implies) {
            DPLL::unset_variable_value(cnf_, new Variable(false, false, consequence->name));
        }
        DPLL::unset_variable_value(cnf_, new Variable(false, false, literal->name));
    }
    if (CERR_LEVEL >= 4) {
        if (!literal->implies.empty()) {
            std::cerr << "unsetting: ";
            for (auto consequence : literal->implies) {
                std::cerr << " " << consequence->to_string();
            }
            std::cerr << std::endl;
        } else {
            std::cerr << "nothing to unset!" << std::endl;
        }
    }
    dependency_graph->remove_node(literal);
}



