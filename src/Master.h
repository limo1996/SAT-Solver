//
//  Master.hpp
//  SAT-Solver
//
//  Created by Jakub Lichman on 10/24/17.
//  Copyright © 2017 Jakub Lichman. All rights reserved.
//

#ifndef Master_hpp
#define Master_hpp

#include <queue>
#include <string>

class State{
private:
    unsigned int *variables;
    size_t size;
    unsigned char message_type;
public:
    State();
    State(unsigned int * variables, size_t size, unsigned char message_type);
    unsigned int * get_variables();
    size_t get_size();
    unsigned char get_message_type();
    void set_variables(unsigned int * variables, size_t size);
    void set_message_type(unsigned char type);
};

class Master{
private:
    std::queue<State> states_to_process;
    std::queue<int> available_ranks;
    size_t all_ranks;
    int my_rank;
    
    void send_task_to_worker(State task, int worker);
    void listen_to_worker(int worker);
    void stop_worker(int worker);
public:
    Master(size_t ranks, int my_rank);
    void print_solution(bool to_file, std::string filename, bool extended_format, int format);
};

#endif /* Master_hpp */
