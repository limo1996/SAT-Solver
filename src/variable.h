#ifndef VARIABLE_H
#define VARIABLE_H

#include <string>

class Variable{
private:
    bool sign;                  /* sign */
    bool value;                 /* value considering the sign */
    bool real_value;            /* real value */
    bool pure;                  /* flag to know if it is pure or not */
    bool assigned;              /* flag to know if it is assigned or not */
    std::string name;			/* its name */

public:
    Variable(bool _sign, bool _value, bool _pure, std::string _name);
    Variable(const Variable &v);
    bool get_sign() const;
    bool get_value() const;
    bool get_pure() const;
    std::string get_name() const;
    bool get_assigned() const;
    bool get_real_value() const;
    void set_value(bool _value);
    void set_pure(bool _pure);
    void set_assigned(bool _assigned);
    void set_real_value(bool _real);
    void set_sign(bool _sign);
    Variable& operator = (const Variable &v);
    bool operator == (const Variable &v);
    void print();
};

#endif // VARIABLE_H
