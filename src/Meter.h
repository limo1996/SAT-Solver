
#ifndef METER_H
#define METER_H

#include <chrono>

using namespace std::chrono;

class Meter {
private:
    long long waitingTime;                                                                  // indicates whether this worker was stopped
    high_resolution_clock::time_point startTime;                                            // time when measurement started
    bool measurement_started;                                                               // indicates whether measurement started
    int send_messages;                                                                      // number of send messages
    int received_messages;                                                                  // number of received messages
    
public:
    int get_waiting_time() { return waitingTime; }                                          // gets waiting time of worker (for models from master)
    
    Meter() {
        waitingTime = 0;
        send_messages = 0;
        received_messages = 0;
        measurement_started = false;
    }
    
    // stops current measurement if was not stopped and adds time to this->waitingTime
    void stop_measure(){
        if(!this->measurement_started)
            return;
        
        this->measurement_started = false;
        high_resolution_clock::time_point endTime = high_resolution_clock::now();
        long long duration = std::chrono::duration_cast<std::chrono::milliseconds>( endTime - startTime ).count();
        this->waitingTime += duration;
    }
    
    // starts measurement
    void start_measure(){
        this->startTime = high_resolution_clock::now();
        this->measurement_started = true;
    }
    
    int get_all_messages() { return send_messages + received_messages; }
    int get_send_messages() { return send_messages; }
    int get_recv_messages() { return received_messages; }
    
    void inc_send_messages(int bytes_sent) { send_messages += bytes_sent; }
    void inc_recv_messages(int bytes_recv) { received_messages += bytes_recv; }
};


#endif //METER_H

