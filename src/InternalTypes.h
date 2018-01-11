//
// Created by jan on 23.11.17.
//

#ifndef SAT_SOLVER_INTERNAL_TYPES_H
#define SAT_SOLVER_INTERNAL_TYPES_H

#include <vector>
#include "Clause.h"
#include "Variable.h"

// forward declarations
class Clause;
class Variable;

/**
 * One global place where we define our internally used data structures
 */
typedef std::vector<Variable *> VariableSet;
typedef std::vector<Clause *> ClauseSet;

#endif //SAT_SOLVER_INTERNAL_TYPES_H
