#include "Dpll.h"

extern int CERR_LEVEL;

/**
 * Returns the first unassigned variable or nullptr if all variables are assigned
 */
Variable *DPLL::find_first_unassigned(VariableSet *vars) {
    for (auto v : *vars) {
        if (!v->get_assigned()) {
            return v;
        }
    }
    return nullptr;
}

/**
 * Checks if all clauses are true
 */
bool DPLL::ALL_CLAUSES_ARE_TRUE(ClauseSet *clauses) {
    for (auto clause : *clauses) {
        if (!clause->is_true()) {
            return false;
        }
    }
    return true;
}

/**
 * Checks if there exists a clause that is false
 */
bool DPLL::ONE_CLAUSE_IS_FALSE(ClauseSet *clauses) {
    for (auto clause : *clauses) {
        if (clause->is_false()) {
            return true;
        }
    }
    return false;
}

/**
 * Goes through the cnf formula and looks for a unit clause
 * @param cnf the cnf object that represents the original formula and a partial model (var->value assignment)
 * @return the variable of the unit clause if it found one, nullptr otherwise
 */
Variable *DPLL::FIND_UNIT_CLAUSE(CNF *cnf) {
    for (auto c : *cnf->get_clauses()) {
        if (!c->is_true()) {
            long count = c->get_vars()->size();
            Variable *unasigned = nullptr;
            for (auto cv: *(c->get_vars())) {
                if (cv->get_assigned()) {
                    count--;
                } else {
                    unasigned = cv;
                }
            }
            if (count == 1) {
                return unasigned;
            }
        }
    }
    return nullptr;
}

/**
 * sets a given variable to a given value in the whole cnf
 */
void DPLL::set_variable_value(CNF *cnf, Variable *var, bool value) {
    for (auto variable : *cnf->get_vars()) {
        if (variable->get_name() == var->get_name()) {
            variable->set_value(value);
            variable->set_assigned(true);
        }
    }
}

void DPLL::unset_variable_value(CNF *cnf, Variable *var) {
    for (auto variable : *cnf->get_vars()) {
        if (variable->get_name() == var->get_name()) {
            variable->set_assigned(false);
        }
    }
}

/**
 * Goes through the cnf formula and looks for a pure variable
 * @param cnf the cnf object that represents the original formula and a partial model (var->value assignment)
 * @return a pure variable if it found one, nullptr otherwise
 */
Variable *DPLL::FIND_PURE_VAR(CNF *cnf) {
    for (auto variable : *cnf->get_model()) {
        bool occurred_positively = false;
        bool occurred_negatively = false;
        for (auto c: *cnf->get_clauses()) {
            if (!c->is_true()) {
                for (auto cv: (*c->get_vars())) {
                    if (cv->get_name() == variable->get_name()) {
                        if (cv->get_assigned() && !cv->get_sign()) {
                            occurred_negatively = true;
                        } else if (cv->get_assigned() && cv->get_sign()) {
                            occurred_positively = true;
                        }
                    }
                    if (occurred_positively && occurred_negatively) {
                        break;
                    }
                }
            }
        }
        if (occurred_positively && occurred_negatively) {
            return variable;
        }
    }
    return nullptr;
}

/**
 * For debugging purposes: outputs a given set of clauses to std::cerr
 */
void DPLL::cout_clauses(ClauseSet *clauses) {
    for (auto c: *clauses) {
        if (!c->is_true()) {
            std::cerr << "(" << c->to_string() << ") ";
        }
    }
    std::cerr << std::endl;
}

/**
 * Run dpll on a given cnf object
 * @param cnf the cnf object that represents the original formula and a partial model (var->value assignment)
 * @return DpllResult object
 */
DpllResult *DPLL::DPLLalgorithm(CNF *cnf) {
    Variable *var;
    ClauseSet *clauses = cnf->get_clauses();
    VariableSet *vars = cnf->get_vars();
    if (CERR_LEVEL >= 3) {
        DPLL::cout_clauses(clauses);
    }

    // Is the formula sat for the given partial model?
    if (ALL_CLAUSES_ARE_TRUE(clauses)) {
        return new DpllResult(true, cnf);
    }

    // Is the formula unsatisfiable for the given partial model?
    if (ONE_CLAUSE_IS_FALSE(clauses)) {
        return new DpllResult(false, nullptr);
    }

    // Pure literal rule
    var = FIND_PURE_VAR(cnf);
    if (var != nullptr) {
        if (CERR_LEVEL >= 2) {
            std::cerr << "pure rule on " << var->get_name() << std::endl;
        }
        set_variable_value(cnf, var, var->get_sign());
        return DPLLalgorithm(cnf);
    }

    // Unit clause rule
    var = FIND_UNIT_CLAUSE(cnf);
    if (var != nullptr) {
        set_variable_value(cnf, var, var->get_sign());
        if (CERR_LEVEL >= 2) {
            std::cerr << "unit clause rule on " << var->get_name() << std::endl;
        }
        if (CERR_LEVEL >= 3) {
            DPLL::cout_clauses(cnf->get_clauses());
        }
        return DPLLalgorithm(cnf);
    }

    // branch on first unassigned variable
    var = find_first_unassigned(vars);
    if (var == nullptr) {
        return new DpllResult(false, nullptr);
    } else {
        return branch_on_variable(var, cnf);
    }
}

/**
 * Handles the branching of DPLL, if we need to make a decision, it either calls the callback function registered in the
 * config or runs both branches recursively
 *
 * The first option is used in the standard setting (sequential)
 * The second option is used in the parallel setting
 */
DpllResult *DPLL::branch_on_variable(Variable *var, CNF *cnf) {
    if (CERR_LEVEL >= 1) {
        std::cerr << "dppl branch on " << var->get_name() << std::endl;
    }
    bool solve_locally = config->branching_limit <= 0 || config->worker == nullptr;
    if (solve_locally) {
        if (config->force_cdcl) {
            if (CERR_LEVEL >= 2) {
                std::cerr << "switching to local cdcl" << std::endl;
            }
            CNF *new_cnf = cnf->build_fresh_cnf_from();
            auto *solver = new CDCL(new_cnf);
            DpllResult *res = solver->CDCLAlgorithm(new_cnf);
            if (res->sat) {
                cnf->overwrite_assignments(res->model_cnf->get_partial_model());
                res->model_cnf = cnf;
            }
            return res;
        } else {
            auto *cnf_copy = new CNF(*cnf);
            set_variable_value(cnf, var, true);
            DpllResult *res = DPLLalgorithm(cnf);
            if (res->sat) {
                return res;
            } else {
                delete cnf;
                set_variable_value(cnf_copy, var, false);
                return DPLLalgorithm(cnf_copy);
            }
        }
    } else {
        config->branching_limit--;
        set_variable_value(cnf, var, false);
        config->worker->dpll_callback(cnf->get_model());
        if (config->worker->get_stop()) {
            return new DpllResult(false, nullptr);
        } else {
            unset_variable_value(cnf, var);
            set_variable_value(cnf, var, true);
            return DPLLalgorithm(cnf);
        }
    }
}

DPLL::DPLL(CNF _cnf, Config *_config) {                /* constructor */
    cnf = new CNF(_cnf);
    config = _config;
}

bool DPLL::SATISFIABLE() {
    DpllResult *res = DPLLalgorithm(cnf);
    cnf = res->model_cnf;
    return res->sat;
}

CNF *DPLL::get_cnf() {
    return cnf;
}

void DPLL::output_model(VariableSet *vars) {
    for (auto v : *vars) {
        std::string true_false = v->get_value() ? "t" : "f";
        std::cout << v->get_name() << " " << true_false << std::endl;
    }
}
