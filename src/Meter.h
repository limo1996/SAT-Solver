
#ifndef METER_H
#define METER_H

#include <chrono>

using namespace std::chrono;

/**
 * Contains different timers and can be used to count received and sent messages.
 */
class Meter {
private:
    high_resolution_clock::time_point run_time_start;
    unsigned runtime;

    high_resolution_clock::time_point waiting_start;                                             // time when measurement started
    unsigned long waiting_time;

    bool waiting_started;                                                                        // indicates whether measurement started
    unsigned send_messages;                                                                      // number of send messages
    unsigned received_messages;                                                                  // number of received messages
    unsigned send_meta;                                                                          // number of meta data send

public:
    unsigned get_waiting_time() { return (unsigned) (waiting_time / 1000000); }                // gets waiting time of worker (for models from master)

    Meter() {
        run_time_start = high_resolution_clock::now();
        waiting_time = 0;
        send_messages = 0;
        received_messages = 0;
        send_meta = 0;
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
            long long duration = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - waiting_start).count();
            this->waiting_time += (unsigned long) duration;
        }
    }

    void stop_runtime() {
        high_resolution_clock::time_point now = high_resolution_clock::now();
        long long duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - run_time_start).count();
        runtime = (unsigned) duration;
    }

    unsigned get_all_messages() { return send_messages + received_messages; }
    unsigned get_send_messages() { return send_messages; }
    unsigned get_recv_messages() { return received_messages; }
    unsigned get_send_meta() { return send_meta; }

    void inc_send_messages(unsigned bytes_sent) { send_messages += bytes_sent; }
    void inc_recv_messages(unsigned bytes_recv) { received_messages += bytes_recv; }
    void inc_send_meta_cout() { send_meta++; }

    unsigned get_runtime() {
        return runtime;
    }
};


#endif //METER_H

