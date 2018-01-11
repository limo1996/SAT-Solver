//
//  Model.h
//  SAT-Solver
//
//  Created by Jakub Lichman on 10/26/17.
//  Copyright © 2017 Jakub Lichman. All rights reserved.
//

#ifndef Model_h_
#define Model_h_

/*
 Data class that stores assignments of variables in formula.
 */
class Model{
private:
    unsigned *variables;                                            // variables
    unsigned size;                                                  // size
public:
    Model();                                                        // creates empty state
    Model(unsigned * variables, unsigned int size);                 // creates state with variables and its size
    unsigned * get_variables();                                     // gets variables
    unsigned get_size();                                            // gets size of variables
    void set_variables(unsigned * variables, unsigned size);        // sets variables and its size
};

#endif /* Model_h_ */
