#include "Worker.h"


Worker::Worker() {
    stop = false;
}

bool Worker::get_stop() {
    return stop;
}

void Worker::send_measurements() {
    stop_runtime();
    unsigned data[4];
    data[0] = get_runtime();
    data[1] = get_waiting_time();
    data[2] = get_send_messages();
    data[3] = get_send_meta();
    MPI_Send(data, 4, MPI_UNSIGNED, 0, 2, MPI_COMM_WORLD);
}

/**
 * encodes the given variables as an unsigned array
 * @param variables the set of variables to encode (only assigned ones are considered)
 * @return encoded vector
 */
std::vector<unsigned> Worker::encode_variables(VariableSet *variables) {
    unsigned num_assigned = count_assigned(variables);
    std::vector<unsigned> encoded;
    encoded.reserve(num_assigned);
    std::vector<Variable *>::iterator iterator;
    for (iterator = variables->begin(); iterator != variables->end(); iterator++) {
        if ((*iterator)->get_assigned()) {
            unsigned encoded_var = (unsigned) (*iterator)->get_name() << 1;
            if ((*iterator)->get_value()) {
                encoded_var++;
            }
            encoded.push_back(encoded_var);
        }
    }
    return encoded;
}

/**
 * counts number of assigned variables passed as parameter.
 * @return number of assigned variables
 */
unsigned Worker::count_assigned(VariableSet *variables) {
    unsigned num_assigned = 0;
    for (auto var : *variables) {
        if (var->get_assigned()) {
            num_assigned++;
        }
    }
    return num_assigned;
}

/**
 * parses and decodes the received encoded variables, sets this::cnf with the parsed variables
 * @param encoded array that contains the encoded model
 * @param size the size of the array
 */
void Worker::parse_and_update_variables(unsigned int encoded[], int size) {
    // first "unset" all the variables in this::cnf
    for (auto clause : *cnf->get_clauses()) {
        for (auto v : *clause->get_vars()) {
            v->set_assigned(false);
        }
    }
    // then set them according to encoded[]
    for (auto clause : *cnf->get_clauses()) {
        for (auto v : *clause->get_vars()) {
            for (int i=0; i< size; i++) {
                int name = encoded[i] >> 1;
                bool encoded_val = encoded[i] % 2 == 1;
                if (v->get_name() == name) {
                    v->set_assigned(true);
                    v->set_value(encoded_val);
                }
            }
        }
    }
}
