/****************************************************************************
 *	Inspired by an implementation by:	Psallidas Fotis                     *
 *	A.M.:1115200600170                                                      *
 *	email:std06170@di.uoa.gr                                                *
 *	Before starting executing and reading                                   *
 * 	the code read the readme.txt file                                       *
 *  http://www.cs.columbia.edu/~fotis/Artificial_Intelligence/dpll/dpll.cpp *
  **************************************************************************/

#ifndef VARIABLE_H
#define VARIABLE_H

#include <string>
#include <set>

/**
 * Represents a literal in a CNF formula
 */
class Variable{
private:
    bool sign;                  /* sign */
    bool value;                 /* value considering the sign */
    bool assigned;              /* flag to know if it is assigned or not */
    unsigned name;			    /* its name */

public:
    Variable(bool _sign, bool _value, unsigned _name);
    Variable(const Variable &v);

    bool get_sign() const;
    bool get_value() const;
    unsigned get_name() const;
    bool get_assigned() const;
    bool get_real_value() const;

    void set_value(bool _value);
    void set_assigned(bool _assigned);
    void set_sign(bool _sign);
    std::string to_string();

    Variable& operator = (const Variable &v);
    bool operator == (const Variable &v);
    void print();

};

#endif // VARIABLE_H
