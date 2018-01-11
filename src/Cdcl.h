//
// Created by jan on 21.11.17.
//

#ifndef SAT_SOLVER_CDCL_H
#define SAT_SOLVER_CDCL_H

#include <unordered_map>
#include "Dpll.h"
#include "CdclDependencyGraph.h"
#include "Solver.h"

class DpllResult;

class CDCL: public Solver {
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
    explicit CDCL(CNF *_cnf);
    DpllResult *CDCLAlgorithm(CNF *cnf);
    bool SATISFIABLE() override;
    CNF *get_cnf() override;
};


#endif //SAT_SOLVER_CDCL_H
