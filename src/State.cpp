//
//  State.cpp
//  SAT-Solver
//
//  Created by Jakub Lichman on 10/26/17.
//  Copyright © 2017 Jakub Lichman. All rights reserved.
//

#include "State.h"

// creates empty state
State::State(){}

// creates state with variables and its size
State::State(unsigned int * variables, unsigned int size){
    this->set_variables(variables, size);
}

// gets variables
unsigned int * State::get_variables(){
    return this->variables;
}

// gets size of variables
unsigned int State::get_size(){
    return this->size;
}

// sets variables and its size
void State::set_variables(unsigned int * variables, unsigned int size){
    this->variables = variables;
    this->size = size;
}
