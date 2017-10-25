#include "dpll.h"

void DPLL::restore_symbol(Variable *v ){
    v->set_assigned(false);
}

/*
 *	bool ALL_VARIABLES_ARE_FALSE(Clause*):
 *		Routine used to decide whether a clause is false.
 *		So it checks if all of the variables of the clause are false.
 */
bool DPLL::ALL_VARIABLES_ARE_FALSE(Clause* cl){
    std::set<Variable*>::iterator it_v;
    for( it_v = cl->get_var()->begin() ; it_v != cl->get_var()->end() ; it_v++){
        if( (*it_v)->get_value() == true  || (*it_v)->get_assigned() == false) return false;
    }
    return true;
}

/*
 *	bool ONE_VARIABLE_IS_FALSE(clause* cl)
 *		Routine used to decide whether a clause is true.
 *		So it looks if one of the variables in the clause is true.
 */
bool DPLL::ONE_VARIABLE_IS_TRUE(Clause* cl){
    std::set<Variable*>::iterator it_v;
    for( it_v = cl->get_var()->begin() ; it_v != cl->get_var()->end() ; it_v++) {
        if( (*it_v)->get_assigned() == true && (*it_v)->get_value() == true) return true;
    }
    return false;
}

/*
 *	void fix_clauses(variable *,set<Clause*> *, bool, bool, bool)
 *		Routine used to fix the value of the variables and the total values of the clauses
 *		when the variable var get assigned into the solution set.
 */
void DPLL::fix_clauses(Variable *var, std::set<Clause*> *clauses, bool lvalue, bool flag, bool unit){
    std::set<Clause*>::iterator it_c;
    std::set<Variable*>::iterator it_v;
    for(it_c = clauses->begin() ; it_c!=clauses->end() ; it_c++){                               /* For all clauses */
        for(it_v = (*it_c)->get_var()->begin() ; it_v != (*it_c)->get_var()->end() ; it_v++){   /* For all variables in the clause */
            if((*it_v)->get_name()==var->get_name()) {                                          /* If we found the same variable as the assigned one */
                (*it_v)->set_assigned(true);                                                    /* set the variable as assigned for the clause*/
                /* and give the proper value */
                if(flag == true) {
                    (*it_v)->set_value(true);
                    (*it_v)->set_real_value((*it_v)->get_sign());
                } else {
                    if((*it_v)->get_sign() == true) (*it_v)->set_value(lvalue);
                    else				(*it_v)->set_value(!lvalue);
                }
            }
        }
        if((*it_c)->get_assigned() == true)continue;                                            /* if clause is assigned continue */
        if(ALL_VARIABLES_ARE_FALSE(*it_c)==true){                                               /* Check if clause can now be set to false */
            (*it_c)->set_assigned(true);                                                        /* if so set the clause as assigned */
            (*it_c)->set_value(false);                                                          /* and give it its proper value */
            continue;                                                                           /* continue with the rest clauses */
        }
        if( ONE_VARIABLE_IS_TRUE(*it_c) ==true){                                                /* check if clause can now be set to true */
            (*it_c)->set_assigned(true);                                                        /* if so set the clause as assigned */
            (*it_c)->set_value(true);                                                           /* and give its proper value */
            continue;                                                                           /* continue with the rest clauses */
        }
    }
};

/*
 *	void restore_clauses(variable*, set<Clause*> *):
 *		Routine used to rebuild the model when the assignement
 *		of variable var took place.
 */
void DPLL::restore_clauses(Variable* var, std::set<Clause*> *clauses) {
    std::set<Clause*>::iterator it_c;
    std::set<Variable*>::iterator it_v;
    for(it_c = clauses->begin() ; it_c!=clauses->end() ; it_c++){                               /* For all clauses */
        for(it_v = (*it_c)->get_var()->begin() ; it_v != (*it_c)->get_var()->end() ; it_v++){   /* For all variables in the clause */
            if((*it_v)->get_name()==var->get_name()) {                                          /* If we found the same variable as the unassigned one */
                (*it_v)->set_assigned(false);                                                   /* ÎšÎ¬Î½Ï„Î·Î½ assigned Î³Î¹Î± Ï„Î¿ ÏƒÏ…Î³ÎºÎµÎºÏÎ¹Î¼Î­Î½Î¿ clause */
            }
        }
        if( (*it_c)->get_assigned()==false ) continue;                                          /* if the clause is already unassigned continue */
        if(ALL_VARIABLES_ARE_FALSE(*it_c)==true){                                               /* if clause remains having all variables false */
            (*it_c)->set_assigned(true);                                                        /* set it as assigned */
            (*it_c)->set_value(false);                                                          /* set the proper vale */
            continue;                                                                           /* continue with the rest of the values */
        }
        if( ONE_VARIABLE_IS_TRUE(*it_c) ==true){                                                /* if clause remains having one variable true */
            (*it_c)->set_assigned(true);                                                        /* set it as assigned */
            (*it_c)->set_value(true);                                                           /* set the proper value */
            continue;
        }
        (*it_c)->set_assigned(false);                                                           /* set the clause as unassigned */
    }
}

/*
 *	void fix_variables(variable *,set<variable*>*,bool)
 *		Routine used to fix the variables' set after
 *		variable's v assignement.
 */
void DPLL::fix_variables(Variable *v, std::set<Variable*>* var, bool u){
    std::set<Variable*>::iterator it_v;
    for(it_v = var->begin() ; it_v != var->end() ; it_v++){                                     /* for all variables */
        if((*it_v)->get_name() == v->get_name()) {                                              /* if we found the variable */
            if(u == true){
                (*it_v)->set_value(v->get_sign());                                              /* set the right value to variable */
                (*it_v)->set_sign(v->get_sign());                                               /* set the proper sign to the variable */
            }
            (*it_v)->set_assigned(true);                                                        /* set variable as assigned */
            return;
        }
    }
    return;
}

/*
 *	void restore_variables(variable *,set<variable*>*)
 *		Routine used to rebuild the variables' set after
 *		variable's v unassignement.
 */
void DPLL::restore_variables(Variable *v, std::set<Variable*>* var) {
    std::set<Variable*>::iterator it_v;
    for(it_v = var->begin() ; it_v != var->end() ; it_v++){                                     /* for all variables */
        if((*it_v)->get_name() == v->get_name()) {                                              /* if we found the variable */
            (*it_v)->set_assigned(false);                                                       /* set is as unassigned */
            return;
        }
    }
}

/*
 *	void fix_pures(set<variable*> *,set<Clause*> *)
 *		Variables used to fix the pureness of the variables.
 */
void DPLL::fix_pures(std::set<Variable*> *vars, std::set<Clause*> *clauses) {
    std::set<Variable*>::iterator it_v,it_cv;
    std::set<Clause*>::iterator it_c;
    Variable *var;
    int i;
    bool sign;
    for( it_v = vars->begin() ; it_v != vars->end() ; it_v++){                                  /* for all variables */
        if( (*it_v)->get_assigned() == true) continue;                                          /* if variable is assigned continue */
        else {
            var = *it_v;
            i = 0;
            for( it_c = clauses->begin() ; it_c != clauses->end() ; it_c++){                    /* for all clauses */
                if((*it_c)->get_assigned() == true ) continue;                                  /* if clause is assigned continue */
                else {
                    for( it_cv = (*it_c)->get_var()->begin() ; it_cv != (*it_c)->get_var()->end() ; it_cv++) { /* for all variables in clause */
                        if(i==2)break;                                                          /* if we have found non pureness break */
                        if( (*it_cv)->get_assigned()==true)continue;                            /* if variable is already assigned continue */
                        else {
                            if( (*it_cv)->get_name() == var->get_name()){                       /* if we found the same variable */
                                if( i == 0 ) {                                                  /* if it's the first time we found the same variable*/
                                    sign = (*it_cv)->get_sign();                                /* get sing */
                                    i = 1;                                                      /* set flag as "we have taken the sign" */
                                } else {
                                    if( (*it_cv)->get_sign() != sign ){                         /* check if the variables have the same sign */
                                        i = 2;                                                  /* if they dont */
                                        break;                                                  /* break */
                                    }
                                }
                            }
                        }
                    }
                    if(i==2)break;
                }
                if(i==2)break;
            }
            if(i == 2) continue;                                                                /* if it's not pure continue with the rest variables */
            if(i == 1 || i==0 ) {                                                               /* if it's pure */
                var->set_pure(true);                                                            /* set it as pure */
                var->set_value(sign);                                                           /* set its value */
                var->set_sign(sign);                                                            /* set the sign */
            }
        }
    }
}

/*
 *	variable* find_first_unassigned(set<variable*>*):
 *		Routine used to pick the first unassigned variable from variable's set
 */
Variable* DPLL::find_first_unassigned(std::set<Variable*> *vars) {
    std::set<Variable*>::iterator it_v;
    for( it_v = vars->begin() ; it_v != vars->end() ; it_v++ )
        if( (*it_v)->get_assigned() == false )return *it_v;
    return NULL;
}

/*
 *	bool ALL_CLAUSES_ARE_TRUE(set<Clause*> *):
 *		Routine used to check whether all clauses of cnf sentence
 *		are true.If so cnf is true else return false.
 */
bool DPLL::ALL_CLAUSES_ARE_TRUE(std::set<Clause*> *clauses){
    std::set<Clause*>::iterator it_c;
    for(it_c = clauses->begin() ; it_c != clauses->end() ; it_c++){                             /* for all clauses */
        if( (*it_c)->get_assigned() == false ) return false;                                    /* if clause is not assigned return false */
        if( (*it_c)->get_assigned() == true && (*it_c)->get_value()== false)return false;       /* if clause is assigned and value is false return false */
    }
    return true;                                                                                /* cnf sentence is true */
}

/*
 *	bool ONE_CLAUSE_IS_FALSE(set<Clause*> *):
 *		Routine used to check whether all clauses of cnf sentence
 *		are true.If so cnf is false else return false.
 */
bool DPLL::ONE_CLAUSE_IS_FALSE(std::set<Clause*> *clauses){
    std::set<Clause*>::iterator it_c;
    for(it_c = clauses->begin() ; it_c != clauses->end() ; it_c++)                              /* for all clauses */
        if( (*it_c)->get_assigned() == true && (*it_c)->get_value()== false){                   /* if clause is assigned and value is false return false */
            return true;
        }
    return false;                                                                               /* cnf sentence is false */
}

/*
 *
 *	variable* FIND_UNIT_CLAUSE(set<Clause*> *,set<variable*> *):
 *		Routine used to find a unit clause in the set of clauses.
 */
Variable* DPLL::FIND_UNIT_CLAUSE(std::set<Clause*> *clauses , std::set<Variable*> *vars) {
    std::set<Clause*>::iterator it_c;
    std::set<Variable*>::iterator it_v;
    Variable *var = NULL;
    int size,v_assigned;
    for(it_c = clauses->begin() ; it_c != clauses->end() ; it_c++) {                            /* for all clauses */
        if((*it_c)->get_assigned() == true)continue;                                            /* if clause is assigned continue with the rest of the clauses */
        size = (*it_c)->get_var()->size();                                                      /* take clause's variables' size */
        v_assigned = 0;
        for( it_v = (*it_c)->get_var()->begin() ; it_v != (*it_c)->get_var()->end() ; it_v++) {	/* for all variables in clauses */
            if( (*it_v)->get_assigned() == true ) {                                             /* if variable is assigned increase counter */
                v_assigned++;
            } else {
                var = *it_v;                                                                    /* else keep a pointer on the variable */
            }
        }
        if(v_assigned == size-1) {                                                              /* if only one variable left then we found a unit */
            (*it_c)->set_value(true);                                                           /* set the value of the clause as true */
            (*it_c)->set_assigned(true);                                                        /* set clause as assigned */
            var->set_assigned(true);                                                            /* set the unit variable as assigned */
            var->set_value(true);                                                               /* set its value as true */
            return var;                                                                         /* return the variable */
        }
    }
    return NULL;
}

/*
 *	variable* FIND_PURE_SYMBOL(set<variable*> *)
 *		routine used to find a pure symbol in the set of variables.
 */
Variable* DPLL::FIND_PURE_SYMBOL(std::set<Variable*> *var){
    std::set<Variable*>::iterator it_v;
    for(it_v = var->begin() ; it_v != var->end() ; it_v++){                                     /* for all variables */
        if((*it_v)->get_pure()==true && (*it_v)->get_assigned() == false) {                     /* if we found a pure and it's not assigned yet */
            (*it_v)->set_assigned(true);                                                        /* set as assigned */
            (*it_v)->set_value(true);
            (*it_v)->set_real_value( (*it_v)->get_sign());                                      /* set the proper value */
            return *it_v;                                                                       /* return the pure variable */
        }
    }
    return NULL;
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
bool DPLL::DPLLalgorithm(std::set<Variable*> *vars, std::set<Clause*> *clauses){
    Variable *var;
    if( ALL_CLAUSES_ARE_TRUE(clauses) == true) return true;
    if( ONE_CLAUSE_IS_FALSE(clauses) == true)  return false;
    fix_pures(vars, clauses);
    var = FIND_PURE_SYMBOL(vars);
    if(var!= NULL) {
        fix_clauses(var,clauses,true,true);
        fix_variables(var,vars,true);
        var->set_value(var->get_sign());
        if( DPLLalgorithm(vars,clauses) == true ) return true;
        else {
            restore_symbol(var);
            restore_clauses(var,clauses);
            return false;
        }
    }
    var = FIND_UNIT_CLAUSE(clauses,vars);
    if( var != NULL ) {
        fix_clauses(var,clauses,var->get_sign(),false);
        fix_variables(var,vars,true);
        if( DPLLalgorithm(vars,clauses) == true ) return true;
        else {
            restore_variables(var,vars);
            restore_clauses(var,clauses);
            restore_symbol(var);
            return false;
        }
    }
    var = find_first_unassigned(vars);
    if( var == nullptr) {
        return false;
    } else {
        return branch_on_variable(var, vars, clauses);
    }
}

/**
 * Handles the branching of DPLL, if we need to make a decision, it either calls the callback function registered in the
 * config or runs both branches recursively
 *
 * The first option is used in the standard setting (sequential)
 * The second option is used in the parallel setting
 */
bool DPLL::branch_on_variable(Variable *var, std::set<Variable *> *vars, std::set<Clause *> *clauses) {
    if (config.callback_on_branch) {
        config.callback(vars);
    }
    var->set_assigned(true);
    var->set_value(true);
    fix_clauses(var, clauses, true, false);
    if (DPLLalgorithm(vars, clauses)) {
        return true;
    } else {
        if (config.callback_on_branch) {
            return false;
        } else {
            var->set_value(true);
            restore_clauses(var, clauses);
            var->set_value(false);
            fix_clauses(var, clauses, false, false);
            if (DPLLalgorithm(vars, clauses)) {
                return true;
            } else {
                restore_clauses(var, clauses);
                restore_symbol(var);
                return false;
            }
        }
    }
}

DPLL::DPLL(CNF _cnf, struct config _config){				/* constructor */
    cnf = new CNF(_cnf);
    config = _config;
}

bool DPLL::DPLL_SATISFIABLE(){
    return DPLLalgorithm(cnf->get_var(),cnf->get_clauses());
}

/*
 *	print(set<Clause*> *, set<variable*> *):
 *		if cnf propositional sentence is satifiable you can
 *		use the -p flag on the command line to print statistics and -pa to print extended.
 *      Extended parameter specifies whether should be printed just variables or variables and clauses.
 *      Format can be either 1 for readable output or 2 for further processing output.
 */
void DPLL::print(std::set<Clause*> *clauses , std::set<Variable*> *vars, bool extended, int format){
    if(format != 1 && format != 2)
        throw std::invalid_argument("Invalid value of format argument: " + std::to_string(format));
    format--;
    std::set<Variable*>::iterator it_vc;
    std::set<Clause*>::iterator it_c;
    std::set<Variable*>::iterator it_v;
    std::string out_values[14] = {": ", "(true) ", "(false) ", "(Unknown) ", "(True)-> ", "(False)-> ", "(Unknown)-> ", " ", "t", "f", "u", "t->", "f->", "u->"};
    for(it_v = vars->begin(); it_v != vars->end(); it_v++) {
        std::cout<<(*it_v)->get_name() << out_values[format * 7];
        if((*it_v)->get_assigned()==true){
            (*it_v)->get_value()==true ? std::cout << out_values[format * 7 + 1] : std::cout << out_values[format * 7 + 2];
        } else {
            std::cout << out_values[format * 7 + 3];
        }
        std::cout << std::endl;
    }

    if(extended)
    {
        for(it_c = clauses->begin() ; it_c != clauses->end() ; it_c++){
            std::cout << "( ";
            (*it_c)->print();
            std::cout << " ): ";
            if((*it_c)->get_assigned()==true) {
                (*it_c)->get_value()==true ? std::cout << out_values[format * 7 + 4] : std::cout << out_values[format * 7 + 5];
            }else{
                std::cout << out_values[format * 7 + 6];
            }
            for(it_vc = (*it_c)->get_var()->begin() ; it_vc != (*it_c)->get_var()->end() ;it_vc++){
                if((*it_vc)->get_sign()==false) std::cout << "~";
                std::cout<<(*it_vc)->get_name();
                if( (*it_vc)->get_assigned()==true){
                    (*it_vc)->get_value()==true ? std::cout << out_values[format * 7 + 1] : std::cout << out_values[format * 7 + 2];
                } else {
                    std::cout << out_values[format * 7 + 3];
                }
            }
            std::cout << std::endl << std::endl;
        }
    }
}
