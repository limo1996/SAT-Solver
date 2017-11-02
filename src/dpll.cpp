#include "dpll.h"

extern int CERR_LEVEL;

/*
 *	variable* find_first_unassigned(set<variable*>*):
 *		Routine used to pick the first unassigned variable from variable's set
 */
Variable *DPLL::find_first_unassigned(std::set<Variable *> *vars) {
    std::set<Variable *>::iterator it_v;
    for (it_v = vars->begin(); it_v != vars->end(); it_v++)
        if (!(*it_v)->get_assigned()) {
            return *it_v;
        }
    return nullptr;
}

/*
 *	bool ALL_CLAUSES_ARE_TRUE(set<Clause*> *):
 *		Routine used to check whether all clauses of cnf sentence
 *		are true. If so cnf is true else return false.
 */
bool DPLL::ALL_CLAUSES_ARE_TRUE(std::set<Clause *> *clauses) {
    for (auto clause : *clauses) {
        if (!clause->is_true()) {
            return false;
        }
    }
    return true;
}

/*
 *	bool ONE_CLAUSE_IS_FALSE(set<Clause*> *):
 *		Routine used to check whether all clauses of cnf sentence
 *		are true.If so cnf is false else return false.
 */
bool DPLL::ONE_CLAUSE_IS_FALSE(std::set<Clause *> *clauses) {
    for (auto clause : *clauses) {
        if (clause->is_false()) {
            return true;
        }
    }
    return false;
}

/*
 *
 *	variable* FIND_UNIT_CLAUSE(set<Clause*> *,set<variable*> *):
 *		Routine used to find a unit clause in the set of clauses.
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

/*
 *	variable* FIND_PURE_SYMBOL(set<variable*> *)
 *		routine used to find a pure symbol in the set of variables.
 */
Variable *DPLL::FIND_PURE_VAR(CNF *cnf) {
    for (auto variable : *cnf->get_vars()) {
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

void cout_clauses(std::set<Clause *> *clauses) {
    for (auto c: *clauses) {
        if (!c->is_true()) {
            std::cerr << "(" << c->to_string() << ") ";
        }
    }
    std::cerr << std::endl;
}

/*
 *
 *	bool DPLLalgorithm(set<variable*> *,set<Clause*> *):
 *		DPLL algorithm implementation using aima's pseydocode.
 *		This effort is not based on minimizing complexity
 *		of DPLL efficiency but abstract as possible aima's algorithm.
 *		So dont expect to solve huge problems.
 *		The tested file it has succeded are on the folder spence
 *		and my_cnf_inputs.
 */
DpllResult *DPLL::DPLLalgorithm(CNF *cnf) {
    Variable *var;
    std::set<Clause *> *clauses = cnf->get_clauses();
    std::set<Variable *> *vars = cnf->get_vars();
    if (CERR_LEVEL >= 2) {
        cout_clauses(clauses);
    }
    if (ALL_CLAUSES_ARE_TRUE(clauses))
        return new DpllResult(true, cnf);
    if (ONE_CLAUSE_IS_FALSE(clauses))
        return new DpllResult(false, nullptr);
    var = FIND_PURE_VAR(cnf);
    if (var != nullptr) {
        if (CERR_LEVEL >= 2) {
            std::cerr << "pure rule on " << var->get_name() << std::endl;
        }
        set_variable_value(cnf, var, var->get_sign());
        return DPLLalgorithm(cnf);
    }
    var = FIND_UNIT_CLAUSE(cnf);
    if (var != nullptr) {
        set_variable_value(cnf, var, var->get_sign());
        if (CERR_LEVEL >= 2) {
            std::cerr << "unit clause rule on " << var->get_name() << std::endl;
            cout_clauses(cnf->get_clauses());
        }
        return DPLLalgorithm(cnf);
    }
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
    if (CERR_LEVEL >= 2) {
        std::cerr << "dppl branch on " << var->get_name() << std::endl;
    }
    CNF *cnf_copy = nullptr;
    if (config->callback_on_branch) {
        set_variable_value(cnf, var, false);
        config->worker->dpll_callback(cnf->get_model());
        unset_variable_value(cnf, var);
    } else {
        cnf_copy = new CNF(*cnf);
    }
    set_variable_value(cnf, var, true);
    DpllResult *res = DPLLalgorithm(cnf);
    if (res->sat) {
        return new DpllResult(true, res->model_cnf);
    } else {
        if (config->callback_on_branch) {
            return new DpllResult(false, nullptr);
        } else {
            if (CERR_LEVEL >= 2) {
                std::cerr << "branch !" << var->get_name() << std::endl;
            }
            set_variable_value(cnf_copy, var, false);
            return DPLLalgorithm(cnf_copy);
        }
    }
}

DPLL::DPLL(CNF _cnf, Config *_config) {                /* constructor */
    cnf = new CNF(_cnf);
    config = _config;
}

bool DPLL::DPLL_SATISFIABLE() {
    DpllResult *res = DPLLalgorithm(cnf);
    cnf = res->model_cnf;
    return res->sat;
}

CNF *DPLL::get_cnf() {
    return cnf;
}

void DPLL::output_model(std::set<Variable *> *vars) {
    for (auto v : *vars) {
        std::string true_false = v->get_value() ? "t" : "f";
        std::cout << v->get_name() << " " << true_false << std::endl;
    }

}
