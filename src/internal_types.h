//
// Created by jan on 23.11.17.
//

#ifndef SAT_SOLVER_INTERNAL_TYPES_H
#define SAT_SOLVER_INTERNAL_TYPES_H

#include <unordered_set>
#include "clause.h"
#include "variable.h"

class Clause;
class Variable;

typedef std::unordered_set<Variable *> VariableSet;
typedef std::unordered_set<Clause *> ClauseSet;

#endif //SAT_SOLVER_INTERNAL_TYPES_H
