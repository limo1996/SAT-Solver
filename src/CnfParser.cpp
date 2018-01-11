/****************************************************************************
 *	Inspired by an implementation by:	Psallidas Fotis                     *
 *	A.M.:1115200600170                                                      *
 *	email:std06170@di.uoa.gr                                                *
 *	Before starting executing and reading                                   *
 * 	the code read the readme.txt file                                       *
 *  http://www.cs.columbia.edu/~fotis/Artificial_Intelligence/dpll/dpll.cpp *
  **************************************************************************/

#include "CnfParser.h"
#include "Exceptions.cpp"
#include <sys/stat.h>

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

int CNFParser::parsing(){
    Clause *clause;
    CNF *cnf;
    VariableSet _var;
    std::string line, VAR, p;
    Variable *v,*vtmp;
    bool sign,pflag = false ,cflag = false;
    std::istringstream *ins;
    int i = 1,j = 0 ,nvars,nclauses;
    unsigned name;
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
                            name = std::stoi(VAR.substr(1));            /* take the name of the variable */
                        } else {                                        /* else variable has a positive sign */
                            sign = true;                                /* so assign its sign as true */
                            name = std::stoi(VAR);                      /* and take its name */
                        }
                        v = new Variable(sign, true, name);             /* create a new variable */
                        _var.push_back(v);                                 /* insert into clause's variables set */
                    }
                    clause = new Clause(_var);                          /* create new clause */
                    _var.clear();
                    Clauses.push_back(clause);                             /* insert to cnf's clauses */
                    clause = NULL;
                    *input >> std::ws;
                    line.clear();
                    delete ins;
                }
            }
            cnf = new CNF(Clauses);                   /* create new cnf */
            cnfs.insert(cnf);                                           /* add to cnfs (to solve) set */
            Clauses.clear();
            var.clear();
            cnf = NULL;
            break;
        default:
            std::cout << "Error income" << std::endl;
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
    return EXIT_SUCCESS;
}

std::unordered_set<CNF*> CNFParser::get_CNFS(){
    return cnfs;
}
