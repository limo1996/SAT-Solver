#include <vector>
#include "worker.h"

Worker::Worker(CNF _cnf) {
    cnf = new CNF(_cnf);
}

void Worker::dpll_callback(std::set<Variable *> *variables) {

}

bool Worker::run_dpll(std::set<Variable *> *variables) {
    return false;
}

std::set<Variable *> *Worker::parse_variables(unsigned int *, int size) {
    return nullptr;
}

std::vector<unsigned> Worker::encode_variables(std::set<Variable *> *variables) {
    std::set<Variable*>::iterator iterator;
    unsigned long num_assigned = 0;
    for(iterator = variables->begin(); iterator != variables->end(); iterator++) {
        if ((*iterator)->get_assigned()) {
            num_assigned++;
        }
    }
    std::vector<unsigned> encoded(num_assigned);
    int i = 0;
    for(iterator = variables->begin(); iterator != variables->end(); iterator++) {
        if ((*iterator)->get_assigned()) {
            unsigned encoded_var = (unsigned) std::stoi((*iterator)->get_name()) << 1;
            if ((*iterator)->get_value()) {
                encoded_var++;
            }
            encoded[i] = encoded_var;
            i++;
        }
    }
    return encoded;
}
