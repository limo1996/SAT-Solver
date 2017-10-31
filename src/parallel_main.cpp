#include <iostream>
#include <vector>
#include "cnfparser.h"
#include "worker.h"
#include "Master.h"

using namespace std;

bool CERR_DEBUG = false;

int main(int argc, char *argv[]) {
    unsigned long num_workers = 1;

    CNFParser *parser;
    try {
        parser = new CNFParser(argv[1]);
    } catch (exception &e) {
        cerr << "exception: " << e.what() << endl;
        return EXIT_FAILURE;
    }
    if (parser->parsing() == EXIT_FAILURE) {
        return EXIT_FAILURE;
    }
    set<CNF *> cnfs = parser->get_CNFS();
    if (cnfs.size() != 1) {
        throw new runtime_error("why is there more than 1 cnf?");
    }

    CNF cnf = *(*(cnfs.begin()));

    MPI_Init(&argc, &argv);

    MPI_Datatype meta_data_type = setup_meta_type();

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    //TODO: this is just for testing purposes...
    if (rank == 0) {
        Master* master = new Master((size_t)size, 0, meta_data_type);
        master->start();
        if (CERR_DEBUG) {
            std::cerr << "Master: num_workers = " << size - 1 << std::endl;
        }
        while(true){
            if(master->listen_to_workers())
                break;
        }

    } else {
        Worker *w = new Worker(*(*(cnfs.begin())), meta_data_type, rank);
        w->wait_for_instructions_from_master();
    }

    MPI_Finalize();

    return 0;
}
