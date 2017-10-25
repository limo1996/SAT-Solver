#ifndef SAT_SOLVER_WORKER_H
#define SAT_SOLVER_WORKER_H


#include "CNF.h"
#include "dpll.h"
#include <stdexcept>
#include <vector>

class Worker {
private:
    CNF *cnf;

    static void dpll_callback(std::set<Variable *> *variables);

    static void send_meta(char i, unsigned assigned);

    static void send_model(std::vector<unsigned int> assigned);

    void send_sat(CNF *cnf);

    void send_unsat();

    void run_dpll();

    void parse_and_update_variables(unsigned[], int size);

    static std::vector<unsigned> encode_variables(std::set<Variable *> *variables);

public:
    explicit Worker(CNF _cnf);

};


#endif //SAT_SOLVER_WORKER_H
