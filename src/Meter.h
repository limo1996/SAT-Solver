
#ifndef METER_H
#define METER_H

#include <chrono>

using namespace std::chrono;

class Meter {
private:
    high_resolution_clock::time_point run_time_start;
    unsigned runtime;

    high_resolution_clock::time_point waiting_start;                                            // time when measurement started
    unsigned waiting_time;

    bool waiting_started;                                                               // indicates whether measurement started
    unsigned send_messages;                                                                      // number of send messages
    unsigned received_messages;                                                                  // number of received messages

public:
    unsigned get_waiting_time() { return waiting_time; }                                          // gets waiting time of worker (for models from master)

    Meter() {
        run_time_start = high_resolution_clock::now();
        waiting_time = 0;
        send_messages = 0;
        received_messages = 0;
        waiting_started = false;
    }

    // starts measurement
    void start_waiting(){
        this->waiting_start = high_resolution_clock::now();
        this->waiting_started = true;
    }

    // stops current measurement if was not stopped and adds time to this->waiting_time
    void stop_waiting(){
        if (this->waiting_started) {
            this->waiting_started = false;
            high_resolution_clock::time_point endTime = high_resolution_clock::now();
            long long duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - waiting_start).count();
            this->waiting_time += (unsigned) duration;
        }
    }

    void stop_runtime() {
        high_resolution_clock::time_point now = high_resolution_clock::now();
        long long duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - run_time_start).count();
        runtime = (unsigned) duration;
    }

    unsigned get_all_messages() { return send_messages + received_messages; }
    int get_send_messages() { return send_messages; }
    int get_recv_messages() { return received_messages; }

    void inc_send_messages(unsigned bytes_sent) { send_messages += bytes_sent; }
    void inc_recv_messages(unsigned bytes_recv) { received_messages += bytes_recv; }

    unsigned get_runtime() {
        return runtime;
    }
};


#endif //METER_H

