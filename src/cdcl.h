//
// Created by jan on 21.11.17.
//

#ifndef SAT_SOLVER_CDCL_H
#define SAT_SOLVER_CDCL_H

#include <unordered_map>
#include "dpll.h"
#include "cdcl_dependency_graph.h"

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
    Clause *learn_clause(StandardLiteral *positive_literal, StandardLiteral *negative_literal);
    void jump_back(LiteralSet relevant_literals);
    void remove_all_consequences(DecisionLiteral *literal);
public:
    explicit CDCL(CNF _cnf);
    bool SATISFIABLE();
    DpllResult *CDCLAlgorithm(CNF *cnf);
    CNF *get_cnf();
};


#endif //SAT_SOLVER_CDCL_H
