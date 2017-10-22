#include "variable.h"
#include "clause.h"
#include "CNF.h"
#include "cnfparser.h"
#include "dpll.h"

#include <iostream>
#include <cstring>

using namespace std;

void printHelp();
void printErr();
bool readArgv(char *argv[], int argc, int pos, bool *flags, int &format, string& outputFile);

int main(int argc, char *argv[])
{
    CNFParser *parser;
    set<CNF*>::iterator it_CNF;
    DPLL *dpll;
    string outputFile;
    int format = 1;                         // default format is 1 i.e. readable format.
    bool flags[3] = {false, false, false};  // [0] => -p flag set, [1] => -pa flag set, [2] => -o flag set
    
    if(argc < 2){
        printErr();
        return EXIT_FAILURE;
    } else {
        if(strcmp("--help", argv[1]) == 0){
            printHelp();
            return EXIT_SUCCESS;
        }
        
        try{
            parser = new CNFParser(argv[1]);
        } catch(exception &e) {
            cerr << "exception: " << e.what() << endl;
            return EXIT_FAILURE;
        }
        
        if(argc > 2 && !readArgv(argv, argc, 2, flags, format, outputFile))
            return EXIT_FAILURE;
        if(argc > 4 && !readArgv(argv, argc, 4, flags, format, outputFile))
            return EXIT_FAILURE;
        if(argc > 6 && !readArgv(argv, argc, 6, flags, format, outputFile))
            return EXIT_FAILURE;
        if(argc > 8){
            printErr();
            return EXIT_FAILURE;
        }
    }

    cout << "Reading Data ...";
    fflush(stdout);
    if(parser->parsing()==EXIT_FAILURE)return EXIT_FAILURE;
    cout<<"OK!"<<endl;
    set<CNF*> cnfs = parser->get_CNFS();
    for(it_CNF = cnfs.begin() ; it_CNF != cnfs.end() ; it_CNF++){
        dpll = new DPLL(*(*it_CNF));
        bool result = dpll->DPLL_SATISFIABLE();
        cout<<(*it_CNF)->get_sentence();
        if(result==true) {
            cout<<" ->  satisfiable"<<endl;
            if(flags[0] || flags[1])
                dpll->print((*it_CNF)->get_clauses(), (*it_CNF)->get_var(), flags[1]);
        } else
            cout<<" ->  not satisfiable"<<endl;
        delete dpll;
    }
    return EXIT_SUCCESS;
}

/*
 Prints standard help for the program.
 */
void printHelp()
{
    cout << "usage: ./SAT-Solver <CNF_input_file> [-o <CNF_output_file>] \n\t\t    [-p <format>] [-pa <format>]\n\n";
    cout << "Basic usage:\n";
    cout << "   ./SAT-Solver <CNF_input_file>\tPrints formula in readable form \n\t\t\t\t\tand whether it is satisfiable or not.\n";
    cout << endl;
    cout << "Advanced usage:\n";
    cout << "   -o <CNF_output_file>\tSpecifies the output file.\n";
    cout << "   -p <format>\t\tBasic extended output (includes variables values).\n";
    cout << "   -pa <format>\t\tExtended output (includes variables values and clauses).\n";
    cout << "   <format>\t\t1 - readable, 2 - for further processing\n";
}

/*
 Prints argument error.
 */
void printErr()
{
    cerr << "Wrong input Data" << endl;
    cerr << "Modify your input as follows or tap ./<executable> --help: " << endl;
    cerr << "\t./<executable> <CNF_input_file> [-o <CNF_output_file>] [-p <format>] [-pa <format>] " << endl;
}

/*
 Checks if the format is valid. If it is not than prints error message.
 */
bool printErrFormat(int format){
    if(format != 1 && format != 2){
        cerr << "Wrong input Data" << endl;
        cerr << "Input format is invalid. It can be either 1 for readable form or 2 for further processing." << endl;
        return false;
    }
    return true;
}

/*
 reads pair of arguments starting at pos.
 */
bool readArgv(char *argv[], int argc, int pos, bool *flags, int &format, string& outputFile)
{
    if(strcmp(argv[pos], "-p") == 0){
        flags[0] = true;
        if(pos + 1 < argc){                         // format does not have to be specified since it has default value
            if(!flags[1]){                          // flag -pa has higher priority than -p. If are both set than -p is ignored.
                format = atoi(argv[pos + 1]);
                return printErrFormat(format);
            }
        }
    } else if(strcmp(argv[pos], "-pa") == 0){
        flags[1] = true;
        if(pos + 1 < argc){                         // format does not have to be specified since it has default value
            format = atoi(argv[pos + 1]);
            return printErrFormat(format);
        }
    } else if(strcmp(argv[pos], "-o") == 0){
        flags[2] = true;
        if(pos + 1 >= argc){                        // if -o is set then output path has to be specified
            printErr();
            return false;
        }
        outputFile = string(argv[pos + 1]);
    } else {
        printErr();
        return false;
    }
    return true;
}
