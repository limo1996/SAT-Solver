/****************************************************************************
 *	Implemented by:	Psallidas Fotis                                         *
 *	A.M.:1115200600170                                                      *
 *	email:std06170@di.uoa.gr                                                *
 *	Before starting executing and reading                                   *
 * 	the code read the readme.txt file                                       *
 *  http://www.cs.columbia.edu/~fotis/Artificial_Intelligence/dpll/dpll.cpp *
  **************************************************************************/

#include "cnfparser.h"
#include "exceptions.cpp"

#include <fstream>
#include <sstream>
#include <iostream>

CNFParser::CNFParser(char* filename) {			/* constructor */
    struct stat info;
    if(stat(filename,&info))
        throw file_not_exists(filename);
    input = new std::fstream(filename, std::fstream::in);	/* take the .cnf stream */
    if(input->fail())
        throw file_open_fail(filename);
}
bool CNFParser::fix_pureness(const Variable *v) {		/* check if a variable already exists and fix the pureness if so */
    std::set<Variable *>::iterator it_v;
    bool flag = true;
    for(it_v = var.begin() ; it_v != var.end() ; it_v++) {
        if((*it_v)->get_name()==v->get_name() ) {
            if( v->get_sign() !=(*it_v)->get_sign() ) (*it_v)->set_pure(false);
            flag = false;
            break;
        }
    }
    return flag;
}
void CNFParser::printCNF() {
    std::set<Clause*>::iterator it_c;
    std::cout<<"CNF: ";
    for(it_c = Clauses.begin() ; it_c != Clauses.end() ; it_c++) {
        std::cout << "( ";
        (*it_c)->print();
        std::cout << " ) ";
        if((++it_c) != Clauses.end())
            std::cout<<" v ";
        it_c--;
    }
    std::cout << std::endl;
}

std::string CNFParser::take_format(){
    std::set<Clause*>::iterator it_c;
    std::string cnf;
    cnf = "CNF: ";
    for(it_c = Clauses.begin() ; it_c != Clauses.end() ; it_c++) {
        cnf += "( ";
        cnf+=(*it_c)->get_unit();
        cnf += " ) ";
        if((++it_c) != Clauses.end() )cnf+=" v ";
        it_c--;
    }
    return cnf;
}

int CNFParser::parsing(){
    Clause *clause;
    CNF *cnf;
    std::set<Variable*> _var;
    std::string line, VAR, name, p;
    Variable *v,*vtmp;
    bool sign,pflag = false ,cflag = false;
    std::istringstream *ins;
    int i = 1,j = 0 ,nvars,nclauses;
    *input >> std::ws;

    std::getline(*input,line);
    while(!input->eof()){                                               /* getline from file */
        switch (line[0]){                                               /* check the input line first character */
        case 'c':                                                       /* if it is a 'c' then the line is a comment */
            *input >> std::ws;                                          /* so read whitespaces and */
            cflag = true;
            break;                                                      /* continue to the new line */
        case 'p':                                                       /* if it is a p then a new cnf starts */
            ins = new std::istringstream(line);                         /* assign the line to a stringstream */
            *ins >> std::ws >> p >> std::ws >> nvars >> std::ws >> nclauses >> std::ws;		/* take the number of variables and the number of clauses */
            delete ins;
            *input>> std::ws;                                           /* read whitespaces from input .cnf file */
            while(std::getline(*input,line)) {                          /* read the .cnf sentence */
                j++;
                if(line[0]=='c') {                                      /* if line starts with a c then cnf ended */
                    cflag = true;
                    break;
                } else if(line[0]=='p') {                               /* if line starts with a p then a new cnf takes place */
                    pflag = true;
                    break;
                } else {                                                /* else it is a clause */
                    ins = new std::istringstream(line);                 /* pass the clause to stringstream */
                    *ins >> std::ws;                                    /* read whitespaces from stringstream */
                    while(!ins->eof()) {                                /* while not the end of line */
                        *ins >> std::ws;
                        *ins>>VAR;                                      /* take the variable */
                        if(VAR =="0") break;                            /* if 0 then the clause ends */
                        else if(VAR[0]=='-') {                          /* if variable has e negative sign */
                            sign = false;                               /* assign its sign as false */
                            name = VAR.substr(1);                       /* take the name of the variable */
                        } else {                                        /* else variable has a positive sign */
                            sign = true;                                /* so assign its sign as true */
                            name = VAR;                                 /* and take its name */
                        }
                        v = new Variable(sign,true,true,name);          /* create a new variable */
                        if( fix_pureness(v) == true ) {                 /* check if variable already exists in variables' set */
                            vtmp = new Variable(*v);                    /* if doesn't exist */
                            var.insert(vtmp);                           /* insert nariable into the variable set */
                            vtmp = NULL;
                        }
                        _var.insert(v);                                 /* insert into clause's variables set */
                    }
                    clause = new Clause(_var);                          /* create new clause */
                    _var.clear();
                    Clauses.insert(clause);                             /* insert to cnf's clauses */
                    clause = NULL;
                    *input >> std::ws;
                    line.clear();
                    delete ins;
                }
            }
            cnf = new CNF(var,Clauses,take_format());                   /* create new cnf */
            cnfs.insert(cnf);                                           /* add to cnfs (to solve) set */
            Clauses.clear();
            var.clear();
            cnf = NULL;
            break;
        default:
            std::cerr << "Error income" << std::endl;
            return EXIT_FAILURE;
        }
        if(pflag == true)
            pflag = false;
        else if(cflag == true) {
            cflag = false;
            line.clear();
            *input >> std::ws;
            std::getline(*input,line);
        } else {
            line.clear();
            *input >> std::ws;
            std::getline(*input,line);
        }
    }
    return EXIT_FAILURE;vers
}

std::set<CNF*> CNFParser::get_CNFS(){
    return cnfs;
}
