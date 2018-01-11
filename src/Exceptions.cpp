#include <exception>
#include <string>

using namespace std;

class file_open_fail : public exception {
    char* filename;
public:
    file_open_fail(char* _filename):filename(_filename) {}
    virtual const char *what(void) const throw() {
            string out;
            out="File ";
            out+=filename;
            out+=" could not be opened";
            return out.c_str();
    }
};

class file_not_exists : public exception {
    char* filename;
    public:
    file_not_exists(char* _filename):filename(_filename) {}
    virtual const char *what(void) const throw() {
            string out;
            out="File ";
            out+=filename;
            out+=" doesn't exist";
            return out.c_str();
    }
};
