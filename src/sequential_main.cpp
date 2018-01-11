#include <string>
#include <fstream>
#include <chrono>
#include <map>
#include "Variable.h"
#include "Clause.h"
#include "Cnf.h"
#include "CnfParser.h"
#include "Dpll.h"
#include "ArgParsing.h"
#include "SolverController.h"

using namespace std;
using namespace std::chrono;

int CERR_LEVEL = 0;

void printHelp();

void default_args(map<string, string> *arg_map) {
    arg_map->insert({"-s", "DPLL"});
    arg_map->insert({"-cerr-level", "0"});
};

/**
 * Main entry point of sequential version.
 */
int main(int argc, char *argv[]) {
    if (argc % 2 != 0 || argc < 2 || (argc == 2 && string(argv[1]) == "--help")) {
        printHelp();
        return 0;
    }
    map<std::string, std::string> arg_map;
    default_args(&arg_map);
    parse_args(&arg_map, argc, argv);
    CERR_LEVEL = std::stoi(arg_map["-cerr-level"]);

    // for timestamp
    const char *path = argv[argc - 1];

    std::string s;
    s = path;
    size_t lastindex = s.find_last_of('.');
    string rawname = s.substr(0,lastindex);
    rawname = rawname + ".time";
    char *pathnew = &rawname[0u];

    auto *parser = new CNFParser(argv[argc - 1]);
    parser->parsing();
    unordered_set<CNF*>::iterator it_CNF;
    SolverType solver_type = arg_map["-s"] == "CDCL" ? CDCL_ : DPLL_;
    unordered_set<CNF*> cnfs = parser->get_CNFS();

    // timestamp starter
    high_resolution_clock::time_point t1 = high_resolution_clock::now();

    for(it_CNF = cnfs.begin() ; it_CNF != cnfs.end() ; it_CNF++){
        auto *config = new Config(solver_type);
        auto *controller = new SolverController(config, *it_CNF);

        bool result = controller->SATISFIABLE();

        if (result) {
            std::cout << "sat" << std::endl;
            DPLL::output_model(controller->get_cnf()->get_model());
        } else {
            std::cout << "unsat" << std::endl;
        }

        delete controller;
    }

    // time end
    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>( t2 - t1 ).count();
    ofstream myfile;
    myfile.open (pathnew, std::ios_base::app);
    myfile << std::endl << duration;
    myfile.close();

    return EXIT_SUCCESS;
}

/*
 Prints standard help for the program.
 */
void printHelp()
{
    cout << "usage: ./SAT-Solver <CNF_input_file>\n";
    cout << "Basic usage:\n";
    cout << "\t./SAT-Solver <CNF_input_file>\t prints whether formula is satisfiable or not\n"
            "\t                             \t and in case its satisfiable a model\n";
    cout << endl;
    cout << "Advanced usage:\n";
    cout << "\t-s <solver> <CNF_output_file>\t Specifies the solver to use, either DPLL or CDCL, default is DPLL.\n";
}
