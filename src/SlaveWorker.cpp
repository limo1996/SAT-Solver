#include <climits>
#include "SlaveWorker.h"

extern int CERR_LEVEL;

SlaveWorker::SlaveWorker(CNF _cnf, MPI_Datatype _meta_data_type, int _worker_rank) {
    cnf = new CNF(_cnf);
    meta_data_type = _meta_data_type;
    worker_rank = _worker_rank;
    stop = false;
}

unsigned count_assigned(std::unordered_set<Variable *> *variables) {
    unsigned num_assigned = 0;
    std::unordered_set<Variable *>::iterator iterator;
    for (iterator = variables->begin(); iterator != variables->end(); iterator++) {
        if ((*iterator)->get_assigned()) {
            num_assigned++;
        }
    }
    return num_assigned;
}

/**
 * outputs given variable assignments to stderr
 */
void SlaveWorker::cerr_model(std::string info, std::unordered_set<Variable *> *variables) {
    std::unordered_set<Variable *>::iterator iterator;
    std::cerr << "SlaveWorker " << worker_rank << ": " << info << " model: (";
    for (iterator = variables->begin(); iterator != variables->end(); iterator++) {
        if ((*iterator)->get_assigned()) {
            std::cerr << (*iterator)->get_name() << ":";
            if ((*iterator)->get_value()) {
                std::cerr << "t";
            } else {
                std::cerr << "f";
            }
            std::cerr << ", ";
        }
    }
    std::cerr << "\b\b)" << std::endl;
}

/**
 * runs dpll on the cnf store in this::cnf
 */
void SlaveWorker::run_dpll() {
    Config *config = new Config(INT_MAX, this);
    DPLL *dpll = new DPLL(*cnf, config);
    bool sat = dpll->DPLL_SATISFIABLE();
    if (sat) {
        send_sat(dpll->get_cnf());
    } else {
        send_unsat();
    }
}

/**
 * check if we receive a stop message from master before the messages are delivered to master
 * if that's the case, cancel the message sends and return true
 * -> the caller is then responsible to * stop.
 */
bool SlaveWorker::stop_received_before_message_completion(MPI_Request *mpi_requests, int size) {
    int flag = 0;
    int all_done = 0;
    if (!this->stop) {
        while (flag == 0 && all_done == 0) {
            MPI_Testall(size, mpi_requests, &all_done, MPI_STATUS_IGNORE);
            MPI_Iprobe(0, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, MPI_STATUS_IGNORE);
        }
    }
    if (flag != MPI_SUCCESS && !this->stop) {
        struct meta meta;
        MPI_Recv(&meta, 1, meta_data_type, 0, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        if (meta.message_type != 1) {
            if(CERR_LEVEL >= 1){
                std::cerr << "SlaveWorker " << worker_rank << ": weird message: " << (int)meta.message_type << " flag: " << flag << " done all: " << all_done << std::endl;
            }
            throw new std::runtime_error("SlaveWorker " + std::to_string(worker_rank)
                                         + " received weird message from master");
        }
        if (CERR_LEVEL >= 1) {
            std::cerr << "SlaveWorker " << worker_rank
            << ": stop received while waiting for message delivery, cancelling requests if necessary"
            << std::endl;
        }
        this->stop = true;
        if (!all_done) {
            for (int i = 0; i < size; i++) {
                MPI_Cancel(mpi_requests + i);
            }
        }
    }
    return flag != 0;
}

/**
 * callback that is used on a dpll branch
 * @param variables contains the variable assignments that some other worker should try
 */
void SlaveWorker::dpll_callback(std::unordered_set<Variable *> *variables) {
    unsigned num_assigned = count_assigned(variables);
    if (CERR_LEVEL >= 1) {
        cerr_model("dpll branch", variables);
    }
    MPI_Request mpi_requests[2];
    mpi_requests[0] = send_meta(10, num_assigned);
    mpi_requests[1] = send_model(encode_variables(variables));
    bool stop = stop_received_before_message_completion(mpi_requests, 2);
    if (stop) {
        this->stop = true;
    }
}

/**
 * Sends meta data to master (non-blocking send)
 * @param i message type
 * @param assigned the number of assigned variables
 * @return the MPI Request on that we can wait for completion of the non-blocking send
 */
MPI_Request SlaveWorker::send_meta(char i, unsigned assigned) {
    if (CERR_LEVEL >= 1) {
        std::cerr << "SlaveWorker " << worker_rank << ": sending meta (i: " << (int) i << ", assigned: "
        << assigned << ")" << std::endl;
    }
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
MPI_Request SlaveWorker::send_model(std::vector<unsigned> assigned) {
    MPI_Request request;
    MPI_Isend(&assigned.front(), (int) assigned.size(), MPI_UNSIGNED, 0, 0, MPI_COMM_WORLD, &request);
    return request;
}

/**
 * Sends sat meta data and satisfiable model to master, triggers waiting for further instructions
 * @param cnf object that contains the assigned variables
 */
void SlaveWorker::send_sat(CNF *cnf) {
    if (!this->stop) {
        std::unordered_set<Variable *> *vars = cnf->get_model();
        // go through the variables and assign true to all the unassigned ones
        for (auto v : *vars) {
            if(!v->get_assigned()) {
                v->set_assigned(true);
                v->set_value(true);
            }
        }
        unsigned num_assigned = count_assigned(vars);
        if (CERR_LEVEL >= 1) {
            cerr_model("sends sat model to master", vars);
        }
        MPI_Request requests[2];
        requests[0] = send_meta(12, num_assigned);
        requests[1] = send_model(encode_variables(vars));
        
        bool stop_received = stop_received_before_message_completion(requests, 2);
        if (!stop_received) {
            wait_for_instructions_from_master();
        } else {
            if (CERR_LEVEL >= 1) {
                std::cerr << "SlaveWorker " << worker_rank << ": gracefully stopping..." << std::endl;
            }
        }
    }
}

/**
 * Sends unsat result to master and triggers waiting for further instructions
 */
void SlaveWorker::send_unsat() {
    if (!this->stop) {
        if (CERR_LEVEL >= 1) {
            std::cerr << "SlaveWorker " << worker_rank << ": sends unsat to master" << std::endl;
        }
        MPI_Request request = send_meta(11, 0);
        bool stop_received = stop_received_before_message_completion(&request, 1);
        if (!stop_received) {
            wait_for_instructions_from_master();
        } else {
            if (CERR_LEVEL >= 1) {
                std::cerr << "SlaveWorker " << worker_rank << ": gracefully stopping..." << std::endl;
            }
        }
    }
}

/**
 * Enters a blocking receive on messages from master
 * Handles received messages:
 * - either done message -> stop
 * - or more work message -> setup variables start dpll
 */
void SlaveWorker::wait_for_instructions_from_master() {
    if (CERR_LEVEL >= 1) {
        std::cerr << "SlaveWorker " << worker_rank << ": is waiting for instructions from master" << std::endl;
    }
    struct meta meta;
    MPI_Recv(&meta, 1, meta_data_type, 0, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    if (meta.message_type == 0) {
        unsigned encoded_model[meta.count];
        if (CERR_LEVEL >= 1) {
            std::cerr << "SlaveWorker " << worker_rank << ": received meta data (message type: 0, count: "
            << meta.count << ")" << std::endl;
        }
        if (meta.count > 0) {
            MPI_Recv(encoded_model, meta.count, MPI_UNSIGNED, 0, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if (CERR_LEVEL >= 2) {
                std::cerr << "SlaveWorker " << worker_rank << ": encoded_model: ";
                for (int i = 0; i < meta.count; i++) {
                    std::cerr << encoded_model[i] << " ";
                }
                std::cerr << std::endl;
            }
            parse_and_update_variables(encoded_model, meta.count);
            if(CERR_LEVEL >= 1){
                cerr_model("received model of size " + std::to_string(meta.count), cnf->get_model());
            }
        } else {
            unsigned encoded[0];
            parse_and_update_variables(encoded, 0);
            if(CERR_LEVEL >= 1){
                std::cerr << "SlaveWorker " << worker_rank
                << ": received model of size 0 and will start solving" << std::endl;
            }
        }
        run_dpll();
    } else {
        if (CERR_LEVEL >= 1) {
            std::cerr << "SlaveWorker " << worker_rank << ": received done message from master" << std::endl;
            std::cerr << "SlaveWorker " << worker_rank << ": gracefully stopping..." << std::endl;
        }
    }
}

/**
 * parses and decodes the received encoded variables, sets this::cnf with the parsed variables
 * @param encoded array that contains the encoded model
 * @param size the size of the array
 */
void SlaveWorker::parse_and_update_variables(unsigned int encoded[], int size) {
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

/**
 * encodes the given variables as an unsigned array
 * @param variables the set of variables to encode (ony assigned ones are considered)
 * @return encoded vector
 */
std::vector<unsigned> SlaveWorker::encode_variables(std::unordered_set<Variable *> *variables) {
    unsigned num_assigned = count_assigned(variables);
    std::vector<unsigned> encoded;
    encoded.reserve(num_assigned);
    std::unordered_set<Variable *>::iterator iterator;
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

