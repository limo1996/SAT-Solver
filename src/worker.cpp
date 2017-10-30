#include "worker.h"

bool CERR_DEBUG = true;

Worker::Worker(CNF _cnf, MPI_Datatype _meta_data_type, int _worker_rank) {
    cnf = new CNF(_cnf);
    meta_data_type = _meta_data_type;
    worker_rank = _worker_rank;
    wait_for_instructions_from_master();
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

/**
 * runs dpll on the cnf store in this::cnf
 */
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

/**
 * callback that is used on a dpll branch
 * @param variables contains the variable assignments that some other worker should try
 */
void Worker::dpll_callback(std::set<Variable *> *variables) {
    if (!received_message_from_master()) {
        std::cout << "dpll callback\n";
        unsigned num_assigned = count_assigned(variables);
        MPI_Request mpi_requests[2];
        mpi_requests[0] = send_meta(10, num_assigned);
        mpi_requests[1] = send_model(encode_variables(variables));
        // TODO: not sure if we introduce a glitch here -> could cancel with MPI_Cancel(request)
        // TODO: technically we could already start with solving while the messages are delivered...
        // for the moment let's leave it like this, but it's a possible performance improvement
        MPI_Waitall(2, mpi_requests, MPI_STATUS_IGNORE);
    }
}

/**
 * Sends meta data to master (non-blocking send)
 * @param i message type
 * @param assigned the number of assigned variables
 * @return the MPI Request on that we can wait for completion of the non-blocking send
 */
MPI_Request Worker::send_meta(char i, unsigned assigned) {
    std::cout << "sending meta... i: " << (int)i << " assigned: " << assigned << std::endl;
    struct meta meta;
    meta.message_type = i;
    meta.count = assigned;

    MPI_Request request;
    MPI_Isend(&meta, 1, meta_data_type, 0, 0, MPI_COMM_WORLD, &request);
    return request;
}

/**
 * Sends model to master (non-blocking send)
 * @param assigned the vector of encoded variables
 * @return the MPI Request on that we can wait for completion of the non-blocking send
 */
MPI_Request Worker::send_model(std::vector<unsigned> assigned) {
    MPI_Request request;
    MPI_Isend(&assigned.front(), (int) assigned.size(), MPI_UNSIGNED, 0, 0, MPI_COMM_WORLD, &request);
    return request;
}

/**
 * Sends sat meta data and satisfiable model to master, triggers waiting for further instructions
 * @param cnf object that contains the assigned variables
 */
void Worker::send_sat(CNF *cnf) {
    if (!received_message_from_master()) {
        if (CERR_DEBUG) {
            std::cerr << "Worker " << worker_rank << ": sends sat model to master" << std::endl;
        }
        std::set<Variable *> *vars = cnf->get_var();
        unsigned num_assigned = count_assigned(vars);
        MPI_Request requests[2];
        requests[0] = send_meta(12, num_assigned);
        requests[1] = send_model(encode_variables(vars));
        // TODO: not sure if we introduce a glitch here -> could cancel with MPI_Cancel(request)
        MPI_Waitall(2, requests, MPI_STATUS_IGNORE);
        wait_for_instructions_from_master();
    }
}

/**
 * Sends unsat result to master and triggers waiting for further instructions
 */
void Worker::send_unsat() {
    if (!received_message_from_master()) {
        if (CERR_DEBUG) {
            std::cerr << "Worker " << worker_rank << ": sends unsat to master" << std::endl;
        }
        MPI_Request request = send_meta(11, 0);
        // TODO: not sure if we introduce a glitch here -> could cancel with MPI_Cancel(request)
        MPI_Wait(&request, MPI_STATUS_IGNORE);
        wait_for_instructions_from_master();
    }
}

/**
 * Probes for messages from master
 * If we received a message while not anticipated, it's either an abort message or the master
 * did send us something weird that we cannot handle...
 * @return true if we received a message from the master
 */
bool Worker::received_message_from_master() {
    int flag;
    MPI_Iprobe(0, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, MPI_STATUS_IGNORE);
    bool msg_received = flag != 0;
    if (msg_received) {
        struct meta meta;
        MPI_Recv(&meta, 1, meta_data_type, 0, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        if (meta.message_type == 1) {
            if (CERR_DEBUG) {
                std::cerr << "Worker " << worker_rank
                          << ": received done message from master and will stop" << std::endl;
            }
        } else {
            std::cerr << "Worker " << worker_rank << ": received unexpected meta message from master: ("
                      << meta.message_type << ", " << meta.count << ")" << std::endl;
        }
    }
    return msg_received;
}

/**
 * Enters a blocking receive on messages from master
 * Handles received messages:
 * - either done message -> stop
 * - or more work message -> setup variables start dpll
 */
void Worker::wait_for_instructions_from_master() {
    if (CERR_DEBUG) {
        std::cerr << "Worker " << worker_rank << ": is waiting for instructions from master" << std::endl;
    }
    struct meta meta;
    MPI_Recv(&meta, 1, meta_data_type, 0, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    if (meta.message_type == 0) {
        if (CERR_DEBUG) {
            std::cerr << "Worker " << worker_rank << ": received model of size " << meta.count
                      << " and will start solving" << std::endl;
        }
        unsigned encoded_model[meta.count];
        if (meta.count > 0) {
            MPI_Recv(encoded_model, meta.count, MPI_UNSIGNED, 0, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            parse_and_update_variables(encoded_model, meta.count);
        } else {
            unsigned encoded[0];
            parse_and_update_variables(encoded, 0);
        }
        run_dpll();
    } else {
        if (CERR_DEBUG) {
            std::cerr << "Worker " << worker_rank << ": received done message from master" << std::endl;
        }
    }
}

/**
 * parses and decodes the received encoded variables, sets this::cnf with the parsed variables
 * @param encoded array that contains the encoded model
 * @param size the size of the array
 */
void Worker::parse_and_update_variables(unsigned int encoded[], int size) {
    std::set<Variable *> vars;
    // first "unset" all the variables in this::cnf
    std::set<Variable *>::iterator iter;
    for (iter = cnf->get_var()->begin(); iter != cnf->get_var()->end(); iter++) {
        if ((*iter)->get_assigned()) {
            (*iter)->set_assigned(false);
        }
    }

    // then "set" all variables based on the encoded model
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

/**
 * encodes the given variables as an unsigned array
 * @param variables the set of variables to encode (ony assigned ones are considered)
 * @return encoded vector
 */
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
