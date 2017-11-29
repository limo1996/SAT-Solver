#include <iostream>
#include <chrono>
#include <unistd.h>
#include <fstream>

#include "cnfparser.h"
#include "StealingWorker.h"

using namespace std;
using namespace std::chrono;

int CERR_LEVEL = 0;

/**
 * Main entry point to parallel stealing version.
 */
int main(int argc, char *argv[]) {
    // for time measurment
    const char *path = argv[1];
    
    std::string s;
    s = path;
    size_t lastindex = s.find_last_of(".");
    string rawname = s.substr(0,lastindex);
    rawname = rawname + ".time";
    char *pathnew = &rawname[0u];
    
    CNFParser *parser;
    try {
        parser = new CNFParser(argv[1]);                                            // create parser
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
    
    // time start
    high_resolution_clock::time_point t1 = high_resolution_clock::now();
    
    MPI_Init(&argc, &argv);                                                         // mpi initialization
    
    MPI_Datatype meta_data_type = setup_meta_type();                                // create data type that is used in inter process communication.
    
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);                                           // get rank and size of processes
    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
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
        ofstream file;
        file.open(pathnew, std::ios_base::app);
        file << std::endl;
        file.close();
        
        // worker 0 aka "temp master"
        StealingWorker* main_worker = new StealingWorker(*(*(cnfs.begin())), meta_data_type, rank, size);
        main_worker->start();
        while (!main_worker->stopped()) {
            main_worker->check_and_process_message_from_worker(true);
        }
    } else {
        StealingWorker *w = new StealingWorker(*(*(cnfs.begin())), meta_data_type, rank, size);
        while (!w->stopped()) {
            w->check_and_process_message_from_worker(true);
        }
    }
    
    MPI_Finalize();                                                                 // mpi end
    
    // time end
    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>( t2 - t1 ).count();
    ofstream myfile;
    myfile.open (pathnew, std::ios_base::app);
    myfile << duration << ' ';
    myfile.close();
    //cout << "RunTime: " << duration << " ms " << std::endl;
    
    return EXIT_SUCCESS;
}
