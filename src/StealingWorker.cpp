#include <climits>
#include "StealingWorker.h"

extern int CERR_LEVEL;

void StealingWorker::debug_output(std::string line, bool newLine, int level){
    if (CERR_LEVEL >= level) {
        std::cerr << "StealingWorker " << this->my_rank << ": " << line;
        if(newLine) std::cerr << std::endl;
    }
}

StealingWorker::StealingWorker(CNF _cnf, MPI_Datatype _meta_data_type, int _my_rank, int _workers_size) {
    cnf = new CNF(_cnf);
    meta_data_type = _meta_data_type;
    my_rank = _my_rank;
    workers_size = _workers_size;
    stop = false;
    next_to_send = 1;
}

/**
 * Worker starts to solve cnf and sends one subproblem to each other worker.
 */
void StealingWorker::start(){
    run_dpll();
}

/**
 * Runs dpll on the cnf store in this::cnf. If finds the model that prints it and stops the other workers.
 * If finds unsat than takes another model from local queue. If it is empty than tries to steal from other workers.
 */
void StealingWorker::run_dpll() {
    Config *config = new Config(INT_MAX, this);
    DPLL *dpll = new DPLL(*cnf, config);
    bool sat = dpll->DPLL_SATISFIABLE();
    if (sat) {
        print_sat_stop_workers(dpll->get_cnf());
    } else {
        check_and_process_message_from_worker(false);
        get_model();
    }
}

/**
 * check if we receive a stop message from another worker before the messages are delivered to all workers
 * if that's the case, cancel the message sends and return true
 * -> the caller is then responsible to * stop.
 */
bool StealingWorker::stop_received_before_message_completion(MPI_Request *mpi_requests, int size) {
    debug_output("stop_received_before_message_completion", true);
    
    int flag = 0;
    int all_done = 0;
    if (!this->stop) {
        while (flag == 0 && all_done == 0) {
            MPI_Testall(size, mpi_requests, &all_done, MPI_STATUS_IGNORE);
            MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, MPI_STATUS_IGNORE);
        }
    }
    if (flag != 0 && !this->stop) {
        struct meta meta;
        MPI_Status status;
        MPI_Recv(&meta, 1, meta_data_type, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        if (meta.message_type == 1) {
            // steal message
            debug_output("received steal message from worker " + std::to_string(status.MPI_SOURCE) + ". Size of the local queue: " + std::to_string(this->stack.size()), true);

            MPI_Status* statuses;
            if(this->stack.empty()) {
                MPI_Request mpi_requests[1];
                mpi_requests[0] = send_meta(status.MPI_SOURCE, 0, INT_MAX);
                MPI_Waitall(1, mpi_requests, statuses);
            } else {
                MPI_Request mpi_requests[2];
                mpi_requests[0] = send_meta(status.MPI_SOURCE, 0, count_assigned(this->stack.back()));
                mpi_requests[1] = send_model(status.MPI_SOURCE, encode_variables(this->stack.back()));
                MPI_Waitall(2, mpi_requests, statuses);
                this->stack.pop_back();
            }
        } else if (meta.message_type == 2){
            debug_output("stop received while waiting for message delivery, cancelling requests if necessary", true);
            this->stop = true;
            if (!all_done) {
                for (int i = 0; i < size; i++) {
                    MPI_Cancel(mpi_requests + i);
                }
            }
        } else
            debug_output("weird message: " + std::to_string((int)meta.message_type) + " flag: " + std::to_string(flag) + " done all: " + std::to_string(all_done), true);
    }
    return flag != 0;
}

//bool StealingWorker::respond( )

/**
 * Enters a blocking receive on messages from master
 * Handles received messages:
 * - either done message -> stop
 * - or more work message -> setup variables start dpll
 */
bool StealingWorker::check_and_process_message_from_worker(bool wait) {
    std::string msg = "checks";
    if(wait)
        msg += " and waits";
    msg += " for message from other workers.";
    debug_output(msg, true);

    if(!wait){
        int flag = 0;
        MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, MPI_STATUS_IGNORE);
        if (flag == 0){
            debug_output("no message to receive.", true);
            return true;
        }
    }
    
    struct meta meta;
    MPI_Status status;
    MPI_Recv(&meta, 1, meta_data_type, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    if (meta.message_type == 0) {
        debug_output("received meta data (message type: 0, count: " + std::to_string(meta.count) + ")", true);
        if(meta.count == INT_MAX)
            return false;
        if (meta.count > 0) {
            unsigned encoded_model[meta.count];
            MPI_Recv(encoded_model, meta.count, MPI_UNSIGNED, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            debug_output("encoded model: ", false, 2);
            for (int i = 0; i < meta.count; i++) {
                debug_output(std::to_string(encoded_model[i]) + " ", false, 2);
            }
            debug_output("", true, 2);
            parse_and_update_variables(encoded_model, meta.count);
            cerr_model("received model of size " + std::to_string(meta.count), cnf->get_model());
        } else {
            unsigned encoded[0];
            parse_and_update_variables(encoded, 0);
            debug_output("received model of size 0 and will start solving", true);
        }
        run_dpll();
    } else if (meta.message_type == 1){
        // steal message
        debug_output("received steal message from worker " + std::to_string(status.MPI_SOURCE) + ". Size of the local queue: " + std::to_string(this->stack.size()), true);
        
        if(this->stack.empty()) {
            MPI_Request mpi_requests[1];
            mpi_requests[0] = send_meta(status.MPI_SOURCE, 0, INT_MAX);
            stop_received_before_message_completion(mpi_requests, 1);
        } else {
            MPI_Request mpi_requests[2];
            mpi_requests[0] = send_meta(status.MPI_SOURCE, 0, count_assigned(this->stack.back()));
            mpi_requests[1] = send_model(status.MPI_SOURCE, encode_variables(this->stack.back()));
            stop_received_before_message_completion(mpi_requests, 2);
            
            this->stack.pop_back();
        }
    } else if (meta.message_type == 2){
        // interrupt message. End silently...
        debug_output("received broadcast message to stop.", true);
        debug_output("gracefully stopping...", true);
        this->stop = true;
        return false;
    } else
        debug_output("received weird(" + std::to_string((int)meta.message_type) + ") message from worker" + std::to_string(status.MPI_SOURCE), true);
        
    return true;
}

/**
 * callback that is used on a dpll branch
 * @param variables contains the variable assignments that some other worker should try
 */
void StealingWorker::dpll_callback(std::unordered_set<Variable *> *variables) {
    if(this->my_rank == 0 && this->next_to_send < this->workers_size){
        debug_output("Sending starting model to StealingWorker " + std::to_string(this->next_to_send), true);
        MPI_Request mpi_requests[2];
        mpi_requests[0] = send_meta(this->next_to_send, 0, count_assigned(variables));
        mpi_requests[1] = send_model(this->next_to_send, encode_variables(variables));
        stop_received_before_message_completion(mpi_requests, 2);
        this->next_to_send++;
    } else {
        cerr_model("dpll store to local stack", variables);
        this->stack.push_front(variables);
        check_and_process_message_from_worker(false);
    }
}

/**
 * Sends unsat result to master and triggers waiting for further instructions
 */
void StealingWorker::get_model() {
    if (!this->stop) {
        if(!this->stack.empty()){
            debug_output("finished branch with unsat. Takes another model from local queue. Size of the queue: " + std::to_string(this->stack.size()), true);
            std::vector<unsigned> vars = encode_variables(this->stack.front());
            parse_and_update_variables(&vars[0], vars.size());
            this->stack.pop_front();
            run_dpll();
        } else {
            debug_output("finished branch with unsat. Tries to steal from worker " + std::to_string((my_rank + 1) % workers_size), true);
            MPI_Request request = send_meta((my_rank + 1) % workers_size, 1, 0);
            bool received = stop_received_before_message_completion(&request, 1);
            if (received) {
                run_dpll();
            } else {
                bool res = check_and_process_message_from_worker(true);
                if(res)
                    run_dpll();
                else if (CERR_LEVEL >= 1) {
                        std::cerr << "StealingWorker " << my_rank << ": gracefully stopping..." << std::endl;
                }
            }
        }
    }
}

/**
 * Sends meta data to master (non-blocking send)
 * @param to_rank rank of the targeted worker
 * @param i message type
 * @param assigned the number of assigned variables
 * @return the MPI Request on that we can wait for completion of the non-blocking send
 */
MPI_Request StealingWorker::send_meta(int to_rank, char i, unsigned assigned) {
    debug_output("sending meta (i: " + std::to_string((int)i) + ", assigned: " + std::to_string(assigned) + ") to worker " + std::to_string(to_rank), true);
    struct meta meta;
    meta.message_type = i;
    meta.count = assigned;
    
    MPI_Request request;
    MPI_Isend(&meta, 1, this->meta_data_type, to_rank, 0, MPI_COMM_WORLD, &request);
    return request;
}

/**
 * Sends model to master (non-blocking send)
 * @param to_rank rank of the targeted worker
 * @param assigned the vector of encoded variables
 * @return the MPI Request on that we can wait for completion of the non-blocking send
 */
MPI_Request StealingWorker::send_model(int to_rank, std::vector<unsigned> assigned) {
    debug_output("sending model of size " + std::to_string(assigned.size()), true);
    MPI_Request request;
    MPI_Isend(&assigned.front(), (int) assigned.size(), MPI_UNSIGNED, to_rank, 0, MPI_COMM_WORLD, &request);
    return request;
}

/**
 * Sends sat meta data and satisfiable model to master, triggers waiting for further instructions
 * @param cnf object that contains the assigned variables
 */
void StealingWorker::print_sat_stop_workers(CNF *cnf) {
    if (!this->stop) {
        cerr_model("Found SAT model. Sends message type 2 to all workers.", cnf->get_model());
        stop_workers();
        output_sat_model(cnf);
        debug_output("gracefully stopping...", true);
    }
}

void StealingWorker::stop_workers(){
    struct meta meta;
    meta.message_type = 2;
    meta.count = 0;
    
    int size = static_cast<int>(this->workers_size - 1);
    MPI_Request mpi_requests[size];
    int count = 0;
    for (int i = 0; i < this->workers_size; i++) {
        if (i != this->my_rank) {
            struct meta meta_copy = meta;
            MPI_Isend(&meta_copy, 1, this->meta_data_type, i, 0, MPI_COMM_WORLD, mpi_requests + count);
            count++;
        }
    }
    debug_output("broadcast initiated", true);
    MPI_Waitall(size, mpi_requests, MPI_STATUS_IGNORE);
    debug_output("broadcast done", true);
}

void StealingWorker::output_sat_model(CNF *cnf){
    std::unordered_set<Variable *> *vars = cnf->get_model();
    // go through the variables and assign true to all the unassigned ones
    for (auto v : *vars) {
        if(!v->get_assigned()) {
            v->set_assigned(true);
            v->set_value(true);
        }
    }
    
    unsigned num_assigned = count_assigned(vars);
    std::vector<unsigned> encoded_model = encode_variables(vars);
    
    std::cout  << "sat" << std::endl;
    for (int i = 0; i < num_assigned; i++) {
        std::string name = std::to_string(encoded_model[i] >> 1);
        bool encoded_val = encoded_model[i] % 2 == 1;
        std::cout <<name << " " << (encoded_val ? "t" : "f") << std::endl;
    }
}

/**
 * parses and decodes the received encoded variables, sets this::cnf with the parsed variables
 * @param encoded array that contains the encoded model
 * @param size the size of the array
 */
void StealingWorker::parse_and_update_variables(unsigned int encoded[], int size) {
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
std::vector<unsigned> StealingWorker::encode_variables(std::unordered_set<Variable *> *variables) {
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

unsigned StealingWorker::count_assigned(std::unordered_set<Variable *> *variables) {
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
void StealingWorker::cerr_model(std::string info, std::unordered_set<Variable *> *variables) {
    if(CERR_LEVEL < 1)
        return;
    
    std::unordered_set<Variable *>::iterator iterator;
    std::cerr << "StealingWorker " << my_rank << ": " << info << " model: (";
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
