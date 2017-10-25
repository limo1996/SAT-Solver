#include "worker.h"

Worker::Worker(CNF _cnf, MPI_Datatype _meta_data_type) {
    cnf = new CNF(_cnf);
    meta_data_type = _meta_data_type;
}

unsigned count_assigned(std::set<Variable *> *variables) {
    unsigned num_assigned = 0;
    std::set<Variable *>::iterator iterator;
    for (iterator = variables->begin(); iterator != variables->end(); iterator++) {
        if ((*iterator)->get_assigned()) {
            num_assigned++;
        }
    }
    return num_assigned;
}

void Worker::run_dpll() {
    Config *config = new Config(true, this);
    DPLL *dpll = new DPLL(*cnf, config);
    bool sat = dpll->DPLL_SATISFIABLE();
    if (sat) {
        send_sat(dpll->get_cnf());
    } else {
        send_unsat();
    }
}

void Worker::dpll_callback(std::set<Variable *> *variables) {
    unsigned num_assigned = count_assigned(variables);
    send_meta(10, num_assigned);
    send_model(encode_variables(variables));
}

void Worker::send_meta(char i, unsigned assigned) {
    struct meta meta;
    meta.message_type = i;
    meta.count = assigned;

    MPI_Send(&meta, 1, meta_data_type, 0, 0, MPI_COMM_WORLD);
}

void Worker::send_model(std::vector<unsigned> assigned) {
    //TODO mpi stuff here
}

void Worker::send_sat(CNF *cnf) {
    std::set<Variable *> *vars = cnf->get_var();
    unsigned num_assigned = count_assigned(vars);
    send_meta(12, num_assigned);
    send_model(encode_variables(vars));
}

void Worker::send_unsat() {
    send_meta(11, 0);
}

void Worker::parse_and_update_variables(unsigned int encoded[], int size) {
    std::set<Variable *> vars;
    for (int i = 0; i < size; i++) {
        bool encoded_val = encoded[i] % 2 == 0;
        std::string name = std::to_string(encoded[i] >> 1);
        std::set<Variable *>::iterator iterator;

        for (iterator = cnf->get_var()->begin(); iterator !=
                                                 cnf->get_var()->end(); iterator++) {                                     /* for all variables */
            if ((*iterator)->get_name() == name) {
                (*iterator)->set_assigned(true);
                (*iterator)->set_value(encoded_val);
                break;
            }
        }
    }
}

std::vector<unsigned> Worker::encode_variables(std::set<Variable *> *variables) {
    unsigned num_assigned = count_assigned(variables);
    std::vector<unsigned> encoded;
    encoded.reserve(num_assigned);
    std::set<Variable *>::iterator iterator;
    for (iterator = variables->begin(); iterator != variables->end(); iterator++) {
        if ((*iterator)->get_assigned()) {
            unsigned encoded_var = (unsigned) std::stoi((*iterator)->get_name()) << 1;
            if ((*iterator)->get_value()) {
                encoded_var++;
            }
            encoded.push_back(encoded_var);
        }
    }
    return encoded;
}
