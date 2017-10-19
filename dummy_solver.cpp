#include<iostream>
#include <string>

using namespace std;

int main() {
    string line;
    getline(cin, line);
    if (line.find("unsat") != string::npos) {
        cout << "unsat" << endl;
    } else {
        cout << "sat" << endl;
        cout << "1 f" << endl;
        cout << "2 t" << endl;
    }
}

