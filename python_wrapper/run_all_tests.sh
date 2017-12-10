#!/usr/bin/env bash
function show_relevant() {
    sed -n '/^\[fail\]/p' tmp_out >> tmp_out_2
    sed -n '/^[\crash\]/p' tmp_out >> tmp_out_2
    sed -n '/successful$/p' tmp_out >> tmp_out_2
    sed -i -e 's/^/  /' tmp_out_2
    cat tmp_out_2
    rm tmp_out tmp_out_2
}

echo "Running integration-test-local"
python main.py --mode integration-test-local > tmp_out; show_relevant
echo "Running integration-test-local --cdcl"
python main.py --mode integration-test-local --cdcl > tmp_out; show_relevant
echo "Running parallel-test-local"
python main.py --mode parallel-test-local > tmp_out; show_relevant
echo "Running parallel-test-local --cdcl"
python main.py --mode parallel-test-local --cdcl > tmp_out; show_relevant
echo "Running stealing-test-local"
python main.py --mode stealing-test-local > tmp_out; show_relevant
echo "Running stealing-test-local --cdcl"
python main.py --mode stealing-test-local --cdcl > tmp_out; show_relevant
