//
// Created by jan on 08.12.17.
//

#ifndef SAT_SOLVER_MEASUREMENT_H
#define SAT_SOLVER_MEASUREMENT_H


#include <iostream>
#include <fstream>
#include <vector>
#include <string>

const int NUM_MEASUREMENTS = 4;

/**
 * Used to collect measurements and write measurements to files
 */
class Measurement {
private:
    std::vector<std::string> files;
    std::vector<std::vector<unsigned>> data;
public:
    void add_measurement(std::vector<unsigned> worker_data) {
        data.push_back(worker_data);
    }

    void write_to_files() {
        std::ofstream file;
        for (int m = 0; m < NUM_MEASUREMENTS; m++) {
            file.open(files[m], std::ios_base::app);
            for (int w = 0; w < data.size(); w++) {
                file << data[w][m] << " ";
            }
            file << std::endl;
            file.close();
        }
    }

    explicit Measurement(std::string cnf_path, std::string prefix) {
        size_t last_dot = cnf_path.find_last_of('.');
        std::string raw_name = cnf_path.substr(0, last_dot);
        files.push_back(raw_name + "_" + prefix + ".time");
        files.push_back(raw_name + "_" + prefix + ".wait");
        files.push_back(raw_name + "_" + prefix + ".comm");
        files.push_back(raw_name + "_" + prefix + ".comm2");
    }
};


#endif //SAT_SOLVER_MEASUREMENT_H
