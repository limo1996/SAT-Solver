//
// Created by jan on 21.11.17.
//

#include <queue>
#include "Cdcl.h"

extern int CERR_LEVEL;

CDCL::CDCL(CNF *_cnf) {
    cnf = new CNF(*_cnf);
    dependency_graph = new Graph();
    parent_decision = nullptr;
}

bool CDCL::SATISFIABLE() {
    DpllResult *res = CDCLAlgorithm(cnf);
    cnf = res->model_cnf;
    return res->sat;
}

/**
 * Helper function to great a standard literal
 */
StandardLiteral *CDCL::create_standard_literal(unsigned name, bool sign) {
    auto *l = new StandardLiteral(name, sign, parent_decision);
    if (parent_decision != nullptr) {
        parent_decision->add_implies(l);
    }
    return l;
}

/**
 * Helper function to great a decision literal
 */
DecisionLiteral *CDCL::create_decision_literal(unsigned name, bool sign) {
    auto *l = new DecisionLiteral(name, sign);
    if (parent_decision != nullptr) {
        parent_decision->add_child(l);
        l->parents.insert((Literal *) parent_decision);
    }
    return l;
}

/**
 * Adds the neccessary dependency edges to the graph after the unit clause rule was applied
 *
 * The edges are: per previously removed variable in the clause add an edge from the negation
 * of that variable. The negation of the variable is guaranteed to be in the graph.
 * @param name the name of the unit variable
 * @param sign the sign of the unit variable
 * @param clause the unit clause itself
 */
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
std::pair<Clause *, Variable *> CDCL::FIND_UNIT_CLAUSE(CNF *cnf) {
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
    return std::make_pair((Clause *) nullptr, (Variable *) nullptr);
}

DpllResult *CDCL::CDCLAlgorithm(CNF *cnf) {
    Variable *var;
    Clause *clause;
    ClauseSet *clauses = cnf->get_clauses();
    VariableSet *vars = cnf->get_vars();
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
    if (DPLL::ONE_CLAUSE_IS_FALSE(clauses)) {
        if (parent_decision == nullptr) {
            return new DpllResult(false, nullptr);
        } else
            throw std::runtime_error("there should be a conflict if one clause is false!");
    }
    std::pair<Clause *, Variable *> uc = CDCL::FIND_UNIT_CLAUSE(cnf);
    clause = uc.first;
    var = uc.second;
    if (clause != nullptr) {
        DPLL::set_variable_value(cnf, var, var->get_sign());
        if (CERR_LEVEL >= 3) {
            std::cerr << "unit clause rule on " << var->to_string() << std::endl;
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
        DecisionLiteral *l = create_decision_literal(var->get_name(), var->get_sign());
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

/**
 * Applies conflict resolution to the dependency graph -> back jumping and clause learning
 */
CNF *CDCL::conflict_resolution() {
    if (CERR_LEVEL >= 2) {
        std::cerr << "conflict resolution" << std::endl;
    }
    auto *p = (StandardLiteral *) dependency_graph->find(new Variable(true, false, dependency_graph->conflict));
    auto *n = (StandardLiteral *) dependency_graph->find(new Variable(false, false, dependency_graph->conflict));

    Clause *newClause = learn_clause(p, n);
    cnf->add_clause(newClause);

    // identify the relevant decision literals
    LiteralSet relevant;
    relevant.insert((Literal *) p->implied_by);
    relevant.insert((Literal *) n->implied_by);

    jump_back(relevant);
    dependency_graph->has_conflict = false;
    return cnf;
}

/**
 * Cuts the dependency graph and construct a new learned clause
 * @param positive_literal the positive literal of the conflict
 * @param negative_literal the negative literal of the conflict
 * @return a new clause learned from the conflict
 */
Clause *CDCL::learn_clause(StandardLiteral *positive_literal, StandardLiteral *negative_literal) {
    LiteralSet parents;
    parents.insert(positive_literal->parents.begin(), positive_literal->parents.end());
    parents.insert(negative_literal->parents.begin(), negative_literal->parents.end());
    VariableSet new_clause_variables;
    LiteralSet decisionParents;
    std::queue<Literal *> workList;
    for (auto parent : parents) {
        workList.push(parent);
    }
    while (!workList.empty()) {
        Literal *e = workList.front();
        workList.pop();
        if (e->type == DECISION) {
            // NOTE: this step is done to get rid of duplicates
            decisionParents.insert(e);
        } else {
            for (auto parent: e->parents) {
                workList.push(parent);
            }
        }
    }

    for (auto parent : decisionParents) {
        new_clause_variables.push_back(new Variable(!parent->sign, false, parent->name));
    }
    Clause *result = new Clause(new_clause_variables);
    if (CERR_LEVEL >= 3) {
        std::cerr << "learning clause: (" << result->to_string() << ")" << std::endl;
    }
    return result;
}

/**
 * Jumps back in the dependency graph
 * Here we pop one relevant literal and as many others as possible (non-relevant ones)
 */
void CDCL::jump_back(LiteralSet relevant_literals) {
    bool one_popped = false;
    while (parent_decision != nullptr) {
        if (relevant_literals.find(parent_decision) != relevant_literals.end()) {
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
    dependency_graph->free_freeable_nodes();
}

void CDCL::remove_all_consequences(DecisionLiteral *literal) {
    for (auto consequence : literal->implies) {
        DPLL::unset_variable_value(cnf, new Variable(false, false, consequence->name));
    }
    DPLL::unset_variable_value(cnf, new Variable(false, false, literal->name));
    dependency_graph->remove_node(literal);
}
