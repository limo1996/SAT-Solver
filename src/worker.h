#ifndef SAT_SOLVER_WORKER_H
#define SAT_SOLVER_WORKER_H


#include "CNF.h"
#include <vector>

class Worker {
private:
    CNF *cnf;

    void dpll_callback(std::set<Variable *> *variables);

    bool run_dpll(std::set<Variable *> *variables);

    std::set<Variable *> *parse_variables(unsigned[], int size);

    std::vector<unsigned> encode_variables(std::set<Variable *> *variables);

public:
    explicit Worker(CNF _cnf);
};


#endif //SAT_SOLVER_WORKER_H
