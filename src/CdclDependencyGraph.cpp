//
// Created by jan on 23.11.17.
//

#include "CdclDependencyGraph.h"

/*
 * Literal
 */
Literal::Literal(unsigned _name, bool _sign) {
    type = UNKNOWN;
    name = _name;
    sign = _sign;
}

void Literal::add_child(Literal *l) {
    children.insert(l);
}

std::string Literal::to_string() {
    return sign ? std::to_string(name) : "!" + std::to_string(name);
}

unsigned Literal::hash() {
    unsigned i = name << 1;
    return sign ? i : i + 1;
}

unsigned Literal::negative_hash() {
    unsigned i = name << 1;
    return sign ? i + 1 : i;
}

unsigned Literal::hash(Variable *v) {
    unsigned i = v->get_name() << 1;
    return v->get_sign() ? i : i + 1;
}

unsigned Literal::negation_hash(Variable *v) {
    unsigned i = v->get_name() << 1;
    return v->get_sign() ? i + 1 : i;
}

/*
 * DecisionLiteral
 */
DecisionLiteral::DecisionLiteral(unsigned _name, bool _sign) : Literal(_name, _sign) {
    type = DECISION;
}

void DecisionLiteral::add_implies(Literal *l) {
    implies.insert(l);
}

/*
 * StandardLiteral
 */
StandardLiteral::StandardLiteral(unsigned _name, bool _sign, DecisionLiteral *_implied_by) :
        Literal(_name, _sign) {
    implied_by = _implied_by;
    type = STANDARD;
}

/*
 * Graph
 */
Graph::Graph() {
    has_conflict = false;
    conflict = 0;
}

/**
 * Searches the dependency graph for an element, if it is not found, nullptr is returned
 */
Literal *Graph::find(unsigned key) {
    std::unordered_map<unsigned, Literal *>::const_iterator got = lookup_map.find(key);
    if (got == lookup_map.end()) {
        return nullptr;
    } else {
        return got->second;
    }
}

/**
 * Searches the dependency graph for a variable, if it is not found, nullptr is returned
 */
Literal *Graph::find(Variable *var) {
    unsigned key = Literal::hash(var);
    return find(key);
}

/**
 * Searches the dependency graph for the negation of a variable, if it is not found, nullptr is returned
 */
Literal *Graph::find_negation(Variable *var) {
    unsigned key = Literal::negation_hash(var);
    return find(key);
}

/**
 * Removes a literal from the dependency graph
 */
void Graph::remove_node(Literal *l) {
    if (l->type == STANDARD) {
        throw std::runtime_error("Not implemented, this shouldn't happen!");
    } else if (l->type == DECISION) {
        auto *dl = (DecisionLiteral *) l;
        for (auto i : dl->implies) {
            for (auto c : i->children) {
                c->parents.erase(i);
            }
            lookup_map.erase(i->hash());
            to_free.insert(i);
        }
        lookup_map.erase(dl->hash());
        to_free.insert(l);
    }
}

void Graph::free_freeable_nodes() {
    for (auto n : to_free) {
        delete n;
    }
    to_free.clear();
}

/**
 * Adds a literal to the dependency graph
 * Setting the parents and children of the element is the responsibility of the caller!
 */
void Graph::add_node(Literal *l) {
    Literal *same = find(l->hash());
    Literal *negation = find(l->negative_hash());
    if (negation != nullptr) {
        has_conflict = true;
        conflict = l->name;
    }
    if (same != nullptr) {
        throw std::runtime_error("attempting to add the same literal twice: " + l->to_string());
    }
    lookup_map.insert(std::make_pair(l->hash(), l));
}
