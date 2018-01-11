//
// Created by jan on 08.12.17.
//
#ifndef SAT_SOLVER_ARG_PARSING_H
#define SAT_SOLVER_ARG_PARSING_H

#include <map>

using namespace std;

void parse_args(map<std::string, std::string> *arg_map, int argc, char *argv[]) {
    int count = 1;
    string key;
    string val;
    while (count < argc-1) {
        if (count % 2 == 1) {
            key = string(argv[count]);
        } else {
            val = string(argv[count]);
            if (arg_map->find(key) != arg_map->end()) {
                arg_map->erase(key);
            }
            arg_map->insert({key, val});
        }
        count++;
    }
}

#endif //SAT_SOLVER_ARG_PARSING_H
