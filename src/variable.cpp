/****************************************************************************
 *	Inspired by an implementation by:	Psallidas Fotis                     *
 *	A.M.:1115200600170                                                      *
 *	email:std06170@di.uoa.gr                                                *
 *	Before starting executing and reading                                   *
 * 	the code read the readme.txt file                                       *
 *  http://www.cs.columbia.edu/~fotis/Artificial_Intelligence/dpll/dpll.cpp *
  **************************************************************************/

#include "variable.h"

#include <iostream>

//constructor
Variable::Variable(bool _sign, bool _value, std::string _name)
        : sign(_sign), value(_value), name(_name), assigned(false) {}

//copy construcor
Variable::Variable(const Variable &v) {						/* copy constructor */
    sign = v.get_sign();
    value = v.get_value();
    assigned = v.get_assigned();
    name = v.get_name();
}

// gets value of sign
bool Variable::get_sign() const {
    return sign;
}

//gets value
bool Variable::get_value() const {
    return value;
}

//gets value of name
std::string Variable::get_name() const {
    return name;
}

//indicates whether is variable assigned
bool Variable::get_assigned() const {
    return assigned;
}

//gets real value
bool Variable::get_real_value() const{
    return sign ? value : !value;
}

//sets value
void Variable::set_value(bool _value) {
    value = _value;
}

//set whether is variable assigned
void Variable::set_assigned(bool _assigned) {
    assigned = _assigned;
}

//sets sign
void Variable::set_sign(bool _sign){
    sign = _sign;
}

//prints content of variable
void Variable::print(){
    std::cout<<"name: "<< name << std::endl;
    std::cout<<"value: "<< value << std::endl;
}

