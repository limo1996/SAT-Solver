#include <iostream>
#include <vector>
#include <chrono>
#include <unistd.h>
#include <fstream>
#include "cnfparser.h"
#include "worker.h"


using namespace std;
using namespace std::chrono;

int main(int argc, char *argv[]) {

    string name = "example.txt";
    const char* path = name.c_str();
    if(ifstream(path))
    {
        // std::cout << "File already exists" << std::endl;
        remove(path);
    }

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

    // timestamp start
    high_resolution_clock::time_point t1 = high_resolution_clock::now();

    MPI_Init(&argc, &argv);

    MPI_Datatype meta_data_type = setup_meta_type();

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    //TODO: this is just for testing purposes...
    if (rank == 0) {
        std::cerr << "Master: num_workers = " << size -1 << std::endl;
        for (int i=1; i<size; i++) {
            struct meta meta;
            meta.message_type = 0;
            meta.count = 0;
            MPI_Send(&meta, 1, meta_data_type, i, 0, MPI_COMM_WORLD);
        }
    } else {
        Worker *w = new Worker(*(*(cnfs.begin())), meta_data_type, rank);
    }

    MPI_Finalize();

    // timestamp end
    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>( t2 - t1 ).count();

    cout << "RunTime: " << duration << " ms " << std::endl;;

    ofstream myfile;
    myfile.open (path);
    myfile << "RunTime: " << duration << "\n";
    myfile.close();

    return 0;
}
