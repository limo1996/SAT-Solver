#include <iostream>
#include <chrono>
#include <unistd.h>
#define TIMING
 
#ifdef TIMING
#define INIT_TIMER auto start = std::chrono::high_resolution_clock::now();
#define START_TIMER  start = std::chrono::high_resolution_clock::now();
#define STOP_TIMER(name)  std::cout << "RUNTIME of " << name << ": " << \
    std::chrono::duration_cast<std::chrono::milliseconds>( \
            std::chrono::high_resolution_clock::now()-start \
    ).count() << " ms " << std::endl; 
#else
#define INIT_TIMER
#define START_TIMER
#define STOP_TIMER(name)
#endif
 
int main() {
    for(int k = 0; k < 10; k++){
        INIT_TIMER
        //START_TIMER
        //sleep(2);
        //STOP_TIMER("sleeping for 2 seconds")
        START_TIMER
        long unsigned int b = 0;
        for(int i = 0; i < 10000000; i++) {
            b += i;
        }
        STOP_TIMER("some long loop")
    }
}