#!/usr/bin/env bash
cmake .
make

# yes, this is a nasty as it looks...
cd parallel_tests
FILES=*
for file in ${FILES}
do
    ../parallel_main "$file" -p 2 > "$file".out
done
cd ../
