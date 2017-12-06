#include <iostream>
#include <chrono>
#include <unistd.h>
#include <fstream>

#include "cnfparser.h"
#include "SlaveWorker.h"
#include "Master.h"

using namespace std;
using namespace std::chrono;

int CERR_LEVEL = 0;

/**
 * Main entry point to sequential version.
 */
int main(int argc, char *argv[]) {
    // for time measurment
    const char *path = argv[1];

    std::string s;
    s = path;
    size_t lastindex = s.find_last_of(".");
    string rawname = s.substr(0,lastindex);
    string waitname = rawname + "_parallel.wait";
    string commname = rawname + "_parallel.comm";
    rawname = rawname + "_parallel.time";
    char *pathnew = &rawname[0u];
    char *waitpath = &waitname[0u];
    char *commpath = &commname[0u];

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
    unordered_set<CNF *> cnfs = parser->get_CNFS();                                           // get cnfs. Parser supports multiple ones but we use just one...
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
    int communication;
    /**
     * Rank 0 is always master. Other n-1 ranks are workers. If program is run just on 1 process, the program will fail (Master will cause assertion failure).
     * Workers are listening to commands of master and are executing tasks. Worker can get to 3 states. It finds solution and let master know it,
     * finds out that current assignment is not correct and let master know it(master knows that he is free now)
     * or it can branch(i.e. it finds two subproblems to solve. It will pick first one and send second one to master.
     * Master sends tasks to free workers and collects tasks that needs to be done from them. If someone found solution master stops all workers and program ends.
     */
    if (rank == 0) {
        ofstream file, file2;
        file.open(pathnew, std::ios_base::app);
        file2.open(waitpath, std::ios_base::app);
        file << std::endl;
        file2 << std::endl;
        file.close();
        file2.close();

        Master* master = new Master((size_t)size, 0, meta_data_type);
        master->start();

        while(!master->listen_to_workers());
        communication = master->get_all_messages();
        
    } else {
        SlaveWorker *w = new SlaveWorker(*(*(cnfs.begin())), meta_data_type, rank);
        w->wait_for_instructions_from_master();
        
        ofstream waitfile;
        waitfile.open(waitpath, std::ios_base::app);
        waitfile << w->get_waiting_time() << ' ';
        waitfile.close();
        communication = w->get_all_messages();
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
    
    ofstream commfile;
    commfile.open(commpath, std::ios_base::app);
    commfile << communication << ' ';
    commfile.close();

    return EXIT_SUCCESS;
}
