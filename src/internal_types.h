//
// Created by jan on 23.11.17.
//

#ifndef SAT_SOLVER_INTERNAL_TYPES_H
#define SAT_SOLVER_INTERNAL_TYPES_H

#include <vector>
#include "clause.h"
#include "variable.h"

class Clause;
class Variable;

typedef std::vector<Variable *> VariableSet;
typedef std::vector<Clause *> ClauseSet;

#endif //SAT_SOLVER_INTERNAL_TYPES_H
