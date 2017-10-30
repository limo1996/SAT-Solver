//
//  State.hpp
//  SAT-Solver
//
//  Created by Jakub Lichman on 10/26/17.
//  Copyright © 2017 Jakub Lichman. All rights reserved.
//

#ifndef State_hpp
#define State_hpp

/*
 Data class that stores assignments of variables in formula.
 */
class State{
private:
    unsigned int *variables;                                            // variables
    unsigned int size;                                                  // size
public:
    State();                                                            // creates empty state
    State(unsigned int * variables, unsigned int size);                 // creates state with variables and its size
    unsigned int * get_variables();                                     // gets variables
    unsigned int get_size();                                            // gets size of variables
    void set_variables(unsigned int * variables, unsigned int size);    // sets variables and its size
};

#endif /* State_hpp */
