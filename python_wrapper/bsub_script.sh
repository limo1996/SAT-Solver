#!/usr/bin/env bash
g++ -std=c++11 src/main.cpp -o main.cpp.o

# yes, this is a nasty as it looks...
cd parallel_tests
FILES=*
for file in ${FILES}
do
    ../main.cpp.o < "$file" > "$file".out
done
cd ../
