//
// Created by jan on 23.11.17.
//

#ifndef SAT_SOLVER_CDCL_DEPENDENCY_GRAPH_H
#define SAT_SOLVER_CDCL_DEPENDENCY_GRAPH_H

#include <unordered_set>
#include <unordered_map>
#include "Variable.h"

/**
 * CDCL dependency graph implementation
 */

class Literal;
typedef std::unordered_set<Literal *> LiteralSet;

enum LType {
    UNKNOWN, DECISION, STANDARD
};


class Literal {
public:
    LType type;
    unsigned name;
    bool sign;
    LiteralSet parents{};
    LiteralSet children{};
    explicit Literal(unsigned _name, bool _sign);
    void add_child(Literal *l);
    std::string to_string();
    unsigned hash();
    unsigned negative_hash();
    static unsigned hash(Variable *v);
    static unsigned negation_hash(Variable *v);
};

class DecisionLiteral : public Literal {
public:
    LiteralSet implies{};
    explicit DecisionLiteral(unsigned _name, bool _sign);
    void add_implies(Literal *l);
};

class StandardLiteral : public Literal {
public:
    DecisionLiteral *implied_by{};
    explicit StandardLiteral(unsigned _name, bool _sign, DecisionLiteral *_implied_by);
};

class Graph {
private:
    std::unordered_map<unsigned, Literal *> lookup_map;
    Literal *find(unsigned key);
    std::unordered_set<Literal *> to_free;

public:
    bool has_conflict;
    unsigned conflict;
    Graph();
    Literal *find(Variable *var);
    Literal *find_negation(Variable *var);
    void remove_node(Literal *l);
    void add_node(Literal *l);
    void free_freeable_nodes();
};

#endif //SAT_SOLVER_CDCL_DEPENDENCY_GRAPH_H
