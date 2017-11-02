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

class Variable{
private:
    bool sign;                  /* sign */
    bool value;                 /* value considering the sign */
    bool assigned;              /* flag to know if it is assigned or not */
    std::string name;			/* its name */

public:
    Variable(bool _sign, bool _value, std::string _name);
    Variable(const Variable &v);

    bool get_sign() const;
    bool get_value() const;
    std::string get_name() const;
    bool get_assigned() const;
    bool get_real_value() const;

    void set_value(bool _value);
    void set_assigned(bool _assigned);
    void set_sign(bool _sign);

    Variable& operator = (const Variable &v);
    bool operator == (const Variable &v);
    void print();

};

#endif // VARIABLE_H
