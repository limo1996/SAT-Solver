#include <iostream>
#include <fstream>
using namespace std;

int main () {
	string name = "example.txt";
	const char* path = name.c_str();
	if(ifstream(path))
	{
		// std::cout << "File already exists" << std::endl;
		remove(path);
	}

	ofstream myfile;
	myfile.open (path);
	myfile << "hello ziwei.\n";
	myfile.close();
	return 0;
}