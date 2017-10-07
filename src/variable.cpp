#include "variable.h"

#include <iostream>

//constructor
Variable::Variable(bool _sign, bool _value, bool _pure, std::string _name)
    :sign(_sign),value(_value),name(_name),pure(_pure),assigned(false),real_value(false){}

//copy construcor
Variable::Variable(const Variable &v) {						/* copy constructor */
    sign = v.get_sign();
    value = v.get_value();
    pure = v.get_pure();
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

//gets value of pure
bool Variable::get_pure() const {
    return pure;
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
    return sign == true ? value : !value;
}

//sets value
void Variable::set_value(bool _value) {
    value = _value;
}

//sets whether is variable pure
void Variable::set_pure(bool _pure) {
    pure = _pure;
}

//set whether is variable assigned
void Variable::set_assigned(bool _assigned) {
    assigned = _assigned;
}

//sets real value
void Variable::set_real_value(bool _real){
    real_value = _real;
}

//sets sign
void Variable::set_sign(bool _sign){
    sign = _sign;
}

//operator of assignment
Variable& Variable::operator = (const Variable &v) {
    if(this != &v) {
        this->sign = v.get_sign();
        this->value = v.get_value();
        this->pure = v.get_pure();
        this->name = v.get_name();
    }
    return *this;
}

//equality operator
bool Variable::operator == (const Variable &v) {
    if( this != &v) {
        if(v.get_sign() != this->sign)return false;
        if(v.get_value() != this->value )return false;
        if(v.get_name() != this->name)return false;
        if(v.get_pure() != this->pure)return false;
    }
    return true;
}

//prints content of variable
void Variable::print(){
    std::cout<<"name: "<< name << std::endl;
    std::cout<<"value: "<< value << std::endl;
    std::cout<<"pure: ";
    (pure == true) ? (std::cout << "is_pure" << std::endl << std::endl) : (std::cout << "is not pure" << std::endl << std::endl);
}

