#include <iostream>
#include <chrono>
#include <fstream>

#include "CnfParser.h"
#include "StealingWorker.h"
#include "Measurement.h"
#include "ArgParsing.h"

using namespace std;
using namespace std::chrono;

int CERR_LEVEL = 0;
double STEALING_RATIO = 0.5;
int CHECK_PERIOD = 2;
int MIN_STACK_SIZE = 3;

void default_args(map<string, string> *arg_map) {
    arg_map->insert({"-local-cdcl", "-1"});
}

/**
 * Main entry point to parallel stealing version.
 */
int main(int argc, char *argv[]) {
    char *path = argv[argc - 1];

    map<std::string, std::string> arg_map;
    default_args(&arg_map);
    parse_args(&arg_map, argc, argv);

    CNFParser *parser;
    try {
        parser = new CNFParser(path);                                            // create parser
    } catch (exception &e) {
        cerr << "exception: " << e.what() << endl;                                  // in case of some error(file not found etc..) print it and abort.
        return EXIT_FAILURE;
    }
    if (parser->parsing() == EXIT_FAILURE) {                                        // in case of wrong input format abort.
        return EXIT_FAILURE;
    }
    unordered_set<CNF *> cnfs = parser->get_CNFS();                                 // get cnfs. Parser supports multiple ones but we use just one...
    if (cnfs.size() != 1) {
        throw new runtime_error("why is there more than 1 cnf?");                   // optional error
    }
    
    CNF cnf = *(*(cnfs.begin()));                                                   // take first one

    MPI_Init(&argc, &argv);                                                         // mpi initialization
    
    MPI_Datatype meta_data_type = setup_meta_type();                                // create data type that is used in inter process communication.
    
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);                                           // get rank and size of processes
    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    StealingWorker* worker;
    /**
     * All workers are equal except two cases: Worker 0 takes role of the master at the beginning. It takes charge of sending initial models to other workers (it always solve one of them).
     * When all workers have model to solve than worker 0 behaves as any other worker.
     * Every worker has its own local stack. In every branch, worker stores one model to local stack and continues with solving another one.
     * When it is cames to solution that current branch is unsatisfiable, it takes another model from the local stack and continues with solving.
     * However local stack will became empty at some point (in case no of the models is sat). Then worker tries to steal model from stack of the other worker.
     * If stack of other worker is not empty, than it continues solving the stolen model, otherwise waits.
     * Worker 0 takes role of master at the end as well. When some worker (can be worker 0) finds SAT model, than it sends it to worker 0, which prints it and stops other workers.
     */
    if (rank == 0) {
        // worker 0 aka "temp master"
        Measurement *measurement = new Measurement(path, "stealing");
        worker = new StealingWorker(*(*(cnfs.begin())), meta_data_type, rank, size, STEALING_RATIO, CHECK_PERIOD, MIN_STACK_SIZE);
        Config *c;
        if (arg_map["-local-cdcl"] == "-1") {
            c = new Config(worker, DPLL_);
        } else {
            c = new Config(true, std::stoi(arg_map["-local-cdcl"]), worker, DPLL_);
        }
        worker->set_config(c);
        worker->start();
        while (!worker->stopped()) {
            worker->check_and_process_message_from_worker(false);
            worker->get_model();
        }
        worker->stop_runtime();
        auto measurements = std::vector<unsigned>({worker->get_runtime(),
                                                   worker->get_waiting_time(),
                                                   worker->get_all_messages(),
                                                   worker->get_send_meta()});
        measurement->add_measurement(measurements);
        for (int i = 1; i < size; i++) {
            unsigned data[NUM_MEASUREMENTS];
            MPI_Recv(&data, NUM_MEASUREMENTS, MPI_UNSIGNED, i, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            measurement->add_measurement(std::vector<unsigned>(data, data + NUM_MEASUREMENTS));
        }
        measurement->write_to_files();
    } else {
        worker = new StealingWorker(*(*(cnfs.begin())), meta_data_type, rank, size, STEALING_RATIO, CHECK_PERIOD, MIN_STACK_SIZE);
        Config *c;
        if (arg_map["-local-cdcl"] == "-1") {
            c = new Config(worker, DPLL_);
        } else {
            c = new Config(true, std::stoi(arg_map["-local-cdcl"]), worker, DPLL_);
        }
        worker->set_config(c);
        worker->start();
        while (!worker->stopped()) {
            worker->check_and_process_message_from_worker(false);
            worker->get_model();
        }
    }
    
    MPI_Finalize();                                                                 // mpi end

    return EXIT_SUCCESS;
}
