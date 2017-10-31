#include <iostream>
#include <chrono>
#include <unistd.h>
#include <fstream>


using namespace std;
using namespace std::chrono;

void function()
{
    long unsigned int b = 0;
    for(int i = 0; i < 10000000; i++) {
        b += i;
    }
}




int main()
{
    int sum = 0;
    int ite = 10;
    int array[ite];

    string name = "example.txt";
    const char* path = name.c_str();
    if(ifstream(path))
    {
        // std::cout << "File already exists" << std::endl;
        remove(path);
    }


    for(int k = 0; k < ite; k++){
        high_resolution_clock::time_point t1 = high_resolution_clock::now();
        function();
        //sleep(2);
        high_resolution_clock::time_point t2 = high_resolution_clock::now();
        //auto duration = duration_cast<microseconds>( t2 - t1 ).count();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>( t2 - t1 ).count();
        sum = sum + duration;
        cout << "RunTime: " << duration << " ms " << std::endl;;
        array[k] = duration;
        
    }
    ofstream myfile;
    myfile.open (path);
    for (int i = 0; i < ite; i++)
        myfile << array[i] << "\n";

    myfile << "Average RunTime: " << sum/ite << " ms " << std::endl;;
    myfile.close();

    return 0;
}