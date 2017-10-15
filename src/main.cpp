#include "variable.h"
#include "clause.h"
#include "cnf.h"
#include "cnfparser.h"
#include "dpll.h"

#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
    CNFParser *parser;
        set<CNF*>::iterator it_CNF;
        DPLL *dpll;
        if(argc != 2 ) {
            if(argc != 3){
    argerr:			cerr<<"Wrong input Data "<<endl;
                cerr<<"Modify your input as follows and retry: "<<endl;
                cerr<<"\t./<executable> <CNF_input_file> or ./<executable> <CNF_input_file> -p "<<endl;
                return EXIT_FAILURE;
            }else if(strcmp(argv[2],"-p"))goto argerr;
        }
        try{
            parser = new CNFParser(argv[1]);
        } catch(exception &e) {
            cerr<<"exception: "<<e.what()<<endl;
            return EXIT_FAILURE;
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
                if( argc == 3)
                if(!strcmp(argv[2],"-p"))
                dpll->print((*it_CNF)->get_clauses(), (*it_CNF)->get_var());
            } else
                cout<<" ->  not satisfiable"<<endl;
            delete dpll;
        }
        return EXIT_SUCCESS;
}
