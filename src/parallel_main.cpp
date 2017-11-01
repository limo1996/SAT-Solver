#include <iostream>
#include "cnfparser.h"
#include "worker.h"
#include "Master.h"

using namespace std;

bool CERR_DEBUG = false;

/**
 * Main entry point to sequential version.
 */
int main(int argc, char *argv[]) {
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
    set<CNF *> cnfs = parser->get_CNFS();                                           // get cnfs. Parser supports multiple ones but we use just one...
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

    /**
     * Rank 0 is always master. Other n-1 ranks are workers. If program is run just on 1 process, the program will fail (Master will cause assertion failure).
     * Workers are listening to commands of master and are executing tasks. Worker can get to 3 states. It finds solution and let master know it,
     * finds out that current assignment is not correct and let master know it(master knows that he is free now)
     * or it can branch(i.e. it finds two subproblems to solve. It will pick first one and send second one to master.
     * Master sends tasks to free workers and collects tasks that needs to be done from them. If someone found solution master stops all workers and program ends.
     */
    if (rank == 0) {
        Master* master = new Master((size_t)size, 0, meta_data_type);
        master->start();
        
        while(!master->listen_to_workers());

    } else {
        Worker *w = new Worker(*(*(cnfs.begin())), meta_data_type, rank);
        w->wait_for_instructions_from_master();
    }

    MPI_Finalize();                                                                 // mpi end

    return EXIT_SUCCESS;
}
