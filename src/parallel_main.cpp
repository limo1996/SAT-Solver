#include <iostream>
#include <vector>
#include "cnfparser.h"
#include "worker.h"

using namespace std;

int main(int argc, char *argv[]) {
    unsigned long num_workers = 1;

    CNFParser *parser;
    try{
        parser = new CNFParser(argv[1]);
    } catch(exception &e) {
        cerr << "exception: " << e.what() << endl;
        return EXIT_FAILURE;
    }
    if (parser->parsing() == EXIT_FAILURE) {
        return EXIT_FAILURE;
    }
    set<CNF*> cnfs = parser->get_CNFS();
    if (cnfs.size() != 1) {
        throw new runtime_error("why is there more than 1 cnf?");
    }

    CNF cnf = *(*(cnfs.begin()));

    vector<Worker*> workers(num_workers);
    for (int i=0; i<num_workers; i++) {
        std::cout << "what" << std::endl;
        workers[i] = new Worker(*(*(cnfs.begin())));
    }

    return 0;
}
