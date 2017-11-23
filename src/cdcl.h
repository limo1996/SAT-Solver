//
// Created by jan on 21.11.17.
//

#ifndef SAT_SOLVER_CDCL_H
#define SAT_SOLVER_CDCL_H

#include <unordered_map>
#include "dpll.h"

class Literal;
typedef std::unordered_set<Literal *> LiteralSet;

enum LType {
    DECISION, STANDARD
};


class Literal {
public:
    LType type;
    unsigned name;
    bool sign;
    LiteralSet parents{};
    LiteralSet children{};
    explicit Literal(unsigned _name, bool _sign) {
        name = _name;
        sign = _sign;
    }
    void add_child(Literal *l) {
        children.insert(l);
    }
    std::string to_string() {
        return sign ? std::to_string(name) : "!" + std::to_string(name);
    }

};

class DecisionLiteral : public Literal {
public:
    LiteralSet implies{};
    explicit DecisionLiteral(unsigned _name, bool _sign): Literal(_name, _sign) {
        type = DECISION;
    }
    void add_implies(Literal *l) {
        implies.insert(l);
    }
};

class StandardLiteral : public Literal {
public:
    DecisionLiteral *implied_by{};
    explicit StandardLiteral(unsigned _name, bool _sign, DecisionLiteral *_implied_by):
            Literal(_name, _sign) {
        implied_by = _implied_by;
        type = STANDARD;
    }
};

class Graph {
private:
    std::unordered_map<std::string, Literal *> lookup_map;
    Literal *find(const std::string &key) {
        std::unordered_map<std::string, Literal *>::const_iterator got = lookup_map.find(key);
        if (got == lookup_map.end()) {
            return nullptr;
        } else {
            return got->second;
        }
    }

public:
    bool has_conflict;
    unsigned conflict;
    Graph() {
        has_conflict = false;
    }
    Literal *find(Variable *var) {
        std::string key = var->get_sign() ? std::to_string(var->get_name()) : "!" + std::to_string(var->get_name());
        return find(key);
    }
    Literal *find_negation(Variable *var) {
        std::string key = var->get_sign() ? "!" + std::to_string(var->get_name()) : std::to_string(var->get_name());
        return find(key);
    }
    std::set<DecisionLiteral*> get_all_decision_literals() {
        std::set<DecisionLiteral*> res;
        for (auto tuple : lookup_map) {
            Literal *l = tuple.second;
            if (l->type == DECISION) {
                res.insert((DecisionLiteral*) l);
            }
        }
        return res;
    }
    void remove_node(Literal *l) {
        if (l->type == STANDARD) {
            throw std::runtime_error("Not implemented, this shouldn't happen!");
        } else if (l->type == DECISION) {
            auto *dl = (DecisionLiteral*) l;
            for (auto i : dl->implies) {
                for (auto c : i->children) {
                    c->parents.erase(i);
                }
                lookup_map.erase(i->to_string());
                //delete i;
            }
            lookup_map.erase(dl->to_string());
            //delete dl;
        }

    }

    void add_node(Literal *l) {
        std::string name = std::to_string(l->name);
        Literal *p = find(name);
        Literal *n = find("!" + name);
        if (l->sign) {
            if (n != nullptr) {
                has_conflict = true;
                conflict = l->name;
            }
            if (p != nullptr) {
                throw std::runtime_error("attempting to add the same literal twice: " + l->to_string());
            }
        } else {
            if (p != nullptr) {
                has_conflict = true;
                conflict = l->name;
            }
            if (n != nullptr) {
                throw std::runtime_error("attempting to add the same literal twice: " + l->to_string());
            }
        }
        std::string key = l->to_string();
        lookup_map.insert(std::make_pair(key, l));
    }
};
class CDCL {
private:
    CNF *cnf;
    Graph *dependency_graph;
    DecisionLiteral *parent_decision;
    StandardLiteral *create_standard_literal(unsigned int name, bool sign);
    DecisionLiteral *create_decision_literal(unsigned int name, bool sign);
    std::pair<Clause *, Variable *> FIND_UNIT_CLAUSE(CNF *cnf);
    void add_dependency_edges_to_graph(unsigned int name, bool sign, Clause *clause);
    CNF* conflict_resolution();
    LiteralSet relevantLiterals();
public:
    explicit CDCL(CNF _cnf);
    bool SATISFIABLE();

    DpllResult *CDCLAlgorithm(CNF *cnf);

    DpllResult *branch_on_variable(Variable *var, CNF *cnf);
    CNF *get_cnf();

    void remove_all_consequences(DecisionLiteral *literal);
};


#endif //SAT_SOLVER_CDCL_H
